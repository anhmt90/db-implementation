#include "Operator.hpp"

using namespace std;

//unique_ptr<Query>  Operator::init(unique_ptr<Query> query){
//
//	return move(query);
//}

ofstream ofs;

void Operator::setConsumer(Operator* consumer) {
		this->consumer =consumer;
}
/*=========================================================================================================*/

void TableScan::consume(Operator* op) { }

void TableScan::produce() {
	ofs.open("TranslatedQuery.cpp", ofstream::app);

	for(auto& t : consumer->getRequired()){
		if(t.tablename == this->tablename){
			required.push_back(t);
		}
	}

	ofs << "for (auto& t : " << tablename <<".data) { \n";
	ofs<<"\t try{\n";
	consumer->consume(this);
	ofs<<"\t } catch(...){ } \n";
	ofs<< "} \n";

	ofs.close();
	ofs.clear();

}
/*=========================================================================================================*/

void Selection::produce(){
	///this.required is the intersection of attributes between (consumer.required) and (this.produced)
	for(auto& p : this->produced){
		for(auto& r : consumer->getRequired()){
			if(p.name == r.name){
				this->required.push_back(p);
			}
		}
	}
	input->produce();
}

void Selection::consume(Operator* op){
	ofs<<"\t if(";
	for(unsigned z = 0, pred_size = predicates.size(); z < pred_size; ++z){
		const auto& p = predicates[z];
		if(get<0>(p).type == parserTypes::Tag::Varchar || get<0>(p).type == parserTypes::Tag::Char){
			ofs<< "t."+get<0>(p).name+" == "+get<0>(p).typeStr+"::castString(string(\""+get<1>(p).value+"\").c_str(), string(\""+get<1>(p).value+"\").length())";
		}
		else if(get<0>(p).type == parserTypes::Tag::Numeric){
			ofs<< "t."+get<0>(p).name+" == " << get<1>(p).typeStr<< "("<< (stod(get<1>(p).value)*pow(10,get<0>(p).dec)) <<")";
		}
		else{
			ofs<< "t."+get<0>(p).name+" == "+get<1>(p).value;
		}

		if (z+1 < pred_size)
			ofs<<" && ";
	}
	ofs << " ) { \n";
	consumer->consume(this);
	ofs << "\t } \n";
}
/*=========================================================================================================*/
void HashJoin::produce(){
	/*
	 * Vector required of a HashJoin comprises all attributes of equijoins
	 * and the required attributes of the consumer from this operator.
	 *
	 * first, the attributes of equijoins will be placed into required
	 * */
	for(auto& eq : equijoins){
		required.push_back(get<0>(eq));
		required.push_back(get<1>(eq));
	}

	/*
	 * Second, required includes further the attributes required from consumer from 'this'.
	 */
	for(auto& p : this->produced){
		for(auto& cr : consumer->getRequired()){
			if(p.name == cr.name){
				//filter duplicates
				bool dup = false;
				for(auto& r : this->required)
					if(r.name == p.name){
						dup=true;
						break;
					}
				if(!dup)
					this->required.push_back(p);
				break;
			}
		}
	}

	/*
	 * Prepare for this.ht_keys and this.probe_keys
	 * ht_keys stores a list of Attribute types for the keys in the Build phase of the hashtable
	 * ht_keys is the left side (this.equijoins)
	 *
	 * probe_keys is the right side of equijoins
	 * */

	for(auto& join: equijoins){
		this->ht_keys.push_back(join.first);
		this->probe_keys.push_back(join.second);
	}

	/*
	 * Prepare for this.ht_vals
	 * ht_vals stores a list of Attribute types for the values of the hashtable
	 * ht_vals is the intersection of attributes between (consumer.required) and (leftInput.produced)
	 * */
	for(auto& lip : leftInput->produced){
		for(auto& cr : consumer->required){
			if(cr.name == lip.name)
				this->ht_vals.push_back(cr);
		}
	}


	/*
	 * Prepare for ht (HashTable) vector
	 * ht is the union of ht_keys and ht_vals
	 */
	ht.reserve(ht_vals.size() + ht_keys.size());
	ht.insert(ht.end(), ht_vals.begin(), ht_vals.end());

	for(auto& k : ht_keys){
		auto itr = find(ht.begin(), ht.end(), k);
		if(itr == ht.end())
			ht.push_back(k);
	}




	ofs.open("TranslatedQuery.cpp", ofstream::app);
	/*
	 * Declare an unordered_multimap as hash tables
	 * */
	ofs << "unordered_multimap<tuple<";
	for(unsigned z = 0, size = ht_keys.size(); z<size; ++z){
		ofs << ht_keys[z].typeStr;
		(z+1 < size)?ofs << ", ":ofs<< ">, ";
	}
	ofs << "tuple<";

	for(unsigned z = 0, vals_size = ht_vals.size(); z<vals_size; ++z){
		ofs<< ht_vals[z].typeStr;
		if(z+1 < vals_size)
			ofs << ", ";
	}
	ofs<< ">> "<<hashTableName<<"; \n";

	ofs.close();
	ofs.clear();

	/*
	 * Call left and right child
	 */
	leftInput->produce();
	rightInput->produce();
}

void HashJoin::consume(Operator* op){
	/*
	 * LEFT (CHILD) OPERATOR COMES UP!
	 * Build phase of the Hash Table starts
	 */
	if(leftInput == op){
		auto l_vals = leftInput->ht_vals;

		for(auto& v : ht){
			auto it = std::find(l_vals.begin(), l_vals.end(), v);

			if (it == l_vals.end()){
				ofs<<"\t\t auto "<<v.name<<" = t."<<v.name<<";\n";
			}

		}

		ofs <<"\t\t auto tupK = make_tuple(" ;
		for(unsigned i=0, keys_size = ht_keys.size(); i<keys_size; ++i){
			auto& key = ht_keys[i];
			ofs<<key.name;
			if(i+1 < keys_size)
				ofs<<", ";
		}
		ofs<< "); \n";


		ofs <<"\t\t auto tupV = make_tuple(" ;
		for(unsigned i=0, size = ht_vals.size(); i<size; ++i){
			auto& v = ht_vals[i];
			ofs<<v.name;
			if(i+1 < size)
				ofs<<", ";
		}

		ofs<< "); \n";
		ofs<< "\t\t "<<hashTableName<<".insert(make_pair(tupK, tupV)); \n";

	}


	/*
	 * RIGHT (CHILD) OPERATOR COMES UP!
	 * Probe phase of the Hash Table starts
	 */
	else{
		auto r_vals = rightInput->getRequired();

		ofs<< "\t auto range = " << hashTableName <<".equal_range(make_tuple(";
		for(unsigned i=0, size = probe_keys.size(); i<size; ++i){
			auto& key = probe_keys[i];
			ofs<<"t."<<key.name;
			if(i+1 < size)
				ofs<<", ";
		}
		ofs<< ")); \n";
		ofs<< "\t for(auto itr = range.first; itr != range.second; ++itr){ \n";


		for(auto& v : ht){
			auto it = std::find(ht_vals.begin(), ht_vals.end(), v);
			if (it != ht_vals.end()){
				ofs<<"\t\t auto "<<v.name<<" = get<"<<distance(ht_vals.begin(), it)<<">(itr->second); \n";
			}
		}

		consumer->consume(this);
		ofs<< "\t } \n";
	}

}

/*=========================================================================================================*/
void Print::produce(){
	input->produce();
}

void Print::consume(Operator* op){

	for(auto& v : required){
		auto it = std::find(op->ht_vals.begin(), op->ht_vals.end(), v);
		if (it == op->ht_vals.end())
			ofs<<"\t\t auto "<<v.name<<" = t."<<v.name<<";\n";
	}
	// output content
	ofs << "\t std::cout << ";
	for(unsigned i = 0, req_size = required.size(); i< req_size; ++i){
		ofs<<required[i].name;
		if(i+1 < req_size)
			ofs << " <<\"\\t\\t \" <<";
	}

	ofs << "<< endl; \n";
}
/*=========================================================================================================*/
//void Operator::end(){
//
//}


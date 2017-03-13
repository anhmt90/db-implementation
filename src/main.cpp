#include <dlfcn.h>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Attribute.hpp"
#include "Parser.hpp"
#include "Query.hpp"
#include "Schema.hpp"
#include "TPCC_Schema.hpp"

using namespace std;
using namespace std::chrono;

Warehouse::Tuple* _warehouse_tuple;
District::Tuple* _district_tuple;
Customer::Tuple* _customer_tuple;
Neworder::Tuple* _neworder_tuple;
Order::Tuple* _order_tuple;
Orderline::Tuple* _orderline_tuple;
Item::Tuple* _item_tuple;
Stock::Tuple* _stock_tuple;



string firstUpper(string s){
	s[0] = toupper(s[0]);
	return s;
}
string allUpper(string& s){
	string str;
	std::transform(s.begin(), s.end(), back_inserter(str), ::toupper);
	return str;
}

ofstream ofs;


string buff;
void print(string str){
	buff += str;
}

unique_ptr<Schema> generateHPP(unique_ptr<Schema>  schema){
	string str;
	str = "#ifndef H_TPCC_SCHEMA \n"
			"#define H_TPCC_SCHEMA\n\n";
	print(str);

	vector<string> headers = {"<chrono>","<cstddef>", "<cstdint>", "<iostream>", "<vector>", "<unordered_map>","<vector>","<string>", "<tuple>", "<utility>", "<algorithm>",
			"\"Types.hpp\"", "\"Attribute.hpp\"", "\"Table.hpp\"",};
	for(auto& h : headers){
		print("#include "+h+"\n");
	}
	print("using namespace std;\n");
	print("using namespace std::chrono;\n");



	// find all types of pkeys
	vector<pair<int,string>> _pk_types;
	for(const auto& rel : schema->relations) {
		int cnt = 0;
		string pkey;
		for(auto& k : rel.primaryKey){
			if(pkey.empty()) pkey = type(rel.attributes[k]);
			else pkey = pkey + ", " + type(rel.attributes[k]);
			++cnt;
		}
		if(cnt == 0) continue;
		bool existed = false;
		for(auto& key_t : _pk_types){
			if(pkey == key_t.second){ existed = true; break;}
		}
		if(!existed) _pk_types.push_back(make_pair(cnt,pkey));
	}

	// generate templates for hash function
	print("namespace std{\n");
	for(auto& key_t : _pk_types){
		str = "template<> struct hash<";
		if(key_t.first == 1){
			str += key_t.second;
		}
		else str += "tuple<"+key_t.second+">";

		str+= ">{\n"
				"size_t operator()(";

		if(key_t.first == 1){
			str += key_t.second;
		}
		else {
			str += "tuple<"+key_t.second+">";
		}
		str += " const& key) const {\n"
				"size_t hash = 0;\n"
				"hash = hashKey<"+key_t.second+">(";

		if(key_t.first == 1){
			str += "key);\n";
		}
		else{
			for(int i = 0; i<key_t.first; ++i){
				str += "get<"+to_string(i)+">(key)";
				if(i+1 != key_t.first)
					str += ", ";
			}
			str += ");\n";
		}

		str += "return hash;\n }\n };\n";
		print(str);
	}
	print("}\n\n");
	/*
	 * Print tables
	 */
	for(const auto& rel : schema->relations){
		if(rel.primaryKey.size() == 0) continue;
		string tableName = firstUpper(rel.name);
		str = "struct "+tableName+" : public Table{\n"
				"struct Tuple{\n";
		for(auto& a : rel.attributes){
			str += type(a) +" "+a.name+";\n";
		}
		str += "Tuple(){ };\n"
				"Tuple(\n";
		for(int i = 0, _size=rel.attributes.size(); i<_size; ++i){
			str += type(rel.attributes[i]) +" "+rel.attributes[i].name;
			if(i+1 != _size)
				str += ", \n";
		}
		str += ") : \n";
		for(int i = 0, _size=rel.attributes.size(); i<_size; ++i){
			str += rel.attributes[i].name+"("+rel.attributes[i].name+")";
			if(i+1 != _size)
				str += ", ";
		}
		str += "{ }\n };\n";

		str += "vector<"+tableName+"::Tuple> data;\n";
		str += "unordered_map<";


		string pkey;
		for(auto& k : rel.primaryKey){
			if(pkey.empty()) pkey = type(rel.attributes[k]);
			else pkey = pkey + ", " + type(rel.attributes[k]);
		}
		if(rel.primaryKey.size() == 1){
			str += pkey;
		} else {
			str += "tuple<"+pkey+">";
		}
		str += ", uint64_t> pk_index;\n";

		// Constructor
		str += tableName+"(){\n"
				"this->name = \""+rel.name+"\";\n"
				"tables.push_back(*this);\n"
				"import();\n"
				"}\n";
		str += "virtual ~"+tableName+"(){};\n"
				"void import();\n";
		str += "Tuple* read(";
		if(rel.primaryKey.size() == 1){
			str += pkey;
		} else {
			str += "tuple<"+pkey+">";
		}
		str += ");\n";
		str += "Tuple* insert(Tuple*);\n";
		str += "bool remove(";
		if(rel.primaryKey.size() == 1){
			str += pkey;
		} else {
			str += "tuple<"+pkey+">";
		}
		str += ");\n";
		str += "};\n\n\n";

		print(str);
	}



	for(const auto& rel : schema->relations){
		if(rel.primaryKey.size() == 0) continue;
		str = "extern " + firstUpper(rel.name) + " " + rel.name + ";\n";
		print(str);
	}
	str = "#endif";
	print(str);


	return move(schema);

}
/*-----------------------------------------------------------------------------------------------*/
unique_ptr<Schema> generateCPP(unique_ptr<Schema>  schema){
	string str;
	vector<string> headers = {"\"TPCC_Schema.hpp\""};
	for(auto& h : headers){
		print("#include "+h+"\n");
	}
	print("\nusing namespace std;\n\n");

	for(const auto& rel : schema->relations){
		if(rel.primaryKey.size() == 0) continue;
		str = firstUpper(rel.name) + " " + rel.name + ";\n";
		print(str);
	}

	str = "\nvoid close_ifstream(ifstream& itbl) {\n"
			"itbl.close();\n"
			"itbl.clear();\n"
			"}\n\n";
	print(str);

	str = "\nvector<string> split(const string &s) {\n"
			"vector<string> splitted;\n"
			"stringstream ss(s);\n"
			"string item;\n"
			"while (std::getline(ss, item, '|')) {\n"
			"splitted.push_back(item);\n}\n"
			"return splitted;\n"
			"}\n\n";
	print(str);

	/// generate import() functions
	for(const auto& rel : schema->relations){
		if(rel.primaryKey.size() == 0) continue;
		auto tableName = firstUpper(rel.name);
		auto _size_a = rel.attributes.size();
		auto _size_k = rel.primaryKey.size();
		str = "void "+tableName+"::import(){\n"
				"ifstream itbl(\"tbl/tpcc_"+rel.name+".tbl\");\n";
		str += "if (!itbl) {\n"
				"cerr << \"tpcc_"+rel.name+".tbl could not be opened! :(\" << endl;\n"
				"exit(1);\n"
				"}\n";
		str += "else{\n"
				"string line;\n"
				"if (itbl.is_open()) {\n"
				"while (getline(itbl, line)) {\n"
				"vector<string> elm = split(line);\n";

		str += tableName+"::Tuple row;\n";
		auto a = rel.attributes;
		for(uint32_t i =0; i<_size_a; ++i){
			str += "row."+ a[i].name+" = row."+a[i].name+".castString(elm["+to_string(i)+"].c_str(), elm["+to_string(i)+"].length());\n";
			if((i == _size_k && i>1) || (_size_a == _size_k && i+1 == _size_k)){
				str += "auto tup = make_tuple(";
				for(int k = _size_k -1; k>=0; --k){
					str += "row."+a[rel.primaryKey[k]].name;
					if(k-1 != -1)
						str += ", ";
				}
				str += ");\n";
			}
		}
		str += "data.push_back(row);\n";
		str += "pk_index.insert(make_pair(";
		if(rel.primaryKey.size() == 1){
			str += "row."+rel.attributes[0].name+", ";
		} else if(rel.primaryKey.size() > 1){
			str += "tup, ";
		}

		str += "data.size() - 1));\n"
				"}\n";
		for(auto& a : rel.attributes){
			str += "tables.back().attributes.push_back(Attribute(\""+a.name+"\", \""+type(a)+"\", \""+a.tablename+"\"));\n";
		}
		str += "}\n"
				"close_ifstream(itbl);\n";
		str += "cout << \""+tableName+" imported!\\n\";\n"
				"}\n"
				"}\n\n\n";

		print(str);
	}

	/// generate read functions
	for(const auto& rel : schema->relations){
		if(rel.primaryKey.size() == 0) continue;
		auto tableName = firstUpper(rel.name);
		auto a = rel.attributes;
		auto _size_k = rel.primaryKey.size();
		str = tableName+"::Tuple* "+tableName+"::read(";
		if(_size_k == 1){
			str += type(rel.attributes[0])+" pkey){\n";
		} else if(_size_k > 1){
			str += "tuple<";
			for(int k = _size_k -1; k>=0; --k){
				str += type(a[k]);
				if(k-1 != -1)
					str += ", ";
			}
			str += "> pkey){\n";
		}
		str += "auto itr = pk_index.find(pkey);\n"
				"if(itr == pk_index.end()) return nullptr;\n"
				"return &data[itr->second];\n}\n\n";
		print(str);
	}
	/// generate insert functions
	for(const auto& rel : schema->relations){
		if(rel.primaryKey.size() == 0) continue;
		auto tableName = firstUpper(rel.name);
		auto a = rel.attributes;
		auto _size_k = rel.primaryKey.size();
		str = tableName+"::Tuple* "+tableName+"::insert("+tableName+"::Tuple* t){\n";
		str += "auto pkey = ";
		if(_size_k == 1){
			str += "t->"+a[0].name+";\n";
		} else if(_size_k > 1){
			str += "make_tuple(";
			for(uint32_t k = 0; k<_size_k; ++k){
				str += "t->"+a[k].name;
				if(k+1 != _size_k)
					str += ", ";
			}
			str += ");\n";
		}

		str += "if(pk_index.find(pkey) == pk_index.end()){\n"
				"data.push_back(*t);\n"
				"pk_index.insert(make_pair(pkey, data.size()-1));\n"
				"return &data.back();\n"
				"}\n"
				"return nullptr;\n"
				"}\n\n";
		print(str);
	}


	/// generate remove functions
	for(const auto& rel : schema->relations){
		if(rel.primaryKey.size() == 0) continue;
		auto tableName = firstUpper(rel.name);
		auto a = rel.attributes;
		auto _size_k = rel.primaryKey.size();
		str = "bool "+tableName+"::remove(";
		if(_size_k == 1){
			str += type(rel.attributes[0])+" pkey){\n";
		} else if(_size_k > 1){
			str += "tuple<";
			for(uint32_t k = 0; k<_size_k; ++k){
				str += type(a[k]);
				if(k+1 != _size_k)
					str += ", ";
			}
			str += "> pkey){\n";
		}
		str += "auto itr = pk_index.find(pkey);\n"
				"if(itr != pk_index.end()){\n"
				"auto index = itr->second;\n"
				"if(index == data.size()-1){\n"
				"data.pop_back();\n"
				"pk_index.erase(itr);\n"
				"return true;\n"
				"}\n"
				"std::swap(data.back(), data[index]);\n"
				"data.pop_back();\n"
				"pk_index.erase(itr);\n"
				"itr = pk_index.find(";

		if(_size_k == 1){
			str += "data[index]."+rel.attributes[0].name;
		} else if(_size_k > 1){
			str += "make_tuple(";
			for(uint32_t k = 0; k<_size_k; ++k){
				str += "data[index]."+a[k].name;
				if(k+1 != _size_k)
					str += ", ";
			}
			str += ")";
		}
		str += ");\n";
		str += "itr->second = index;\n"
				"return true;\n"
				"}\n"
				"return false;\n"
				"}\n\n\n";
		print(str);
	}

	return move(schema);
}

void delivery(int32_t w_id, int32_t o_carrier_id, Timestamp datetime){
	for(int32_t d_id = 1; d_id<=10; ++d_id){
		Integer _min = INT32_MAX;
		for(auto& _neworder : neworder.data){
			if(_neworder.no_d_id == d_id && _neworder.no_w_id == w_id)
				_min = min(_min,_neworder.no_o_id);
		}
		if(_min == INT32_MAX)
			continue;

		Integer o_id = _min;
		// delete
//		if(neworder.remove(Predicate(o_id, d_id, w_id).pk_3int) == false) return; // uncomment this to abort the transaction in edge case
		neworder.remove(make_tuple(o_id, Integer(d_id), Integer(w_id)));
		// select
		if(!(_order_tuple = order.read(make_tuple(o_id, Integer(d_id), Integer(w_id))))) return;
		auto o_ol_cnt = _order_tuple->o_ol_cnt;
		auto o_c_id = _order_tuple->o_c_id;
		// update
		if((_order_tuple = order.read(make_tuple(o_id, Integer(d_id), Integer(w_id))))){
			_order_tuple->o_carrier_id = o_carrier_id;
		}
		Numeric<6,2> ol_total=0;
		for(int32_t ol_number = 1; Numeric<2,0>(ol_number) <= o_ol_cnt; ol_number++){
			if(!(_orderline_tuple = orderline.read(make_tuple(o_id, Integer(d_id), Integer(w_id), Integer(ol_number))))) return;
			auto ol_amount = _orderline_tuple->ol_amount;
			ol_total = ol_total + ol_amount;
			//update
			if((_orderline_tuple = orderline.read(make_tuple(o_id, Integer(d_id), Integer(w_id), Integer(ol_number))))){
				_orderline_tuple->ol_delivery_d = datetime;
			}
		}
		// update
		if((_customer_tuple = customer.read(make_tuple(o_c_id, Integer(d_id), Integer(w_id))))){
			_customer_tuple->c_balance = _customer_tuple->c_balance + ol_total.castS<12>();
		}

	}
}



void newOrder(int32_t w_id, int32_t d_id, int32_t c_id, int32_t items, int32_t supware[15], int32_t itemid[15], int32_t qty[15], Timestamp datetime){

	if(!(_warehouse_tuple = warehouse.read(w_id))) return;
	auto w_tax = _warehouse_tuple->w_tax;

	if(!(_customer_tuple = customer.read(make_tuple(Integer(c_id), Integer(d_id), Integer(w_id))))) return;
	auto c_discount = _customer_tuple->c_discount;

	if(!(_district_tuple = district.read(make_tuple(d_id, w_id)))) return;
	auto o_id = _district_tuple->d_next_o_id;
	auto d_tax = _district_tuple->d_tax;

	uint32_t all_local = 1;
	for(int index = 0; index <= items-1; ++index){
		if(w_id != supware[index])
			all_local = 0;
	}
	// insert
	_order_tuple = order.insert(new Order::Tuple(o_id, d_id, w_id, c_id, datetime, 0, items, all_local));
	//	if(!(_order_tuple = order.insert(new Order::Tuple(o_id, d_id, w_id, c_id, datetime, 0, items, all_local)))) return; // uncomment this to abort the transaction in edge case
	_neworder_tuple = neworder.insert(new Neworder::Tuple(o_id, d_id, w_id));
	//	if(!(_neworder_tuple = neworder.insert(new NewOrder::Tuple(o_id, d_id, w_id)))) return; // uncomment this to abort the transaction in edge case

	for(int index = 0; index <= items -1 ; ++index){
		if(!(_item_tuple = item.read(itemid[index]))) return;
		auto i_price = _item_tuple->i_price;

		if(!(_stock_tuple = stock.read(make_tuple(itemid[index], supware[index])))) return;
		auto s_quantity = _stock_tuple->s_quantity;
		auto s_remote_cnt = _stock_tuple->s_remote_cnt;
		auto s_order_cnt = _stock_tuple->s_order_cnt;
		Char<24> s_dist;
		switch(d_id){
		case 1 : s_dist = _stock_tuple->s_dist_01; break;
		case 2 : s_dist = _stock_tuple->s_dist_02; break;
		case 3 : s_dist = _stock_tuple->s_dist_03; break;
		case 4 : s_dist = _stock_tuple->s_dist_04; break;
		case 5 : s_dist = _stock_tuple->s_dist_05; break;
		case 6 : s_dist = _stock_tuple->s_dist_06; break;
		case 7 : s_dist = _stock_tuple->s_dist_07; break;
		case 8 : s_dist = _stock_tuple->s_dist_08; break;
		case 9 : s_dist = _stock_tuple->s_dist_09; break;
		case 10 : s_dist = _stock_tuple->s_dist_10; break;
		}
		//update
		if(s_quantity > qty[index]){
			if((_stock_tuple = stock.read(make_tuple(itemid[index], supware[index]))))
				_stock_tuple->s_quantity = s_quantity - qty[index];
		} else {
			if((_stock_tuple = stock.read(make_tuple(itemid[index], supware[index]))))
				_stock_tuple->s_quantity = s_quantity + 91 - qty[index];
		}

		if(supware[index] != w_id){
			if((_stock_tuple = stock.read(make_tuple(itemid[index], supware[index]))))
				_stock_tuple->s_remote_cnt = s_remote_cnt + 1;
		} else {
			if((_stock_tuple = stock.read(make_tuple(itemid[index], supware[index]))))
				_stock_tuple->s_order_cnt = s_order_cnt + 1;
		}

		Numeric<6,2> ol_amount = qty[index] * i_price.getDouble() * (1 + w_tax.getDouble() + d_tax.getDouble()) * (1 - c_discount.getDouble());
		if(!(_orderline_tuple = orderline.insert(new Orderline::Tuple(o_id, d_id, w_id, index+1, itemid[index], supware[index], 0, qty[index], ol_amount, s_dist)))) return;
	}
	//-----end for -----//
}


const int32_t warehouses=5;

int32_t urand(int32_t min,int32_t max) {
	return (random()%(max-min+1))+min;
}

int32_t urandexcept(int32_t min,int32_t max,int32_t v) {
	if (max<=min)
		return min;
	int32_t r=(random()%(max-min))+min;
	if (r>=v)
		return r+1; else
			return r;
}

int32_t nurand(int32_t A,int32_t x,int32_t y) {
	return ((((random()%A)|(random()%(y-x+1)+x))+42)%(y-x+1))+x;
}

void newOrderRandom(Timestamp now) {
	int32_t w_id=urand(1,warehouses);
	int32_t d_id=urand(1,10);
	int32_t c_id=nurand(1023,1,3000);
	int32_t ol_cnt=urand(5,15);

	int32_t supware[15];
	int32_t itemid[15];
	int32_t qty[15];
	for (int32_t i=0; i<ol_cnt; i++) {
		if (urand(1,100)>1)
			supware[i]=w_id; else
				supware[i]=urandexcept(1,warehouses,w_id);
		itemid[i]=nurand(8191,1,100000);
		qty[i]=urand(1,10);
	}

	newOrder(w_id,d_id,c_id,ol_cnt,supware,itemid,qty,now);
}

void deliveryRandom(Timestamp now) {
	delivery(urand(1,warehouses),urand(1,10),now);
}

void oltp(Timestamp now) {
	int rnd=urand(1,100);
	if (rnd<=10) {
		deliveryRandom(now);
	} else {
		newOrderRandom(now);
	}
}


/*=========================================================================================================*/
int main() {
	SchemaParser sp("schema.sql");
	QueryParser qp;

	///Parse the Schema
	std::unique_ptr<Schema> schema = sp.parse();
//	std::cout << schema->toString() << std::endl; // uncomment to print the parsed data of the schema

	/*
	 * The generated code is in the root folder
	 */
	bool _use_generated_code = false;
	if(_use_generated_code){
		schema = generateHPP(move(schema));
		ofs.open("./src/TPCC_Schema.hpp", ofstream::out);;
		ofs<< buff;
		ofs.clear();
		ofs.close();
		buff = "";


		schema = generateCPP(move(schema));
		ofs.open("./src/TPCC_Schema.cpp", ofstream::out);;
		ofs<< buff;
		ofs.clear();
		ofs.close();
	}
	else {
		/*=========================================================================================================*/
		size_t times = 1000000;

		cout << endl;
		cout << "PK Index of Order: " << order.pk_index.size() << " indexes\n";
		cout << "PK Index of NewOrder: " << neworder.pk_index.size() << " indexes\n";
		cout << "PK Index of OrderLine: " << orderline.pk_index.size() << " indexes\n\n";

		cout << "Order: " << order.data.size() << " tuples\n";
		cout << "NewOrder: " << neworder.data.size() << " tuples\n";
		cout << "OrderLine: " << orderline.data.size() << " tuples\n";
		cout << "Executing "<< times<< " transactions newOrder and delivery mixed... \n\n";
		auto start=high_resolution_clock::now();
		for(size_t i = 0; i<times; i++){
			//		newOrderRandom(0);
			oltp(0);
			//		deliveryRandom(0);
			//		cout << fixed<< sumQuery() <<"!\n";
		}
		auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
		cout << "Transactions accomplished in " << end << "s\n";
		cout << "Speed: " << times/(double)end << " transactions/s\n";

		cout << endl;
		cout << "Order: " << order.data.size() << " tuples\n";
		cout << "NewOrder: " << neworder.data.size() << " tuples\n";
		cout << "OrderLine: " << orderline.data.size() << " tuples\n";

		cout << "PK Index of Order: " << order.pk_index.size() << " indexes\n";
		cout << "PK Index of NewOrder: " << neworder.pk_index.size() << " indexes\n";
		cout << "PK Index of OrderLine: " << orderline.pk_index.size() << " indexes\n\n";
	}
}








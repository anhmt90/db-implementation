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
NewOrder::Tuple* _neworder_tuple;
Order::Tuple* _order_tuple;
OrderLine::Tuple* _orderline_tuple;
Item::Tuple* _item_tuple;
Stock::Tuple* _stock_tuple;


//std::string type(const Attribute& attr) {
//   parserTypes::Tag type = attr.type;
//   switch(type) {
//      case parserTypes::Tag::Integer:
//         return "Integer";
//
//      case parserTypes::Tag::Numeric: {
//         std::stringstream ss;
//         ss << "Numeric<" << attr.len << "," << attr.dec << ">";
//         return ss.str();
//      }
//      case parserTypes::Tag::Char: {
//         std::stringstream ss;
//         ss << "Char<" << attr.len << ">";
//         return ss.str();
//      }
//      case parserTypes::Tag::Varchar: {
//    	  std::stringstream ss;
//    	  ss << "Varchar<" << attr.len << ">";
//    	  return ss.str();
//      }
//      case parserTypes::Tag::Timestamp: {
//    	  std::stringstream ss;
//    	  return "Timestamp";
//      }
//   }
//   throw;
//}

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
void printHPP(string str){
	buff += str;
}

unique_ptr<Schema> generateHPP(unique_ptr<Schema>  schema){
	string str;
	str = "#ifndef H_TPCC_SCHEMA \n"
			"#define H_TPCC_SCHEMA\n\n";
	printHPP(str);

	vector<string> headers = {"<fstream>", "<sstream>", "<unordered_map>","<vector>","<string>", "<tuple>", "<utility>", "<algorithm>",
			"\"Types.hpp\""};
	for(auto& h : headers){
		printHPP("#include "+h+"\n");
	}
	printHPP("using namespace std;\n");
	printHPP("using namespace std::chrono;\n");



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


	printHPP("namespace std{\n");
	for(auto& key_t : _pk_types){
		str = "template<> struct hash<"+key_t.second+">{\n"
				"size_t operator()(";
		if(key_t.first == 1){
			str += key_t.second;
		}
		else {
			str += "tuple<"+key_t.second+">";
		}
		str += " const& key) const {\n"
				"size_t hash = 0;\n"
				"hash = hashKey<"+key_t.second+"(";

		if(key_t.first == 1){
			str += "key);\n";
		}
		else{
			for(int i = 0; i<key_t.first; ++i){
				str += "get<"+to_string(i)+">(key), ";
				if(i+1 == key_t.first)
					str += ");\n";
			}
		}

		str += "return hash;\n }\n };\n\n";
		printHPP(str);
	}


	/*
	 * Print tables
	 */
	for(const auto& rel : schema->relations){
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
				"}";
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

		printHPP(str);
	}



	for(const auto& rel : schema->relations){
		str = "extern " + firstUpper(rel.name) + " " + rel.name + ";\n";
		printHPP(str);
	}
	str = "#endif";
	printHPP(str);

//	str = "vector<string> split(const string &s) {\n"
//			"vector<string> splitted;\n"
//			"stringstream ss(s);\n"
//			"string item;\n"
//			"while (std::getline(ss, item, '|')) {\n"
//			"splitted.push_back(item);\n}\nreturn splitted;\n"
//			"}\n";
//
//	printHPP(str);

	return move(schema);

}

/*=========================================================================================================*/
int main() {
	SchemaParser sp("schema.sql");
	QueryParser qp;

	///Parse the Schema
	std::unique_ptr<Schema> schema = sp.parse();
	std::cout << schema->toString() << std::endl; // uncomment to print the parsed data of the schema

	schema = generateHPP(move(schema));
	ofs.open("Generated_Schema.hpp", ofstream::app);;
	ofs<< buff;
	ofs.clear();
	ofs.close();

	/*=========================================================================================================*/
//	size_t times = 1000000;
//
//	cout << endl;
//	cout << "PK Index of Order: " << order.pk_index.size() << " indexes\n";
//	cout << "PK Index of NewOrder: " << neworder.pk_index.size() << " indexes\n";
//	cout << "PK Index of OrderLine: " << orderline.pk_index.size() << " indexes\n\n";
//
//	cout << "Order: " << order.data.size() << " tuples\n";
//	cout << "NewOrder: " << neworder.data.size() << " tuples\n";
//	cout << "OrderLine: " << orderline.data.size() << " tuples\n";
//	cout << "Executing "<< times<< " transactions newOrder and delivery mixed... \n\n";
//	auto start=high_resolution_clock::now();
//	for(size_t i = 0; i<times; i++){
//		//		newOrderRandom(0);
//		oltp(0);
//		//		deliveryRandom(0);
//		//		cout << fixed<< sumQuery() <<"!\n";
//	}
//	auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
//	cout << "Transactions accomplished in " << end << "s\n";
//	cout << "Speed: " << times/(double)end << " transactions/s\n";
//
//	cout << endl;
//	cout << "Order: " << order.data.size() << " tuples\n";
//	cout << "NewOrder: " << neworder.data.size() << " tuples\n";
//	cout << "OrderLine: " << orderline.data.size() << " tuples\n";
//
//	cout << "PK Index of Order: " << order.pk_index.size() << " indexes\n";
//	cout << "PK Index of NewOrder: " << neworder.pk_index.size() << " indexes\n";
//	cout << "PK Index of OrderLine: " << orderline.pk_index.size() << " indexes\n\n";
}







//void delivery(int32_t w_id, int32_t o_carrier_id, Timestamp datetime){
//	for(int32_t d_id = 1; d_id<=10; ++d_id){
//		Integer _min = INT32_MAX;
//		for(auto& _neworder : neworder.data){
//			if(_neworder.no_d_id == d_id && _neworder.no_w_id == w_id)
//				_min = min(_min,_neworder.no_o_id);
//		}
//		if(_min == INT32_MAX)
//			continue;
//
//		Integer o_id = _min;
//		// delete
////		if(neworder.remove(Predicate(o_id, d_id, w_id).pk_3int) == false) return; // uncomment this to abort the transaction in edge case
//		neworder.remove(Predicate(o_id, d_id, w_id).pk_3int);
//		// select
//		if(!(_order_tuple = order.read(Predicate(o_id, d_id, w_id).pk_3int))) return;
//		auto o_ol_cnt = _order_tuple->o_ol_cnt;
//		auto o_c_id = _order_tuple->o_c_id;
//		// update
//		if((_order_tuple = order.read(Predicate(o_id, d_id, w_id).pk_3int))){
//			_order_tuple->o_carrier_id = o_carrier_id;
//		}
//		Numeric<6,2> ol_total=0;
//		for(int32_t ol_number = 1; Numeric<2,0>(ol_number) <= o_ol_cnt; ol_number++){
//			if(!(_orderline_tuple = orderline.read(Predicate(o_id, d_id, w_id, ol_number).pk_4int))) return;
//			auto ol_amount = _orderline_tuple->ol_amount;
//			ol_total = ol_total + ol_amount;
//			//update
//			if((_orderline_tuple = orderline.read(Predicate(o_id, d_id, w_id, ol_number).pk_4int))){
//				_orderline_tuple->ol_delivery_d = datetime;
//			}
//		}
//		// update
//		if((_customer_tuple = customer.read(Predicate(o_c_id, d_id, w_id).pk_3int))){
//			_customer_tuple->c_balance = _customer_tuple->c_balance + ol_total.castS<12>();
//		}
//
//	}
//}
//
//
//
//void newOrder(int32_t w_id, int32_t d_id, int32_t c_id, int32_t items, int32_t supware[15], int32_t itemid[15], int32_t qty[15], Timestamp datetime){
//
//	if(!(_warehouse_tuple = warehouse.read(Predicate(w_id).pk_int))) return;
//	auto w_tax = _warehouse_tuple->w_tax;
//
//	if(!(_customer_tuple = customer.read(Predicate(c_id, d_id, w_id).pk_3int))) return;
//	auto c_discount = _customer_tuple->c_discount;
//
//	if(!(_district_tuple = district.read(Predicate(d_id, w_id).pk_2int))) return;
//	auto o_id = _district_tuple->d_next_o_id;
//	auto d_tax = _district_tuple->d_tax;
//
//	uint32_t all_local = 1;
//	for(int index = 0; index < items; ++index){
//		if(w_id != supware[index])
//			all_local = 0;
//	}
//	// insert
//	_order_tuple = order.insert(new Order::Tuple(o_id, d_id, w_id, c_id, datetime, 0, items, all_local));
//	//	if(!(_order_tuple = order.insert(new Order::Tuple(o_id, d_id, w_id, c_id, datetime, 0, items, all_local)))) return; // uncomment this to abort the transaction in edge case
//	_neworder_tuple = neworder.insert(new NewOrder::Tuple(o_id, d_id, w_id));
//	//	if(!(_neworder_tuple = neworder.insert(new NewOrder::Tuple(o_id, d_id, w_id)))) return; // uncomment this to abort the transaction in edge case
//
//	for(int index = 0; index <= items; ++index){
//		if(!(_item_tuple = item.read(Predicate(itemid[index]).pk_int))) return;
//		auto i_price = _item_tuple->i_price;
//
//		if(!(_stock_tuple = stock.read(Predicate(itemid[index], supware[index]).pk_2int))) return;
//		auto s_quantity = _stock_tuple->s_quantity;
//		auto s_remote_cnt = _stock_tuple->s_remote_cnt;
//		auto s_order_cnt = _stock_tuple->s_order_cnt;
//		Char<24> s_dist;
//		switch(d_id){
//		case 1 : s_dist = _stock_tuple->s_dist_01; break;
//		case 2 : s_dist = _stock_tuple->s_dist_02; break;
//		case 3 : s_dist = _stock_tuple->s_dist_03; break;
//		case 4 : s_dist = _stock_tuple->s_dist_04; break;
//		case 5 : s_dist = _stock_tuple->s_dist_05; break;
//		case 6 : s_dist = _stock_tuple->s_dist_06; break;
//		case 7 : s_dist = _stock_tuple->s_dist_07; break;
//		case 8 : s_dist = _stock_tuple->s_dist_08; break;
//		case 9 : s_dist = _stock_tuple->s_dist_09; break;
//		case 10 : s_dist = _stock_tuple->s_dist_10; break;
//		}
//		//update
//		if(s_quantity > qty[index]){
//			if((_stock_tuple = stock.read(Predicate(itemid[index], supware[index]).pk_2int)))
//				_stock_tuple->s_quantity = s_quantity - qty[index];
//		} else {
//			if((_stock_tuple = stock.read(Predicate(itemid[index], supware[index]).pk_2int)))
//				_stock_tuple->s_quantity = s_quantity + 91 - qty[index];
//		}
//
//		if(supware[index] != w_id){
//			if((_stock_tuple = stock.read(Predicate(itemid[index], supware[index]).pk_2int)))
//				_stock_tuple->s_remote_cnt = s_remote_cnt + 1;
//		} else {
//			if((_stock_tuple = stock.read(Predicate(itemid[index], supware[index]).pk_2int)))
//				_stock_tuple->s_order_cnt = s_order_cnt + 1;
//		}
//
//		Numeric<6,2> ol_amount = qty[index] * i_price.getDouble() * (1 + w_tax.getDouble() + d_tax.getDouble()) * (1 - c_discount.getDouble());
//		if(!(_orderline_tuple = orderline.insert(new OrderLine::Tuple(o_id, d_id, w_id, index+1, itemid[index], supware[index], 0, qty[index], ol_amount, s_dist)))) return;
//	}
//	//-----end for -----//
//}
//
//
//const int32_t warehouses=5;
//
//int32_t urand(int32_t min,int32_t max) {
//	return (random()%(max-min+1))+min;
//}
//
//int32_t urandexcept(int32_t min,int32_t max,int32_t v) {
//	if (max<=min)
//		return min;
//	int32_t r=(random()%(max-min))+min;
//	if (r>=v)
//		return r+1; else
//			return r;
//}
//
//int32_t nurand(int32_t A,int32_t x,int32_t y) {
//	return ((((random()%A)|(random()%(y-x+1)+x))+42)%(y-x+1))+x;
//}
//
//void newOrderRandom(Timestamp now) {
//	int32_t w_id=urand(1,warehouses);
//	int32_t d_id=urand(1,10);
//	int32_t c_id=nurand(1023,1,3000);
//	int32_t ol_cnt=urand(5,15);
//
//	int32_t supware[15];
//	int32_t itemid[15];
//	int32_t qty[15];
//	for (int32_t i=0; i<ol_cnt; i++) {
//		if (urand(1,100)>1)
//			supware[i]=w_id; else
//				supware[i]=urandexcept(1,warehouses,w_id);
//		itemid[i]=nurand(8191,1,100000);
//		qty[i]=urand(1,10);
//	}
//
//	newOrder(w_id,d_id,c_id,ol_cnt,supware,itemid,qty,now);
//}
//
//void deliveryRandom(Timestamp now) {
//	delivery(urand(1,warehouses),urand(1,10),now);
//}
//
//void oltp(Timestamp now) {
//	int rnd=urand(1,100);
//	if (rnd<=10) {
//		deliveryRandom(now);
//	} else {
//		newOrderRandom(now);
//	}
//}

/*
 * ImportDB.cpp
 *
 *  Created on: Oct 24, 2016
 *      Author: anhmt90
 */
#ifndef H_TPCC_SCHEMA
#define H_TPCC_SCHEMA

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>

#include <algorithm>
#include <chrono>

#include "Types.hpp"
#include "Table.hpp"
using namespace std;
using namespace std::chrono;
//using namespace types;

typedef tuple<Integer, Integer> tup_2Int;
typedef tuple<Integer, Integer, Integer> tup_3Int;
typedef tuple<Integer, Integer, Integer, Integer> tup_4Int;

/*-----------------------------------------------------------------------------------------------------------------------*/
namespace std {

template<>
struct hash<Integer> {
	size_t operator()(Integer const& key) const {
		size_t hash = 0;
		hash = hashKey<Integer>(key);
		return hash;
	}
};

template<>
struct hash<tuple<Integer>> {
	size_t operator()(tuple<Integer> const& key) const {
		size_t hash = 0;
		hash = hashKey<Integer>(get<0>(key));
		return hash;
	}
};

template<>
struct hash<tuple<Integer, Integer>> {
	size_t operator()(tuple<Integer, Integer> const& key) const {
		size_t hash = 0;
		hash = hashKey<Integer, Integer>(get<0>(key), get<1>(key));
		return hash;
	}
};

template<>
struct hash<tuple<Integer, Integer, Integer>> {
	size_t operator()(tuple<Integer, Integer, Integer> const& key) const {
		size_t hash = 0;
		hash = hashKey<Integer, Integer, Integer>(get<0>(key), get<1>(key),
				get<2>(key));
		return hash;
	}
};

template<>
struct hash<tuple<Integer, Integer, Integer, Integer>> {
	size_t operator()(
			tuple<Integer, Integer, Integer, Integer> const& key) const {
		size_t hash = 0;
		hash = hashKey<Integer, Integer, Integer, Integer>(get<0>(key),
				get<1>(key), get<2>(key), get<3>(key));
		return hash;
	}
};

template<>
struct hash<tuple<Integer, Integer, Varchar<16>, Varchar<16>>> {
	size_t operator()(
			tuple<Integer, Integer, Varchar<16>, Varchar<16>> const& key) const {
		size_t hash = 0;
		hash = hashKey<Integer, Integer, Varchar<16>, Varchar<16>>(get<0>(key),
				get<1>(key), get<2>(key), get<3>(key));
		return hash;
	}
};
}

//template<>
//struct hash<tuple<Integer, Integer, Varchar<16>, Varchar<16>>> {
//	size_t operator()( tuple<Integer, Integer, Varchar<16>, Varchar<16>> const& key) const {
//		size_t hash = 0;
//		hash = hashKey<Integer, Integer, Varchar<16>, Varchar<16>>(get<0>(key),
//				get<1>(key), get<2>(key), get<3>(key));
//		return hash;
//	}
//};

/*-----------------------------------------------------------------------------------------------------------------------*/
template<typename Relation, typename Index, typename PKey, typename Row>
void _insert(Relation rel, Index index, PKey key, Row row){
	rel.push_back(row);
	index.insert(make_pair(key, row));
}

template<typename Index, typename PKey>
bool _find(Index index, PKey key){
	try {
		index.at(key);
		return true;
	}
	catch(out_of_range& oor){
		return false;
	};
}

template<typename Relation, typename Index, typename PKey, typename Row>
Row* _select(Relation rel, Index index, PKey key){
	try{
		return &rel[index.at(key)];
	} catch(out_of_range& e){
		cerr << e.what() << "\n";
		return nullptr;
	}
}
template<typename Attribute, typename Value>
void _update(Attribute *attr, Value val){
	*attr = val;
}

template<typename Relation, typename PKey>
void _delete(Relation rel, PKey key){
	rel.erease(key);
}
/*--------------------------------Define structure of a row for each table-----------------------------------*/

struct w_Row {
	Integer w_id;
	Varchar<10> w_name;
	Varchar<20> w_street_1, w_street_2, w_city;
	Char<2> w_state;
	Char<9> w_zip;
	Numeric<4, 4> w_tax;
	Numeric<12, 2> w_ytd;

	w_Row(){}

	w_Row(
			Integer id,
			Varchar<10> name,
			Varchar<20> street_1,
			Varchar<20> street_2,
			Varchar<20> city,
			Char<2> state,
			Char<9> zip,
			Numeric<4, 4> tax,
			Numeric<12, 2> ytd
	):
		w_id(id), w_name(name), w_street_1(street_1), w_street_2(street_2), w_city(city),
		w_state(state), w_zip(zip), w_tax(tax), w_ytd(ytd){ }
};

struct d_Row {
	Integer d_id, d_w_id; //PKey
	Varchar<10> d_name;
	Varchar<20> d_street_1, d_street_2, d_city;
	Char<2> d_state;
	Char<9> d_zip;
	Numeric<4, 4> d_tax;
	Numeric<12, 2> d_ytd;
	Integer d_next_o_id;

	d_Row(){}

	d_Row(
			Integer id,
			Integer w_id,
			Varchar<10> name,
			Varchar<20> street_1,
			Varchar<20> street_2,
			Varchar<20> city,
			Char<2> state,
			Char<9> zip,
			Numeric<4, 4> tax,
			Numeric<12, 2> ytd,
			Integer next_o_id
	):
		d_id(id), d_w_id(w_id),
		d_name(name), d_street_1(street_1), d_street_2(street_2), d_city(city),
		d_state(state), d_zip(zip), d_tax(tax), d_ytd(ytd),
		d_next_o_id(next_o_id){ }

};

struct c_Row {
	Integer c_id, c_d_id, c_w_id; //Pkey
	Varchar<16> c_first;
	Char<2> c_middle;
	Varchar<16> c_last;
	Varchar<20> c_street_1, c_street_2, c_city;
	Char<2>  c_state;
	Char<9> c_zip;
	Char<16> c_phone;
	Timestamp c_since;
	Char<2> c_credit;
	Numeric<12, 2> c_credit_lim;
	Numeric<4, 4> c_discount;
	Numeric<12, 2> c_balance,c_ytd_payment;
	Numeric<4, 0> c_payment_cnt, c_delivery_cnt;
	Varchar<500> c_data;

	c_Row(){}

	c_Row(
			Integer id,
			Integer d_id,
			Integer w_id,
			Varchar<16> first,
			Char<2> middle,
			Varchar<16> last,
			Varchar<20> street_1,
			Varchar<20> street_2,
			Varchar<20> city,
			Char<2>  state,
			Char<9> zip,
			Char<16> phone,
			Timestamp since,
			Char<2> credit,
			Numeric<12, 2> credit_lim,
			Numeric<4, 4> discount,
			Numeric<12, 2> balance,
			Numeric<12, 2> ytd_payment,
			Numeric<4, 0> payment_cnt,
			Numeric<4, 0> delivery_cnt,
			Varchar<500> data
	):
		c_id(id), c_d_id(d_id), c_w_id(w_id),
		c_first(first), c_middle(middle), c_last(last),
		c_street_1(street_1), c_street_2(street_2), c_city(city),
		c_state(state), c_zip(zip), c_phone(phone), c_since(since),
		c_credit(credit),c_credit_lim(credit_lim), c_discount(discount),
		c_balance(balance), c_ytd_payment(ytd_payment),
		c_payment_cnt(payment_cnt), c_delivery_cnt(delivery_cnt), c_data(data){ }
};

struct h_Row {
	Integer h_c_id, h_c_d_id, h_c_w_id, h_d_id, h_w_id;
	Timestamp h_date;
	Numeric<6, 2> h_amount;
	Varchar<24> h_data;

	h_Row(){}

	h_Row(	Integer c_id,
			Integer c_d_id,
			Integer c_w_id,
			Integer d_id,
			Integer w_id,
			Timestamp date,
			Numeric<6, 2> amount,
			Varchar<24> data
	):
		h_c_id(c_id), h_c_d_id(c_d_id), h_c_w_id(c_w_id),
		h_d_id(d_id), h_w_id(w_id), h_date(date),
		h_amount(amount), h_data(data){}



};

struct no_Row {
	Integer no_o_id, no_d_id, no_w_id; //Pkey

	no_Row(){}

	no_Row(Integer o_id, Integer d_id, Integer w_id):
		no_o_id(o_id), no_d_id(d_id), no_w_id(w_id){

	}
};

struct o_Row {
	Integer o_id, o_d_id, o_w_id; //Pkey
	Integer o_c_id;
	Timestamp o_entry_d;
	Integer o_carrier_id;
	Numeric<2, 0> o_ol_cnt;
	Numeric<1, 0> o_all_local;

	o_Row(){}

	o_Row(
			Integer id,
			Integer d_id,
			Integer w_id,
			Integer c_id,
			Timestamp entry_d,
			Integer carrier_id,
			Numeric<2,0> ol_cnt,
			Numeric<1, 0> all_local
		):
				o_id(id), o_d_id(d_id), o_w_id(w_id),
				o_c_id(c_id), o_entry_d(entry_d), o_carrier_id(carrier_id),
				o_ol_cnt(ol_cnt), o_all_local(all_local){

	}
};

struct ol_Row {
	Integer ol_o_id, ol_d_id, ol_w_id, ol_number; //Pkey
	Integer ol_i_id, ol_supply_w_id;
	Timestamp ol_delivery_d;
	Numeric<2, 0> ol_quantity;
	Numeric<6, 2> ol_amount;
	Char<24> ol_dist_info;

	ol_Row(){}

	ol_Row(
			Integer o_id,
			Integer d_id,
			Integer w_id,
			Integer number,
			Integer i_id,
			Integer supply_w_id,
			Timestamp delivery_d,
			Numeric<2, 0> quantity,
			Numeric<6, 2> amount,
			Char<24> dist_info
			):
				ol_o_id(o_id), ol_d_id(d_id), ol_w_id(w_id), ol_number(number),
				ol_i_id(i_id), ol_supply_w_id(supply_w_id), ol_delivery_d(delivery_d),
				ol_quantity(quantity), ol_amount(amount), ol_dist_info(dist_info){

	}
};

struct i_Row {
	//uno_Rowed_map<Integer, Integer> i_id;
	Integer i_id;	//Pkey
	Integer i_im_id;
	Varchar<24> i_name;
	Numeric<5, 2> i_price;
	Varchar<50> i_data;


	i_Row(){}

	i_Row(
			Integer id,
			Integer im_id,
			Varchar<24> name,
			Numeric<5, 2> price,
			Varchar<50> data
	):
			i_id(id),
			i_im_id(im_id), i_name(name), i_price(price),
			i_data(data){ }
};

struct s_Row {
	Integer s_i_id, s_w_id; //Pkey
	Numeric<4, 0> s_quantity;
	Char<24> s_dist_01, s_dist_02, s_dist_03, s_dist_04, s_dist_05, s_dist_06,
			s_dist_07, s_dist_08, s_dist_09, s_dist_10;
	Numeric<8, 0> s_ytd;
	Numeric<4, 0> s_order_cnt, s_remote_cnt;
	Varchar<50> s_data;

	s_Row(){}

	s_Row(
			Integer i_id,
			Integer w_id,
			Numeric<4, 0> quantity,
			Char<24> dist_01, Char<24> dist_02, Char<24> dist_03, Char<24> dist_04,
			Char<24> dist_05, Char<24> dist_06, Char<24> dist_07, Char<24> dist_08,
			Char<24> dist_09, Char<24> dist_10,
			Numeric<8, 0> ytd,
			Numeric<4, 0> order_cnt,
			Numeric<4, 0> remote_cnt,
			Varchar<50> data
	):
		s_i_id(i_id), s_w_id(w_id),
		s_quantity(quantity), s_dist_01(dist_01), s_dist_02(dist_02), s_dist_03(dist_03),
		s_dist_04(dist_04),s_dist_05(dist_05), s_dist_06(dist_06),
		s_dist_07(dist_07), s_dist_08(dist_08),s_dist_09(dist_09), s_dist_10(dist_10),
		s_ytd(ytd), s_order_cnt(order_cnt), s_remote_cnt(remote_cnt), s_data(data){ }
};
/*-----------------------------------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------------------------------------------------*/

//void w_Insert(vector<string>, unordered_map<Integer, w_Row>);




//template<typename T>
//void _Import(ifstream itbl&, elm, T x){
//	string line;
//	if (itbl.is_open()) {
//		while (getline(itbl, line)) {
//			w_Insert(split(line), w);
//		}
//	}
//}



inline std::ostream& operator<<(std::ostream& s,
                       const std::tuple<Integer, Integer>& t) {
  s << "(" << std::get<0>(t) << "," << std::get<1>(t) << "," << ")";
  return s;
}
inline std::ostream& operator<<(std::ostream& s,
                       const std::tuple<Integer, Integer, Integer>& t) {
  s << "(" << std::get<0>(t) << "," << std::get<1>(t) << "," <<
      std::get<2>(t) << ")";
  return s;
}
inline std::ostream& operator<<(std::ostream& s,
                       const std::tuple<Integer, Integer, Integer, Integer>& t) {
  s << "(" << std::get<0>(t) << "," << std::get<1>(t) << "," <<
	      std::get<2>(t) << std::get<3>(t) << ")";
  return s;
}
inline std::ostream& operator<<(std::ostream& s,
                       const std::tuple<Integer, Integer, Varchar<16>, Varchar<16>>& t) {
  s << "(" << std::get<0>(t) << "," << std::get<1>(t) << "," <<
	      std::get<2>(t) << std::get<3>(t) << ")";
  return s;
}
/*----------------------------------------Supporting functions-----------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------------------------------*/
//typedef vector<w_Row> Warehouse;
//typedef vector<d_Row> District;
//typedef vector<c_Row> Customer;
//typedef vector<h_Row> History;
//typedef vector<no_Row> NewOrder;
//typedef vector<o_Row> OrderLine;
//typedef vector<i_Row> Index;
//typedef vector<s_Row> Stock;
//
//typedef unordered_map<Integer, size_t> warehouse;
//typedef unordered_map<tuple<Integer, Integer>, size_t> district;
//typedef unordered_map<tuple<Integer, Integer, Integer>, size_t> customer;
//typedef unordered_map<tuple<Integer, Integer, Integer>, size_t> neworder;
//typedef unordered_map<tuple<Integer, Integer, Integer>, size_t> order;
//typedef unordered_map<tuple<Integer, Integer, Integer, Integer>, size_t> orderline;
//typedef unordered_map<Integer, size_t> item;
//typedef unordered_map<tuple<Integer, Integer>, size_t> stock;

extern vector<Table> tables;

struct TPCC {
	vector<w_Row> warehouse;
	vector<d_Row> district;
	vector<c_Row> customer;
	vector<h_Row> history;
	vector<no_Row> neworder;
	vector<o_Row> order;
	vector<ol_Row> orderline;
	vector<i_Row> item;
	vector<s_Row> stock;

	/*
	 * Primary key indexes of each table as unordered_map
	 * with key part is the primary key and value part is
	 * index-number on the vector of the table.
	 */
	unordered_map<Integer, size_t> warehouse_ix;
	unordered_map<tuple<Integer, Integer>, size_t> district_ix;
	unordered_map<tuple<Integer, Integer, Integer>, size_t> customer_ix;
	unordered_map<tuple<Integer, Integer, Integer>, size_t> neworder_ix;
	unordered_map<tuple<Integer, Integer, Integer>, size_t> order_ix;
	unordered_map<tuple<Integer, Integer, Integer, Integer>, size_t> orderline_ix;
	unordered_map<Integer, size_t> item_ix;
	unordered_map<tuple<Integer, Integer>, size_t> stock_ix;



	TPCC();

	~TPCC();

	void w_Insert(Integer , w_Row &);

	void w_Import(ifstream& );
	/*-----------------------------------------------------------------------------------------------------------------------*/
	void d_Insert(tup_2Int, d_Row&);

	void d_Import(ifstream&);
	/*-----------------------------------------------------------------------------------------------------------------------*/
	void c_Insert(tup_3Int, c_Row&);

	inline void c_Import(ifstream&);
	/*-----------------------------------------------------------------------------------------------------------------------*/
	void h_Insert(h_Row&);

	void h_Import(ifstream&);
	/*-----------------------------------------------------------------------------------------------------------------------*/
	void no_Insert(tup_3Int, no_Row);

	inline void no_Import(ifstream&);
	/*-----------------------------------------------------------------------------------------------------------------------*/
	void o_Insert(tup_3Int, o_Row&);

	inline void o_Import(ifstream&);
	/*-----------------------------------------------------------------------------------------------------------------------*/
	void ol_Insert(tup_4Int, ol_Row);

	inline void ol_Import(ifstream&);
	/*-----------------------------------------------------------------------------------------------------------------------*/
	void i_Insert(Integer, i_Row&);

	void i_Import(ifstream&);
	/*-----------------------------------------------------------------------------------------------------------------------*/
	void s_Insert(tup_2Int, s_Row&);

	void s_Import(ifstream&);

	/*-----------------------------------------------------------------------------------------------------------------------*/
	//std::ostream& operator<<(std::ostream& out,const w_Row& value);
	//For the two indexes

	void _importIndex();
	void _import();


};
#endif

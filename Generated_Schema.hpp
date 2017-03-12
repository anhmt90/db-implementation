#ifndef H_TPCC_SCHEMA 
#define H_TPCC_SCHEMA

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <tuple>
#include <utility>
#include <algorithm>
#include "Types.hpp"
using namespace std;
using namespace std::chrono;
namespace std{
template<> struct hash<Integer>{
size_t operator()(Integer const& key) const {
size_t hash = 0;
hash = hashKey<Integer(key);
return hash;
 }
 };

template<> struct hash<Integer, Integer>{
size_t operator()(tuple<Integer, Integer> const& key) const {
size_t hash = 0;
hash = hashKey<Integer, Integer(get<0>(key), get<1>(key), );
return hash;
 }
 };

template<> struct hash<Integer, Integer, Integer>{
size_t operator()(tuple<Integer, Integer, Integer> const& key) const {
size_t hash = 0;
hash = hashKey<Integer, Integer, Integer(get<0>(key), get<1>(key), get<2>(key), );
return hash;
 }
 };

template<> struct hash<Integer, Integer, Integer, Integer>{
size_t operator()(tuple<Integer, Integer, Integer, Integer> const& key) const {
size_t hash = 0;
hash = hashKey<Integer, Integer, Integer, Integer(get<0>(key), get<1>(key), get<2>(key), get<3>(key), );
return hash;
 }
 };

struct Warehouse : public Table{
struct Tuple{
Integer w_id;
Varchar<10> w_name;
Varchar<20> w_street_1;
Varchar<20> w_street_2;
Varchar<20> w_city;
Char<2> w_state;
Char<9> w_zip;
Numeric<4,4> w_tax;
Numeric<12,2> w_ytd;
Tuple(){ };
Tuple(
Integer w_id, 
Varchar<10> w_name, 
Varchar<20> w_street_1, 
Varchar<20> w_street_2, 
Varchar<20> w_city, 
Char<2> w_state, 
Char<9> w_zip, 
Numeric<4,4> w_tax, 
Numeric<12,2> w_ytd) : 
w_id(w_id), w_name(w_name), w_street_1(w_street_1), w_street_2(w_street_2), w_city(w_city), w_state(w_state), w_zip(w_zip), w_tax(w_tax), w_ytd(w_ytd){ }
 };
vector<Warehouse::Tuple> data;
unordered_map<Integer, uint64_t> pk_index;
Warehouse(){
this->name = "warehouse";
tables.push_back(*this);
import();
}virtual ~Warehouse(){};
void import();
Tuple* read(Integer);
Tuple* insert(Tuple*);
bool remove(Integer);
};


struct District : public Table{
struct Tuple{
Integer d_id;
Integer d_w_id;
Varchar<10> d_name;
Varchar<20> d_street_1;
Varchar<20> d_street_2;
Varchar<20> d_city;
Char<2> d_state;
Char<9> d_zip;
Numeric<4,4> d_tax;
Numeric<12,2> d_ytd;
Integer d_next_o_id;
Tuple(){ };
Tuple(
Integer d_id, 
Integer d_w_id, 
Varchar<10> d_name, 
Varchar<20> d_street_1, 
Varchar<20> d_street_2, 
Varchar<20> d_city, 
Char<2> d_state, 
Char<9> d_zip, 
Numeric<4,4> d_tax, 
Numeric<12,2> d_ytd, 
Integer d_next_o_id) : 
d_id(d_id), d_w_id(d_w_id), d_name(d_name), d_street_1(d_street_1), d_street_2(d_street_2), d_city(d_city), d_state(d_state), d_zip(d_zip), d_tax(d_tax), d_ytd(d_ytd), d_next_o_id(d_next_o_id){ }
 };
vector<District::Tuple> data;
unordered_map<tuple<Integer, Integer>, uint64_t> pk_index;
District(){
this->name = "district";
tables.push_back(*this);
import();
}virtual ~District(){};
void import();
Tuple* read(tuple<Integer, Integer>);
Tuple* insert(Tuple*);
bool remove(tuple<Integer, Integer>);
};


struct Customer : public Table{
struct Tuple{
Integer c_id;
Integer c_d_id;
Integer c_w_id;
Varchar<16> c_first;
Char<2> c_middle;
Varchar<16> c_last;
Varchar<20> c_street_1;
Varchar<20> c_street_2;
Varchar<20> c_city;
Char<2> c_state;
Char<9> c_zip;
Char<16> c_phone;
Timestamp c_since;
Char<2> c_credit;
Numeric<12,2> c_credit_lim;
Numeric<4,4> c_discount;
Numeric<12,2> c_balance;
Numeric<12,2> c_ytd_payment;
Numeric<4,0> c_payment_cnt;
Numeric<4,0> c_delivery_cnt;
Varchar<500> c_data;
Tuple(){ };
Tuple(
Integer c_id, 
Integer c_d_id, 
Integer c_w_id, 
Varchar<16> c_first, 
Char<2> c_middle, 
Varchar<16> c_last, 
Varchar<20> c_street_1, 
Varchar<20> c_street_2, 
Varchar<20> c_city, 
Char<2> c_state, 
Char<9> c_zip, 
Char<16> c_phone, 
Timestamp c_since, 
Char<2> c_credit, 
Numeric<12,2> c_credit_lim, 
Numeric<4,4> c_discount, 
Numeric<12,2> c_balance, 
Numeric<12,2> c_ytd_payment, 
Numeric<4,0> c_payment_cnt, 
Numeric<4,0> c_delivery_cnt, 
Varchar<500> c_data) : 
c_id(c_id), c_d_id(c_d_id), c_w_id(c_w_id), c_first(c_first), c_middle(c_middle), c_last(c_last), c_street_1(c_street_1), c_street_2(c_street_2), c_city(c_city), c_state(c_state), c_zip(c_zip), c_phone(c_phone), c_since(c_since), c_credit(c_credit), c_credit_lim(c_credit_lim), c_discount(c_discount), c_balance(c_balance), c_ytd_payment(c_ytd_payment), c_payment_cnt(c_payment_cnt), c_delivery_cnt(c_delivery_cnt), c_data(c_data){ }
 };
vector<Customer::Tuple> data;
unordered_map<tuple<Integer, Integer, Integer>, uint64_t> pk_index;
Customer(){
this->name = "customer";
tables.push_back(*this);
import();
}virtual ~Customer(){};
void import();
Tuple* read(tuple<Integer, Integer, Integer>);
Tuple* insert(Tuple*);
bool remove(tuple<Integer, Integer, Integer>);
};


struct History : public Table{
struct Tuple{
Integer h_c_id;
Integer h_c_d_id;
Integer h_c_w_id;
Integer h_d_id;
Integer h_w_id;
Timestamp h_date;
Numeric<6,2> h_amount;
Varchar<24> h_data;
Tuple(){ };
Tuple(
Integer h_c_id, 
Integer h_c_d_id, 
Integer h_c_w_id, 
Integer h_d_id, 
Integer h_w_id, 
Timestamp h_date, 
Numeric<6,2> h_amount, 
Varchar<24> h_data) : 
h_c_id(h_c_id), h_c_d_id(h_c_d_id), h_c_w_id(h_c_w_id), h_d_id(h_d_id), h_w_id(h_w_id), h_date(h_date), h_amount(h_amount), h_data(h_data){ }
 };
vector<History::Tuple> data;
unordered_map<tuple<>, uint64_t> pk_index;
History(){
this->name = "history";
tables.push_back(*this);
import();
}virtual ~History(){};
void import();
Tuple* read(tuple<>);
Tuple* insert(Tuple*);
bool remove(tuple<>);
};


struct Neworder : public Table{
struct Tuple{
Integer no_o_id;
Integer no_d_id;
Integer no_w_id;
Tuple(){ };
Tuple(
Integer no_o_id, 
Integer no_d_id, 
Integer no_w_id) : 
no_o_id(no_o_id), no_d_id(no_d_id), no_w_id(no_w_id){ }
 };
vector<Neworder::Tuple> data;
unordered_map<tuple<Integer, Integer, Integer>, uint64_t> pk_index;
Neworder(){
this->name = "neworder";
tables.push_back(*this);
import();
}virtual ~Neworder(){};
void import();
Tuple* read(tuple<Integer, Integer, Integer>);
Tuple* insert(Tuple*);
bool remove(tuple<Integer, Integer, Integer>);
};


struct Order : public Table{
struct Tuple{
Integer o_id;
Integer o_d_id;
Integer o_w_id;
Integer o_c_id;
Timestamp o_entry_d;
Integer o_carrier_id;
Numeric<2,0> o_ol_cnt;
Numeric<1,0> o_all_local;
Tuple(){ };
Tuple(
Integer o_id, 
Integer o_d_id, 
Integer o_w_id, 
Integer o_c_id, 
Timestamp o_entry_d, 
Integer o_carrier_id, 
Numeric<2,0> o_ol_cnt, 
Numeric<1,0> o_all_local) : 
o_id(o_id), o_d_id(o_d_id), o_w_id(o_w_id), o_c_id(o_c_id), o_entry_d(o_entry_d), o_carrier_id(o_carrier_id), o_ol_cnt(o_ol_cnt), o_all_local(o_all_local){ }
 };
vector<Order::Tuple> data;
unordered_map<tuple<Integer, Integer, Integer>, uint64_t> pk_index;
Order(){
this->name = "order";
tables.push_back(*this);
import();
}virtual ~Order(){};
void import();
Tuple* read(tuple<Integer, Integer, Integer>);
Tuple* insert(Tuple*);
bool remove(tuple<Integer, Integer, Integer>);
};


struct Orderline : public Table{
struct Tuple{
Integer ol_o_id;
Integer ol_d_id;
Integer ol_w_id;
Integer ol_number;
Integer ol_i_id;
Integer ol_supply_w_id;
Timestamp ol_delivery_d;
Numeric<2,0> ol_quantity;
Numeric<6,2> ol_amount;
Char<24> ol_dist_info;
Tuple(){ };
Tuple(
Integer ol_o_id, 
Integer ol_d_id, 
Integer ol_w_id, 
Integer ol_number, 
Integer ol_i_id, 
Integer ol_supply_w_id, 
Timestamp ol_delivery_d, 
Numeric<2,0> ol_quantity, 
Numeric<6,2> ol_amount, 
Char<24> ol_dist_info) : 
ol_o_id(ol_o_id), ol_d_id(ol_d_id), ol_w_id(ol_w_id), ol_number(ol_number), ol_i_id(ol_i_id), ol_supply_w_id(ol_supply_w_id), ol_delivery_d(ol_delivery_d), ol_quantity(ol_quantity), ol_amount(ol_amount), ol_dist_info(ol_dist_info){ }
 };
vector<Orderline::Tuple> data;
unordered_map<tuple<Integer, Integer, Integer, Integer>, uint64_t> pk_index;
Orderline(){
this->name = "orderline";
tables.push_back(*this);
import();
}virtual ~Orderline(){};
void import();
Tuple* read(tuple<Integer, Integer, Integer, Integer>);
Tuple* insert(Tuple*);
bool remove(tuple<Integer, Integer, Integer, Integer>);
};


struct Item : public Table{
struct Tuple{
Integer i_id;
Integer i_im_id;
Varchar<24> i_name;
Numeric<5,2> i_price;
Varchar<50> i_data;
Tuple(){ };
Tuple(
Integer i_id, 
Integer i_im_id, 
Varchar<24> i_name, 
Numeric<5,2> i_price, 
Varchar<50> i_data) : 
i_id(i_id), i_im_id(i_im_id), i_name(i_name), i_price(i_price), i_data(i_data){ }
 };
vector<Item::Tuple> data;
unordered_map<Integer, uint64_t> pk_index;
Item(){
this->name = "item";
tables.push_back(*this);
import();
}virtual ~Item(){};
void import();
Tuple* read(Integer);
Tuple* insert(Tuple*);
bool remove(Integer);
};


struct Stock : public Table{
struct Tuple{
Integer s_i_id;
Integer s_w_id;
Numeric<4,0> s_quantity;
Char<24> s_dist_01;
Char<24> s_dist_02;
Char<24> s_dist_03;
Char<24> s_dist_04;
Char<24> s_dist_05;
Char<24> s_dist_06;
Char<24> s_dist_07;
Char<24> s_dist_08;
Char<24> s_dist_09;
Char<24> s_dist_10;
Numeric<8,0> s_ytd;
Numeric<4,0> s_order_cnt;
Numeric<4,0> s_remote_cnt;
Varchar<50> s_data;
Tuple(){ };
Tuple(
Integer s_i_id, 
Integer s_w_id, 
Numeric<4,0> s_quantity, 
Char<24> s_dist_01, 
Char<24> s_dist_02, 
Char<24> s_dist_03, 
Char<24> s_dist_04, 
Char<24> s_dist_05, 
Char<24> s_dist_06, 
Char<24> s_dist_07, 
Char<24> s_dist_08, 
Char<24> s_dist_09, 
Char<24> s_dist_10, 
Numeric<8,0> s_ytd, 
Numeric<4,0> s_order_cnt, 
Numeric<4,0> s_remote_cnt, 
Varchar<50> s_data) : 
s_i_id(s_i_id), s_w_id(s_w_id), s_quantity(s_quantity), s_dist_01(s_dist_01), s_dist_02(s_dist_02), s_dist_03(s_dist_03), s_dist_04(s_dist_04), s_dist_05(s_dist_05), s_dist_06(s_dist_06), s_dist_07(s_dist_07), s_dist_08(s_dist_08), s_dist_09(s_dist_09), s_dist_10(s_dist_10), s_ytd(s_ytd), s_order_cnt(s_order_cnt), s_remote_cnt(s_remote_cnt), s_data(s_data){ }
 };
vector<Stock::Tuple> data;
unordered_map<tuple<Integer, Integer>, uint64_t> pk_index;
Stock(){
this->name = "stock";
tables.push_back(*this);
import();
}virtual ~Stock(){};
void import();
Tuple* read(tuple<Integer, Integer>);
Tuple* insert(Tuple*);
bool remove(tuple<Integer, Integer>);
};


extern Warehouse warehouse;
extern District district;
extern Customer customer;
extern History history;
extern Neworder neworder;
extern Order order;
extern Orderline orderline;
extern Item item;
extern Stock stock;
#endif
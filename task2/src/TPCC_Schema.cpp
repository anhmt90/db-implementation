#include "TPCC_Schema.hpp"

using namespace std;

Warehouse warehouse;
District district;
Customer customer;
Neworder neworder;
Order order;
Orderline orderline;
Item item;
Stock stock;

void close_ifstream(ifstream& itbl) {
itbl.close();
itbl.clear();
}


vector<string> split(const string &s) {
vector<string> splitted;
stringstream ss(s);
string item;
while (std::getline(ss, item, '|')) {
splitted.push_back(item);
}
return splitted;
}

void Warehouse::import(){
ifstream itbl("tbl/tpcc_warehouse.tbl");
if (!itbl) {
cerr << "tpcc_warehouse.tbl could not be opened! :(" << endl;
exit(1);
}
else{
string line;
if (itbl.is_open()) {
while (getline(itbl, line)) {
vector<string> elm = split(line);
Warehouse::Tuple row;
row.w_id = row.w_id.castString(elm[0].c_str(), elm[0].length());
row.w_name = row.w_name.castString(elm[1].c_str(), elm[1].length());
row.w_street_1 = row.w_street_1.castString(elm[2].c_str(), elm[2].length());
row.w_street_2 = row.w_street_2.castString(elm[3].c_str(), elm[3].length());
row.w_city = row.w_city.castString(elm[4].c_str(), elm[4].length());
row.w_state = row.w_state.castString(elm[5].c_str(), elm[5].length());
row.w_zip = row.w_zip.castString(elm[6].c_str(), elm[6].length());
row.w_tax = row.w_tax.castString(elm[7].c_str(), elm[7].length());
row.w_ytd = row.w_ytd.castString(elm[8].c_str(), elm[8].length());
data.push_back(row);
pk_index.insert(make_pair(row.w_id, data.size() - 1));
}
tables.back().attributes.push_back(Attribute("w_id", "Integer", "warehouse"));
tables.back().attributes.push_back(Attribute("w_name", "Varchar<10>", "warehouse"));
tables.back().attributes.push_back(Attribute("w_street_1", "Varchar<20>", "warehouse"));
tables.back().attributes.push_back(Attribute("w_street_2", "Varchar<20>", "warehouse"));
tables.back().attributes.push_back(Attribute("w_city", "Varchar<20>", "warehouse"));
tables.back().attributes.push_back(Attribute("w_state", "Char<2>", "warehouse"));
tables.back().attributes.push_back(Attribute("w_zip", "Char<9>", "warehouse"));
tables.back().attributes.push_back(Attribute("w_tax", "Numeric<4,4>", "warehouse"));
tables.back().attributes.push_back(Attribute("w_ytd", "Numeric<12,2>", "warehouse"));
}
close_ifstream(itbl);
cout << "Warehouse imported!\n";
}
}


void District::import(){
ifstream itbl("tbl/tpcc_district.tbl");
if (!itbl) {
cerr << "tpcc_district.tbl could not be opened! :(" << endl;
exit(1);
}
else{
string line;
if (itbl.is_open()) {
while (getline(itbl, line)) {
vector<string> elm = split(line);
District::Tuple row;
row.d_id = row.d_id.castString(elm[0].c_str(), elm[0].length());
row.d_w_id = row.d_w_id.castString(elm[1].c_str(), elm[1].length());
row.d_name = row.d_name.castString(elm[2].c_str(), elm[2].length());
auto tup = make_tuple(row.d_id, row.d_w_id);
row.d_street_1 = row.d_street_1.castString(elm[3].c_str(), elm[3].length());
row.d_street_2 = row.d_street_2.castString(elm[4].c_str(), elm[4].length());
row.d_city = row.d_city.castString(elm[5].c_str(), elm[5].length());
row.d_state = row.d_state.castString(elm[6].c_str(), elm[6].length());
row.d_zip = row.d_zip.castString(elm[7].c_str(), elm[7].length());
row.d_tax = row.d_tax.castString(elm[8].c_str(), elm[8].length());
row.d_ytd = row.d_ytd.castString(elm[9].c_str(), elm[9].length());
row.d_next_o_id = row.d_next_o_id.castString(elm[10].c_str(), elm[10].length());
data.push_back(row);
pk_index.insert(make_pair(tup, data.size() - 1));
}
tables.back().attributes.push_back(Attribute("d_id", "Integer", "district"));
tables.back().attributes.push_back(Attribute("d_w_id", "Integer", "district"));
tables.back().attributes.push_back(Attribute("d_name", "Varchar<10>", "district"));
tables.back().attributes.push_back(Attribute("d_street_1", "Varchar<20>", "district"));
tables.back().attributes.push_back(Attribute("d_street_2", "Varchar<20>", "district"));
tables.back().attributes.push_back(Attribute("d_city", "Varchar<20>", "district"));
tables.back().attributes.push_back(Attribute("d_state", "Char<2>", "district"));
tables.back().attributes.push_back(Attribute("d_zip", "Char<9>", "district"));
tables.back().attributes.push_back(Attribute("d_tax", "Numeric<4,4>", "district"));
tables.back().attributes.push_back(Attribute("d_ytd", "Numeric<12,2>", "district"));
tables.back().attributes.push_back(Attribute("d_next_o_id", "Integer", "district"));
}
close_ifstream(itbl);
cout << "District imported!\n";
}
}


void Customer::import(){
ifstream itbl("tbl/tpcc_customer.tbl");
if (!itbl) {
cerr << "tpcc_customer.tbl could not be opened! :(" << endl;
exit(1);
}
else{
string line;
if (itbl.is_open()) {
while (getline(itbl, line)) {
vector<string> elm = split(line);
Customer::Tuple row;
row.c_id = row.c_id.castString(elm[0].c_str(), elm[0].length());
row.c_d_id = row.c_d_id.castString(elm[1].c_str(), elm[1].length());
row.c_w_id = row.c_w_id.castString(elm[2].c_str(), elm[2].length());
row.c_first = row.c_first.castString(elm[3].c_str(), elm[3].length());
auto tup = make_tuple(row.c_id, row.c_d_id, row.c_w_id);
row.c_middle = row.c_middle.castString(elm[4].c_str(), elm[4].length());
row.c_last = row.c_last.castString(elm[5].c_str(), elm[5].length());
row.c_street_1 = row.c_street_1.castString(elm[6].c_str(), elm[6].length());
row.c_street_2 = row.c_street_2.castString(elm[7].c_str(), elm[7].length());
row.c_city = row.c_city.castString(elm[8].c_str(), elm[8].length());
row.c_state = row.c_state.castString(elm[9].c_str(), elm[9].length());
row.c_zip = row.c_zip.castString(elm[10].c_str(), elm[10].length());
row.c_phone = row.c_phone.castString(elm[11].c_str(), elm[11].length());
row.c_since = row.c_since.castString(elm[12].c_str(), elm[12].length());
row.c_credit = row.c_credit.castString(elm[13].c_str(), elm[13].length());
row.c_credit_lim = row.c_credit_lim.castString(elm[14].c_str(), elm[14].length());
row.c_discount = row.c_discount.castString(elm[15].c_str(), elm[15].length());
row.c_balance = row.c_balance.castString(elm[16].c_str(), elm[16].length());
row.c_ytd_payment = row.c_ytd_payment.castString(elm[17].c_str(), elm[17].length());
row.c_payment_cnt = row.c_payment_cnt.castString(elm[18].c_str(), elm[18].length());
row.c_delivery_cnt = row.c_delivery_cnt.castString(elm[19].c_str(), elm[19].length());
row.c_data = row.c_data.castString(elm[20].c_str(), elm[20].length());
data.push_back(row);
pk_index.insert(make_pair(tup, data.size() - 1));
}
tables.back().attributes.push_back(Attribute("c_id", "Integer", "customer"));
tables.back().attributes.push_back(Attribute("c_d_id", "Integer", "customer"));
tables.back().attributes.push_back(Attribute("c_w_id", "Integer", "customer"));
tables.back().attributes.push_back(Attribute("c_first", "Varchar<16>", "customer"));
tables.back().attributes.push_back(Attribute("c_middle", "Char<2>", "customer"));
tables.back().attributes.push_back(Attribute("c_last", "Varchar<16>", "customer"));
tables.back().attributes.push_back(Attribute("c_street_1", "Varchar<20>", "customer"));
tables.back().attributes.push_back(Attribute("c_street_2", "Varchar<20>", "customer"));
tables.back().attributes.push_back(Attribute("c_city", "Varchar<20>", "customer"));
tables.back().attributes.push_back(Attribute("c_state", "Char<2>", "customer"));
tables.back().attributes.push_back(Attribute("c_zip", "Char<9>", "customer"));
tables.back().attributes.push_back(Attribute("c_phone", "Char<16>", "customer"));
tables.back().attributes.push_back(Attribute("c_since", "Timestamp", "customer"));
tables.back().attributes.push_back(Attribute("c_credit", "Char<2>", "customer"));
tables.back().attributes.push_back(Attribute("c_credit_lim", "Numeric<12,2>", "customer"));
tables.back().attributes.push_back(Attribute("c_discount", "Numeric<4,4>", "customer"));
tables.back().attributes.push_back(Attribute("c_balance", "Numeric<12,2>", "customer"));
tables.back().attributes.push_back(Attribute("c_ytd_payment", "Numeric<12,2>", "customer"));
tables.back().attributes.push_back(Attribute("c_payment_cnt", "Numeric<4,0>", "customer"));
tables.back().attributes.push_back(Attribute("c_delivery_cnt", "Numeric<4,0>", "customer"));
tables.back().attributes.push_back(Attribute("c_data", "Varchar<500>", "customer"));
}
close_ifstream(itbl);
cout << "Customer imported!\n";
}
}


void Neworder::import(){
ifstream itbl("tbl/tpcc_neworder.tbl");
if (!itbl) {
cerr << "tpcc_neworder.tbl could not be opened! :(" << endl;
exit(1);
}
else{
string line;
if (itbl.is_open()) {
while (getline(itbl, line)) {
vector<string> elm = split(line);
Neworder::Tuple row;
row.no_o_id = row.no_o_id.castString(elm[0].c_str(), elm[0].length());
row.no_d_id = row.no_d_id.castString(elm[1].c_str(), elm[1].length());
row.no_w_id = row.no_w_id.castString(elm[2].c_str(), elm[2].length());
auto tup = make_tuple(row.no_o_id, row.no_d_id, row.no_w_id);
data.push_back(row);
pk_index.insert(make_pair(tup, data.size() - 1));
}
tables.back().attributes.push_back(Attribute("no_o_id", "Integer", "neworder"));
tables.back().attributes.push_back(Attribute("no_d_id", "Integer", "neworder"));
tables.back().attributes.push_back(Attribute("no_w_id", "Integer", "neworder"));
}
close_ifstream(itbl);
cout << "Neworder imported!\n";
}
}


void Order::import(){
ifstream itbl("tbl/tpcc_order.tbl");
if (!itbl) {
cerr << "tpcc_order.tbl could not be opened! :(" << endl;
exit(1);
}
else{
string line;
if (itbl.is_open()) {
while (getline(itbl, line)) {
vector<string> elm = split(line);
Order::Tuple row;
row.o_id = row.o_id.castString(elm[0].c_str(), elm[0].length());
row.o_d_id = row.o_d_id.castString(elm[1].c_str(), elm[1].length());
row.o_w_id = row.o_w_id.castString(elm[2].c_str(), elm[2].length());
row.o_c_id = row.o_c_id.castString(elm[3].c_str(), elm[3].length());
auto tup = make_tuple(row.o_id, row.o_d_id, row.o_w_id);
row.o_entry_d = row.o_entry_d.castString(elm[4].c_str(), elm[4].length());
row.o_carrier_id = row.o_carrier_id.castString(elm[5].c_str(), elm[5].length());
row.o_ol_cnt = row.o_ol_cnt.castString(elm[6].c_str(), elm[6].length());
row.o_all_local = row.o_all_local.castString(elm[7].c_str(), elm[7].length());
data.push_back(row);
pk_index.insert(make_pair(tup, data.size() - 1));
}
tables.back().attributes.push_back(Attribute("o_id", "Integer", "order"));
tables.back().attributes.push_back(Attribute("o_d_id", "Integer", "order"));
tables.back().attributes.push_back(Attribute("o_w_id", "Integer", "order"));
tables.back().attributes.push_back(Attribute("o_c_id", "Integer", "order"));
tables.back().attributes.push_back(Attribute("o_entry_d", "Timestamp", "order"));
tables.back().attributes.push_back(Attribute("o_carrier_id", "Integer", "order"));
tables.back().attributes.push_back(Attribute("o_ol_cnt", "Numeric<2,0>", "order"));
tables.back().attributes.push_back(Attribute("o_all_local", "Numeric<1,0>", "order"));
}
close_ifstream(itbl);
cout << "Order imported!\n";
}
}


void Orderline::import(){
ifstream itbl("tbl/tpcc_orderline.tbl");
if (!itbl) {
cerr << "tpcc_orderline.tbl could not be opened! :(" << endl;
exit(1);
}
else{
string line;
if (itbl.is_open()) {
while (getline(itbl, line)) {
vector<string> elm = split(line);
Orderline::Tuple row;
row.ol_o_id = row.ol_o_id.castString(elm[0].c_str(), elm[0].length());
row.ol_d_id = row.ol_d_id.castString(elm[1].c_str(), elm[1].length());
row.ol_w_id = row.ol_w_id.castString(elm[2].c_str(), elm[2].length());
row.ol_number = row.ol_number.castString(elm[3].c_str(), elm[3].length());
row.ol_i_id = row.ol_i_id.castString(elm[4].c_str(), elm[4].length());
auto tup = make_tuple(row.ol_number, row.ol_o_id, row.ol_d_id, row.ol_w_id);
row.ol_supply_w_id = row.ol_supply_w_id.castString(elm[5].c_str(), elm[5].length());
row.ol_delivery_d = row.ol_delivery_d.castString(elm[6].c_str(), elm[6].length());
row.ol_quantity = row.ol_quantity.castString(elm[7].c_str(), elm[7].length());
row.ol_amount = row.ol_amount.castString(elm[8].c_str(), elm[8].length());
row.ol_dist_info = row.ol_dist_info.castString(elm[9].c_str(), elm[9].length());
data.push_back(row);
pk_index.insert(make_pair(tup, data.size() - 1));
}
tables.back().attributes.push_back(Attribute("ol_o_id", "Integer", "orderline"));
tables.back().attributes.push_back(Attribute("ol_d_id", "Integer", "orderline"));
tables.back().attributes.push_back(Attribute("ol_w_id", "Integer", "orderline"));
tables.back().attributes.push_back(Attribute("ol_number", "Integer", "orderline"));
tables.back().attributes.push_back(Attribute("ol_i_id", "Integer", "orderline"));
tables.back().attributes.push_back(Attribute("ol_supply_w_id", "Integer", "orderline"));
tables.back().attributes.push_back(Attribute("ol_delivery_d", "Timestamp", "orderline"));
tables.back().attributes.push_back(Attribute("ol_quantity", "Numeric<2,0>", "orderline"));
tables.back().attributes.push_back(Attribute("ol_amount", "Numeric<6,2>", "orderline"));
tables.back().attributes.push_back(Attribute("ol_dist_info", "Char<24>", "orderline"));
}
close_ifstream(itbl);
cout << "Orderline imported!\n";
}
}


void Item::import(){
ifstream itbl("tbl/tpcc_item.tbl");
if (!itbl) {
cerr << "tpcc_item.tbl could not be opened! :(" << endl;
exit(1);
}
else{
string line;
if (itbl.is_open()) {
while (getline(itbl, line)) {
vector<string> elm = split(line);
Item::Tuple row;
row.i_id = row.i_id.castString(elm[0].c_str(), elm[0].length());
row.i_im_id = row.i_im_id.castString(elm[1].c_str(), elm[1].length());
row.i_name = row.i_name.castString(elm[2].c_str(), elm[2].length());
row.i_price = row.i_price.castString(elm[3].c_str(), elm[3].length());
row.i_data = row.i_data.castString(elm[4].c_str(), elm[4].length());
data.push_back(row);
pk_index.insert(make_pair(row.i_id, data.size() - 1));
}
tables.back().attributes.push_back(Attribute("i_id", "Integer", "item"));
tables.back().attributes.push_back(Attribute("i_im_id", "Integer", "item"));
tables.back().attributes.push_back(Attribute("i_name", "Varchar<24>", "item"));
tables.back().attributes.push_back(Attribute("i_price", "Numeric<5,2>", "item"));
tables.back().attributes.push_back(Attribute("i_data", "Varchar<50>", "item"));
}
close_ifstream(itbl);
cout << "Item imported!\n";
}
}


void Stock::import(){
ifstream itbl("tbl/tpcc_stock.tbl");
if (!itbl) {
cerr << "tpcc_stock.tbl could not be opened! :(" << endl;
exit(1);
}
else{
string line;
if (itbl.is_open()) {
while (getline(itbl, line)) {
vector<string> elm = split(line);
Stock::Tuple row;
row.s_i_id = row.s_i_id.castString(elm[0].c_str(), elm[0].length());
row.s_w_id = row.s_w_id.castString(elm[1].c_str(), elm[1].length());
row.s_quantity = row.s_quantity.castString(elm[2].c_str(), elm[2].length());
auto tup = make_tuple(row.s_i_id, row.s_w_id);
row.s_dist_01 = row.s_dist_01.castString(elm[3].c_str(), elm[3].length());
row.s_dist_02 = row.s_dist_02.castString(elm[4].c_str(), elm[4].length());
row.s_dist_03 = row.s_dist_03.castString(elm[5].c_str(), elm[5].length());
row.s_dist_04 = row.s_dist_04.castString(elm[6].c_str(), elm[6].length());
row.s_dist_05 = row.s_dist_05.castString(elm[7].c_str(), elm[7].length());
row.s_dist_06 = row.s_dist_06.castString(elm[8].c_str(), elm[8].length());
row.s_dist_07 = row.s_dist_07.castString(elm[9].c_str(), elm[9].length());
row.s_dist_08 = row.s_dist_08.castString(elm[10].c_str(), elm[10].length());
row.s_dist_09 = row.s_dist_09.castString(elm[11].c_str(), elm[11].length());
row.s_dist_10 = row.s_dist_10.castString(elm[12].c_str(), elm[12].length());
row.s_ytd = row.s_ytd.castString(elm[13].c_str(), elm[13].length());
row.s_order_cnt = row.s_order_cnt.castString(elm[14].c_str(), elm[14].length());
row.s_remote_cnt = row.s_remote_cnt.castString(elm[15].c_str(), elm[15].length());
row.s_data = row.s_data.castString(elm[16].c_str(), elm[16].length());
data.push_back(row);
pk_index.insert(make_pair(tup, data.size() - 1));
}
tables.back().attributes.push_back(Attribute("s_i_id", "Integer", "stock"));
tables.back().attributes.push_back(Attribute("s_w_id", "Integer", "stock"));
tables.back().attributes.push_back(Attribute("s_quantity", "Numeric<4,0>", "stock"));
tables.back().attributes.push_back(Attribute("s_dist_01", "Char<24>", "stock"));
tables.back().attributes.push_back(Attribute("s_dist_02", "Char<24>", "stock"));
tables.back().attributes.push_back(Attribute("s_dist_03", "Char<24>", "stock"));
tables.back().attributes.push_back(Attribute("s_dist_04", "Char<24>", "stock"));
tables.back().attributes.push_back(Attribute("s_dist_05", "Char<24>", "stock"));
tables.back().attributes.push_back(Attribute("s_dist_06", "Char<24>", "stock"));
tables.back().attributes.push_back(Attribute("s_dist_07", "Char<24>", "stock"));
tables.back().attributes.push_back(Attribute("s_dist_08", "Char<24>", "stock"));
tables.back().attributes.push_back(Attribute("s_dist_09", "Char<24>", "stock"));
tables.back().attributes.push_back(Attribute("s_dist_10", "Char<24>", "stock"));
tables.back().attributes.push_back(Attribute("s_ytd", "Numeric<8,0>", "stock"));
tables.back().attributes.push_back(Attribute("s_order_cnt", "Numeric<4,0>", "stock"));
tables.back().attributes.push_back(Attribute("s_remote_cnt", "Numeric<4,0>", "stock"));
tables.back().attributes.push_back(Attribute("s_data", "Varchar<50>", "stock"));
}
close_ifstream(itbl);
cout << "Stock imported!\n";
}
}


Warehouse::Tuple* Warehouse::read(Integer pkey){
auto itr = pk_index.find(pkey);
if(itr == pk_index.end()) return nullptr;
return &data[itr->second];
}

District::Tuple* District::read(tuple<Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr == pk_index.end()) return nullptr;
return &data[itr->second];
}

Customer::Tuple* Customer::read(tuple<Integer, Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr == pk_index.end()) return nullptr;
return &data[itr->second];
}

Neworder::Tuple* Neworder::read(tuple<Integer, Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr == pk_index.end()) return nullptr;
return &data[itr->second];
}

Order::Tuple* Order::read(tuple<Integer, Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr == pk_index.end()) return nullptr;
return &data[itr->second];
}

Orderline::Tuple* Orderline::read(tuple<Integer, Integer, Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr == pk_index.end()) return nullptr;
return &data[itr->second];
}

Item::Tuple* Item::read(Integer pkey){
auto itr = pk_index.find(pkey);
if(itr == pk_index.end()) return nullptr;
return &data[itr->second];
}

Stock::Tuple* Stock::read(tuple<Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr == pk_index.end()) return nullptr;
return &data[itr->second];
}

Warehouse::Tuple* Warehouse::insert(Warehouse::Tuple* t){
auto pkey = t->w_id;
if(pk_index.find(pkey) == pk_index.end()){
data.push_back(*t);
pk_index.insert(make_pair(pkey, data.size()-1));
return &data.back();
}
return nullptr;
}

District::Tuple* District::insert(District::Tuple* t){
auto pkey = make_tuple(t->d_id, t->d_w_id);
if(pk_index.find(pkey) == pk_index.end()){
data.push_back(*t);
pk_index.insert(make_pair(pkey, data.size()-1));
return &data.back();
}
return nullptr;
}

Customer::Tuple* Customer::insert(Customer::Tuple* t){
auto pkey = make_tuple(t->c_id, t->c_d_id, t->c_w_id);
if(pk_index.find(pkey) == pk_index.end()){
data.push_back(*t);
pk_index.insert(make_pair(pkey, data.size()-1));
return &data.back();
}
return nullptr;
}

Neworder::Tuple* Neworder::insert(Neworder::Tuple* t){
auto pkey = make_tuple(t->no_o_id, t->no_d_id, t->no_w_id);
if(pk_index.find(pkey) == pk_index.end()){
data.push_back(*t);
pk_index.insert(make_pair(pkey, data.size()-1));
return &data.back();
}
return nullptr;
}

Order::Tuple* Order::insert(Order::Tuple* t){
auto pkey = make_tuple(t->o_id, t->o_d_id, t->o_w_id);
if(pk_index.find(pkey) == pk_index.end()){
data.push_back(*t);
pk_index.insert(make_pair(pkey, data.size()-1));
return &data.back();
}
return nullptr;
}

Orderline::Tuple* Orderline::insert(Orderline::Tuple* t){
auto pkey = make_tuple(t->ol_o_id, t->ol_d_id, t->ol_w_id, t->ol_number);
if(pk_index.find(pkey) == pk_index.end()){
data.push_back(*t);
pk_index.insert(make_pair(pkey, data.size()-1));
return &data.back();
}
return nullptr;
}

Item::Tuple* Item::insert(Item::Tuple* t){
auto pkey = t->i_id;
if(pk_index.find(pkey) == pk_index.end()){
data.push_back(*t);
pk_index.insert(make_pair(pkey, data.size()-1));
return &data.back();
}
return nullptr;
}

Stock::Tuple* Stock::insert(Stock::Tuple* t){
auto pkey = make_tuple(t->s_i_id, t->s_w_id);
if(pk_index.find(pkey) == pk_index.end()){
data.push_back(*t);
pk_index.insert(make_pair(pkey, data.size()-1));
return &data.back();
}
return nullptr;
}

bool Warehouse::remove(Integer pkey){
auto itr = pk_index.find(pkey);
if(itr != pk_index.end()){
auto index = itr->second;
if(index == data.size()-1){
data.pop_back();
pk_index.erase(itr);
return true;
}
std::swap(data.back(), data[index]);
data.pop_back();
pk_index.erase(itr);
itr = pk_index.find(data[index].w_id);
itr->second = index;
return true;
}
return false;
}


bool District::remove(tuple<Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr != pk_index.end()){
auto index = itr->second;
if(index == data.size()-1){
data.pop_back();
pk_index.erase(itr);
return true;
}
std::swap(data.back(), data[index]);
data.pop_back();
pk_index.erase(itr);
itr = pk_index.find(make_tuple(data[index].d_id, data[index].d_w_id));
itr->second = index;
return true;
}
return false;
}


bool Customer::remove(tuple<Integer, Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr != pk_index.end()){
auto index = itr->second;
if(index == data.size()-1){
data.pop_back();
pk_index.erase(itr);
return true;
}
std::swap(data.back(), data[index]);
data.pop_back();
pk_index.erase(itr);
itr = pk_index.find(make_tuple(data[index].c_id, data[index].c_d_id, data[index].c_w_id));
itr->second = index;
return true;
}
return false;
}


bool Neworder::remove(tuple<Integer, Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr != pk_index.end()){
auto index = itr->second;
if(index == data.size()-1){
data.pop_back();
pk_index.erase(itr);
return true;
}
std::swap(data.back(), data[index]);
data.pop_back();
pk_index.erase(itr);
itr = pk_index.find(make_tuple(data[index].no_o_id, data[index].no_d_id, data[index].no_w_id));
itr->second = index;
return true;
}
return false;
}


bool Order::remove(tuple<Integer, Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr != pk_index.end()){
auto index = itr->second;
if(index == data.size()-1){
data.pop_back();
pk_index.erase(itr);
return true;
}
std::swap(data.back(), data[index]);
data.pop_back();
pk_index.erase(itr);
itr = pk_index.find(make_tuple(data[index].o_id, data[index].o_d_id, data[index].o_w_id));
itr->second = index;
return true;
}
return false;
}


bool Orderline::remove(tuple<Integer, Integer, Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr != pk_index.end()){
auto index = itr->second;
if(index == data.size()-1){
data.pop_back();
pk_index.erase(itr);
return true;
}
std::swap(data.back(), data[index]);
data.pop_back();
pk_index.erase(itr);
itr = pk_index.find(make_tuple(data[index].ol_o_id, data[index].ol_d_id, data[index].ol_w_id, data[index].ol_number));
itr->second = index;
return true;
}
return false;
}


bool Item::remove(Integer pkey){
auto itr = pk_index.find(pkey);
if(itr != pk_index.end()){
auto index = itr->second;
if(index == data.size()-1){
data.pop_back();
pk_index.erase(itr);
return true;
}
std::swap(data.back(), data[index]);
data.pop_back();
pk_index.erase(itr);
itr = pk_index.find(data[index].i_id);
itr->second = index;
return true;
}
return false;
}


bool Stock::remove(tuple<Integer, Integer> pkey){
auto itr = pk_index.find(pkey);
if(itr != pk_index.end()){
auto index = itr->second;
if(index == data.size()-1){
data.pop_back();
pk_index.erase(itr);
return true;
}
std::swap(data.back(), data[index]);
data.pop_back();
pk_index.erase(itr);
itr = pk_index.find(make_tuple(data[index].s_i_id, data[index].s_w_id));
itr->second = index;
return true;
}
return false;
}



#include <iostream>
#include <memory>
#include <algorithm>
#include <climits>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <atomic>


#include "TPCC_Schema.hpp"
#include "Types.hpp"

using namespace std;

Warehouse::Tuple* _warehouse_tuple;
District::Tuple* _district_tuple;
Customer::Tuple* _customer_tuple;
NewOrder::Tuple* _neworder_tuple;
Order::Tuple* _order_tuple;
OrderLine::Tuple* _orderline_tuple;
Item::Tuple* _item_tuple;
Stock::Tuple* _stock_tuple;

void newOrder(int32_t w_id, int32_t d_id, int32_t c_id, int32_t items, int32_t supware[15], int32_t itemid[15], int32_t qty[15], Timestamp datetime){

	if(!(_warehouse_tuple = warehouse.read(Predicate(w_id).pk_int))) return;
	auto w_tax = _warehouse_tuple->w_tax;

	if(!(_customer_tuple = customer.read(Predicate(c_id, d_id, w_id).pk_3int))) return;
	auto c_discount = _customer_tuple->c_discount;

	if(!(_district_tuple = district.read(Predicate(d_id, w_id).pk_2int))) return;
	auto o_id = _district_tuple->d_next_o_id;
	auto d_tax = _district_tuple->d_tax;

	uint32_t all_local = 1;
	for(int index = 0; index < items-1; ++index){
		if(w_id != supware[index])
			all_local = 0;
	}
	// insert
	_order_tuple = order.insert(new Order::Tuple(o_id, d_id, w_id, c_id, datetime, 0, items, all_local));
	//	if(!(_order_tuple = order.insert(new Order::Tuple(o_id, d_id, w_id, c_id, datetime, 0, items, all_local)))) return; // uncomment this to abort the transaction in edge case
	_neworder_tuple = neworder.insert(new NewOrder::Tuple(o_id, d_id, w_id));
	//	if(!(_neworder_tuple = neworder.insert(new NewOrder::Tuple(o_id, d_id, w_id)))) return; // uncomment this to abort the transaction in edge case

	for(int index = 0; index <= items - 1; ++index){
		if(!(_item_tuple = item.read(Predicate(itemid[index]).pk_int))) return;
		auto i_price = _item_tuple->i_price;

		if(!(_stock_tuple = stock.read(Predicate(itemid[index], supware[index]).pk_2int))) return;
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
			if((_stock_tuple = stock.read(Predicate(itemid[index], supware[index]).pk_2int)))
				_stock_tuple->s_quantity = s_quantity - qty[index];
		} else {
			if((_stock_tuple = stock.read(Predicate(itemid[index], supware[index]).pk_2int)))
				_stock_tuple->s_quantity = s_quantity + 91 - qty[index];
		}

		if(supware[index] != w_id){
			if((_stock_tuple = stock.read(Predicate(itemid[index], supware[index]).pk_2int)))
				_stock_tuple->s_remote_cnt = s_remote_cnt + 1;
		} else {
			if((_stock_tuple = stock.read(Predicate(itemid[index], supware[index]).pk_2int)))
				_stock_tuple->s_order_cnt = s_order_cnt + 1;
		}

		Numeric<6,2> ol_amount = qty[index] * i_price.getDouble() * (1 + w_tax.getDouble() + d_tax.getDouble()) * (1 - c_discount.getDouble());
		if(!(_orderline_tuple = orderline.insert(new OrderLine::Tuple(o_id, d_id, w_id, index+1, itemid[index], supware[index], 0, qty[index], ol_amount, s_dist)))) return;
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


int main() {
	size_t times = 1000000;

	cout << endl;
	cout << "Order: " << order.data.size() << " tuples\n";
	cout << "NewOrder: " << neworder.data.size() << " tuples\n";
	cout << "OrderLine: " << orderline.data.size() << " tuples\n";
	cout << "Executing "<< times<< " transactions newOrder... \n\n";
	auto start=high_resolution_clock::now();
	for(size_t i = 0; i<times; i++){
		newOrderRandom(0);
	}
	auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
	cout << "Transactions accomplished in " << end << "s\n";
	cout << "Speed: " << times/(double)end << " transactions/s\n";

	cout << endl;
	cout << "Order: " << order.data.size() << " tuples\n";
	cout << "NewOrder: " << neworder.data.size() << " tuples\n";
	cout << "OrderLine: " << orderline.data.size() << " tuples\n";
}

/*
 * main.cpp
 *
 *  Created on: Jan 19, 2017
 *      Author: anhmt90
 */
#include <iostream>
#include <memory>
#include <algorithm>
#include <thread>
//#include <climits>
//#include <signal.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/wait.h>
//#include <atomic>
//#include <dlfcn.h>

using namespace std;

#include "TPCC_Schema.hpp"

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
const int32_t warehouses=5;

/*------------------------------------Function newOrder()----------------------------------------------*/
void newOrder(int32_t w_id, int32_t d_id, int32_t c_id, int32_t items, int32_t supware[15], int32_t itemid[15], int32_t qty[15], Timestamp datetime){
	Warehouse_PK::iterator _warehouse_itr;
	District_PK::iterator _district_itr;
	Customer_PK::iterator _customer_itr;
	NewOrder_PK::iterator _neworder_itr;
	Order_PK::iterator _order_itr;
	OrderLine_PK::iterator _orderline_itr;
	Item_PK::iterator _item_itr;
	Stock_PK::iterator _stock_itr;



	_warehouse_itr = warehouse.pk_index.find(Predicate(w_id).pk_int);
	if(_warehouse_itr == warehouse.pk_index.end()) return;
	auto w_tax = warehouse.data[_warehouse_itr->second].w_tax;

	_customer_itr = customer.pk_index.find(Predicate(c_id, d_id, w_id).pk_3int);
	if(_customer_itr == customer.pk_index.end()) return;
	auto c_discount = customer.data[_customer_itr->second].c_discount;

	_district_itr = district.pk_index.find(Predicate(d_id, w_id).pk_2int);
	if(_district_itr == district.pk_index.end()) return;
	auto o_id = district.data[_district_itr->second].d_next_o_id;
	auto d_tax = district.data[_district_itr->second].d_tax;

	uint32_t all_local = 1;
	for(int index = 0; index < items; ++index){
		if(w_id != supware[index])
			all_local = 0;
	}

	if(order.pk_index.find(Predicate(o_id, d_id, w_id).pk_3int) == order.pk_index.end()){
		order.data.push_back(*(new Order::Tuple(o_id, d_id, w_id, c_id, datetime, 0, items, all_local)));
		order.pk_index.insert(make_pair(Predicate(o_id, d_id, w_id).pk_3int, order.data.size()-1));
	} else return;


	if(neworder.pk_index.find(Predicate(o_id, d_id, w_id).pk_3int) == neworder.pk_index.end()){
		neworder.data.push_back(*(new NewOrder::Tuple(o_id, d_id, w_id)));
		neworder.pk_index.insert(make_pair(Predicate(o_id, d_id, w_id).pk_3int, neworder.data.size()-1));
	} else return;


	for(int index = 0; index < items; ++index){
		_item_itr = item.pk_index.find(Predicate(itemid[index]).pk_int);
		if(_item_itr == item.pk_index.end()) return;
		auto i_price = item.data[_item_itr->second].i_price;


		auto s_i_id = itemid[index];
		auto s_w_id = supware[index];

		_stock_itr = stock.pk_index.find(Predicate(s_i_id, s_w_id).pk_2int);
		if(_stock_itr == stock.pk_index.end()) return;
		auto s_quantity = stock.data[_stock_itr->second].s_quantity;
		auto s_remote_cnt = stock.data[_stock_itr->second].s_remote_cnt;
		auto s_order_cnt = stock.data[_stock_itr->second].s_order_cnt;
		Char<24> s_dist;
		switch(d_id){
		case 1 : s_dist = stock.data[_stock_itr->second].s_dist_01; break;
		case 2 : s_dist = stock.data[_stock_itr->second].s_dist_02; break;
		case 3 : s_dist = stock.data[_stock_itr->second].s_dist_03; break;
		case 4 : s_dist = stock.data[_stock_itr->second].s_dist_04; break;
		case 5 : s_dist = stock.data[_stock_itr->second].s_dist_05; break;
		case 6 : s_dist = stock.data[_stock_itr->second].s_dist_06; break;
		case 7 : s_dist = stock.data[_stock_itr->second].s_dist_07; break;
		case 8 : s_dist = stock.data[_stock_itr->second].s_dist_08; break;
		case 9 : s_dist = stock.data[_stock_itr->second].s_dist_09; break;
		case 10 : s_dist = stock.data[_stock_itr->second].s_dist_10; break;
		}


		if(s_quantity > qty[index]){
			_stock_itr = stock.pk_index.find(Predicate(s_i_id, s_w_id).pk_2int);
			if(_stock_itr != stock.pk_index.end()){
				stock.data[_stock_itr->second].s_quantity = s_quantity - qty[index];
			}
		}
		else {
			_stock_itr = stock.pk_index.find(Predicate(s_i_id, s_w_id).pk_2int);
			if(_stock_itr != stock.pk_index.end()){
				stock.data[_stock_itr->second].s_quantity = s_quantity + 91 - qty[index];
			}
		}


		if(supware[index] != w_id){
			_stock_itr = stock.pk_index.find(Predicate(s_i_id, s_w_id).pk_2int);
			if(_stock_itr != stock.pk_index.end()){
				stock.data[_stock_itr->second].s_remote_cnt = s_remote_cnt + 1;
			}
		}
		else {
			_stock_itr = stock.pk_index.find(Predicate(s_i_id, s_w_id).pk_2int);
			if(_stock_itr != stock.pk_index.end()){
				stock.data[_stock_itr->second].s_order_cnt = s_order_cnt + 1;
			}
		}

		Numeric<6,2> ol_amount = qty[index] * i_price.getDouble() * (1 + w_tax.getDouble() + d_tax.getDouble()) * (1 - c_discount.getDouble());

		if(orderline.pk_index.find(Predicate(o_id, d_id, w_id, index+1).pk_4int) == orderline.pk_index.end()){
			orderline.data.push_back(*(new OrderLine::Tuple(o_id, d_id, w_id, index+1, itemid[index], supware[index], 0, qty[index], ol_amount, s_dist)));
			orderline.pk_index.insert(make_pair(Predicate(o_id, d_id, w_id, index+1).pk_4int, orderline.data.size()-1));
		} else return;

	}
	//-----end for -----//
}




/*--------------------------------------------main()---------------------------------------------------------*/
int main(int argc, char* argv[]) {
	cout << endl;
	cout << "Order: " << order.data.size() << " tuples\n";
	cout << "NewOrder: " << neworder.data.size() << " tuples\n";
	cout << "OrderLine: " << orderline.data.size() << " tuples\n";

	size_t times = 1000000;
	cout << "Executing "<< times<< " transactions ... \n";
	auto start=high_resolution_clock::now();
	for(size_t z = 0; z<times; z++){
		//newOrderRandom(0);
		/*------------------------------------Randomize the inputs----------------------------------------------*/
		int32_t w_id=urand(1,warehouses);
		int32_t d_id=urand(1,10);
		int32_t c_id=nurand(1023,1,3000);
		int32_t ol_cnt=urand(5,15);

		int32_t items = ol_cnt;

		int32_t supware[15];
		int32_t itemid[15];
		int32_t qty[15];
		for (int32_t i=0; i<ol_cnt; i++) {
			if (urand(1,100)>1)
				supware[i]=w_id;
			else
				supware[i]=urandexcept(1,warehouses,w_id);
			itemid[i]=nurand(8191,1,100000);
			qty[i]=urand(1,10);
		}
		Timestamp now = 0;
		/*------------------------------------Execute the transaction newOrder-----------------------------------*/
		newOrder(w_id, d_id, c_id, ol_cnt, supware, itemid, qty, now);
	}
	auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
	cout << "Transactions executing accomplished in " << end << "s\n";
	cout << "Speed: " << times/(double)end << " transactions/s\n";

	cout << "Order: " << order.data.size() << " tuples\n";
	cout << "NewOrder: " << neworder.data.size() << " tuples\n";
	cout << "OrderLine: " << orderline.data.size() << " tuples\n";
}



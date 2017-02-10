#include <iostream>
#include <memory>
#include <algorithm>
#include <climits>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <atomic>
#include <dlfcn.h>


#include "../hpp/Parser.hpp"
#include "../hpp/Schema.hpp"
#include "../hpp/Generator.hpp"
#include "../hpp/TPCC_Schema.hpp"
#include "../hpp/Types.hpp"
#include "../hpp/Table.hpp"
#include "../hpp/Attribute.hpp"
#include "../hpp/SemanticAnalyser.hpp"

//#include "../operators/Print.hpp"
//#include "../operators/HashJoin.hpp"
//#include "../operators/Selection.hpp"
//#include "../operators/TableScan.hpp"
//#include "../operators/Operator.hpp"

using namespace std;

//std::unique_ptr<TPCC> tpcc;
TPCC* tpcc;
size_t c = 0;

int64_t sumQuery(){
	int64_t sum = 0;
	auto customer = tpcc->customer;
	auto order = tpcc->order;
	auto orderline = tpcc->orderline;

	unordered_map<tup_3Int, tuple<Integer, Integer, Integer, int64_t>> hashTable1;
	unordered_map<tup_3Int, tuple<int64_t, int64_t>> hashTable2;
	for(size_t i = 0 ; i<order.size();++i){
			auto tupK = make_tuple((Integer)order[i].o_c_id, (Integer)order[i].o_d_id, (Integer)order[i].o_w_id); ///check Required on leftside with equijoins
			auto tupV = make_tuple((Integer)order[i].o_id, (Integer)order[i].o_d_id, (Integer)order[i].o_w_id, order[i].o_ol_cnt.value); ///this is consumer.Required from this.Required.
			hashTable1.insert(make_pair(tupK, tupV));



	}
	for(size_t j = 0; j<customer.size(); ++j){ ///TableScan Operator
		try{
			if(customer[j].c_last.value[0] == 'B'){ /// Selection Operator
				///HashJoin
				auto ht1_val = hashTable1.at(make_tuple((Integer)customer[j].c_id, (Integer)customer[j].c_d_id, (Integer)customer[j].c_w_id));
				auto tupK = make_tuple(get<0>(ht1_val), get<1>(ht1_val), get<2>(ht1_val));
				auto tupV = make_tuple(customer[j].c_balance.value, get<3>(ht1_val));
				hashTable2.insert(make_pair(tupK,tupV));
			}
		}catch(out_of_range &oor){

		}

	}

	for(size_t k = 0; k<orderline.size(); ++k){
		try{
			auto ht2_val = hashTable2.at(make_tuple((Integer)orderline[k].ol_o_id, (Integer)orderline[k].ol_d_id, (Integer)orderline[k].ol_w_id));
			sum+= (orderline[k].ol_quantity.value * orderline[k].ol_amount.value) - (get<0>(ht2_val) * get<1>(ht2_val));
		}catch(out_of_range &oor){

		}

	}

	return sum;
}

void delivery(int32_t w_id, int32_t o_carrier_id, Timestamp datetime){
	for(int32_t d_id = 1; d_id<=10; d_id++){

		int32_t min = INT32_MAX;
		for(auto& no : tpcc->neworder){
			if(no.no_d_id.value == d_id && no.no_w_id.value == w_id){
				if(min > no.no_o_id.value)
					min = no.no_o_id.value;

			}
		}

		if(min < INT32_MAX){
			int32_t o_id = min;
			try{
				//Line 5
				auto i_no = (tpcc->neworder_ix).at(make_tuple((Integer)o_id, (Integer)d_id, (Integer)w_id));
				(tpcc->neworder_ix).erase(make_tuple((Integer)o_id, (Integer)d_id, Integer(w_id)));
				std::swap(tpcc->neworder[i_no], tpcc->neworder.back());
				tpcc->neworder.pop_back();
				(tpcc->neworder_ix).at(make_tuple((Integer)tpcc->neworder[i_no].no_o_id,
						(Integer)tpcc->neworder[i_no].no_d_id, (Integer)tpcc->neworder[i_no].no_w_id)) = i_no;

				//Line 7
				auto i_o = (tpcc->order_ix).at(make_tuple((Integer)o_id, (Integer)d_id, (Integer)w_id));
				auto o_ol_cnt = (tpcc->order)[i_o].o_ol_cnt.value;
				auto o_c_id = (tpcc->order)[i_o].o_c_id.value;

				//Line 8
				(tpcc->order)[i_o].o_carrier_id.value = o_carrier_id;

				//Line 10
				Numeric<6,2> ol_total = 0;

				//Line 11
				for(auto ol_number = 1 ; ol_number<=o_ol_cnt; ol_number++){
					//Line 12
					auto i_ol = (tpcc->orderline_ix).at(make_tuple((Integer)o_id,(Integer)d_id,
							(Integer)w_id,(Integer)ol_number));

					auto ol_amount = (tpcc->orderline)[i_ol].ol_amount;

					//Line 13
					ol_total = ol_total + ol_amount;

					//Line 14
					(tpcc->orderline)[i_ol].ol_delivery_d = datetime;
				}

				//Line 17
				auto i_c = (tpcc->customer_ix).at(make_tuple(Integer(o_c_id),Integer(d_id),Integer(w_id)));
				(tpcc->customer)[i_c].c_balance.value = (tpcc->customer)[i_c].c_balance.value + ol_total.value;
			} catch (out_of_range& oor){
				c++;
			}
		}
	}
}


void newOrder(int32_t w_id, int32_t d_id, int32_t c_id, int32_t items, int32_t supware[15],
		int32_t itemid[15], int32_t qty[15], Timestamp datetime){
	try{
		//Select
		auto i_w = (tpcc->warehouse_ix).at(w_id);
		auto w_tax = (tpcc->warehouse)[i_w].w_tax.value;

		auto i_c = (tpcc->customer_ix).at(make_tuple((Integer)c_id, (Integer) d_id, (Integer) w_id));
		auto c_discount = (tpcc->customer)[i_c].c_discount.value;

		auto i_d = (tpcc->district_ix).at(make_tuple((Integer)d_id, (Integer)w_id));
		auto o_id = (tpcc->district)[i_d].d_next_o_id.value;
		auto d_tax = (tpcc->district)[i_d].d_tax.value;

		//Update
		(tpcc->district)[i_d].d_next_o_id = o_id+1;

		int64_t all_local = 1;

		for(int32_t index = 0; index < items; index++){
			if(w_id != supware[index])
				all_local = 0;
		}

		//Insert
		tpcc->TPCC::o_Insert(make_tuple((Integer)o_id, (Integer)d_id, (Integer)w_id), *(new o_Row(o_id, d_id, w_id,c_id,
				datetime,0,items,all_local)));
		tpcc->no_Insert(make_tuple((Integer)o_id, (Integer)d_id, (Integer)w_id), *(new no_Row((Integer)o_id, (Integer)d_id, (Integer)w_id)));

		for(int32_t index = 0; index < items; index++){
			auto i_i = (tpcc->item_ix).at(itemid[index]);
			auto i_price = (tpcc->item)[i_i].i_price.value;

			auto i_s = (tpcc->stock_ix).at(make_tuple(itemid[index],supware[index]));
			auto s_dist = (tpcc->stock)[i_s].s_dist_01;
			switch(d_id){
			case 2: s_dist = (tpcc->stock)[i_s].s_dist_02; break;
			case 3: s_dist = (tpcc->stock)[i_s].s_dist_03; break;
			case 4: s_dist = (tpcc->stock)[i_s].s_dist_04; break;
			case 5: s_dist = (tpcc->stock)[i_s].s_dist_05; break;
			case 6: s_dist = (tpcc->stock)[i_s].s_dist_06; break;
			case 7: s_dist = (tpcc->stock)[i_s].s_dist_07; break;
			case 8: s_dist = (tpcc->stock)[i_s].s_dist_08; break;
			case 9: s_dist = (tpcc->stock)[i_s].s_dist_09; break;
			case 10: s_dist = (tpcc->stock)[i_s].s_dist_10; break;
			default: s_dist = (tpcc->stock)[i_s].s_dist_01; break;
			}

			//cout << selection.s_quantity <<"\t"<< selection.s_remote_cnt <<"\t"<< selection.s_order_cnt <<"\t"<< s_dist << "\n";
			if((tpcc->stock)[i_s].s_quantity > qty[index]){
				(tpcc->stock)[i_s].s_quantity = (tpcc->stock)[i_s].s_quantity - qty[index];
			} else {
				(tpcc->stock)[i_s].s_quantity = (tpcc->stock)[i_s].s_quantity + 91 - qty[index];
			}

			i_s = (tpcc->stock_ix).at(make_tuple(itemid[index],w_id));
			if(supware[index] != w_id){
				(tpcc->stock)[i_s].s_remote_cnt+=1;
			} else {
				(tpcc->stock)[i_s].s_order_cnt+=1;
			}

			auto ol_amount = (qty[index] * (i_price) * (1.0 + w_tax + d_tax) * (1.0 - c_discount));

			tpcc->ol_Insert(make_tuple((Integer)o_id, (Integer)d_id, (Integer)w_id, (Integer)index+1),
					*(new ol_Row((Integer)o_id, (Integer)d_id, (Integer)w_id, (Integer)index+1, itemid[index], supware[index],0, qty[index], ol_amount, s_dist)));


}
	}catch(out_of_range &oor){
		c++;
	}

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



//atomic<short> loop;


void CHILD_handler(int signum, siginfo_t* si, void* uc){
	int status = 0;
	pid_t childPID = wait(&status);
	cout<<"Child with PID " << childPID << " has terminated.\n";

	struct timespec forkStart, forkEnd;
	clock_gettime(CLOCK_REALTIME, &forkStart);
	pid_t pid = fork();
	clock_gettime(CLOCK_REALTIME, &forkEnd);

	cout<<"PID returned from fork:" << pid << "\n";
	cout << "fork() took "  << (forkEnd.tv_sec - forkStart.tv_sec)
										+ ( forkEnd.tv_nsec - forkStart.tv_nsec )/1E9<< "s to run.\n";
	if(pid == 0){
		auto start=high_resolution_clock::now();
		cout <<  "Query returns: " << (Numeric<12,2>) sumQuery();
		auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
		cout << ", accomplished in " << end << "s\n";
		_exit(0);
	} else if (pid == -1){
		cerr << "Fork failed!\n";
		}
}



/*=========================================================================================================*/
int main(int argc, char* argv[]) {
	/*
	 * Set this 'task' variable from 3 to 5 to run the respective task.
	 */
	unsigned task = 5;

	tpcc = new TPCC();

	SchemaParser sp("schema.sql");
	QueryParser qp;

	switch(task){
/*-------------------------------------------------------Task05---------------------------------------------------------------*/
	case 5:
	{
		try {

			std::unique_ptr<Schema> schema = sp.parse();
			//std::cout << schema->toString() << std::endl;

			SemanticAnalyser semanal(move(schema));
			cout<<"Type 'exit' to quit the console.\n";

			while(true){
				QueryParser qp;
				cout<<"\nQUERY> ";
				std::unique_ptr<Query> query = qp.parseCommand();
				if(!query)
					exit(1);
//				std::cout << query->toString() << std::endl;

				/*----------------------------------------------------------------------------------*/
				auto start=high_resolution_clock::now();
				/*----------------------------------------------------------------------------------*/

				semanal.setQuery(move(query));
				semanal.analyse();
				semanal.build();

//				if(system("./run_query.sh") != 0)
//					continue;
				system("./run_query.sh");

				void* handle=dlopen("./TranslatedQuery.so",RTLD_NOW);
				if (!handle) {
					std::cerr << "dlopen-error: " << dlerror() << std::endl;
					exit(1);
				}

				auto exe =reinterpret_cast<void (*)(TPCC*)>(dlsym(handle, "qExecution"));
				if (!exe) {
					std::cerr << "dlsym-error: " << dlerror() << std::endl;
					exit(1);
				}

				/*----------------------------------------------------------------------------------*/
				auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
				/*----------------------------------------------------------------------------------*/

//				struct timespec exeStart, exeEnd;
//				clock_gettime(CLOCK_REALTIME, &exeStart);
				auto exeStart=high_resolution_clock::now();
				exe(tpcc);
				auto exeEnd = duration_cast<duration<double>>(high_resolution_clock::now()-exeStart).count();
//				clock_gettime(CLOCK_REALTIME, &exeEnd);

				cout << "Query compiled in " << end << "s\n";
				cout << "Query executed in " << exeEnd << "s\n";
//				cout << "Query executed in "  << (exeEnd.tv_sec - exeStart.tv_sec)
//										+ ( exeEnd.tv_nsec - exeStart.tv_nsec )/1E9<< "s.\n";


				if (dlclose(handle)) {
					std::cerr << "dlclose-error: " << dlerror() << std::endl;
					exit(1);
				}

			}

		}catch (...){

		}


	}

		break;
/*-------------------------------------------------------Task04---------------------------------------------------------------*/
	case 4:
	{
		bool useGenCode = true;
		try {
			if(!useGenCode){

				///Parse the Schema
				std::unique_ptr<Schema> schema = sp.parse();
				std::cout << schema->toString() << std::endl;

				///Parse the Query
				std::unique_ptr<Query> query = qp.parseFile("q1.sql");
				std::cout << query->toString() << std::endl;

				///Manually build the algebra tree
				auto customer = query->relations[0];
				auto order = query->relations[1];
				auto orderline = query->relations[2];

				TableScan* scan_customer = new TableScan(customer);
				TableScan* scan_order = new TableScan(order);
				TableScan* scan_orderline = new TableScan(orderline);


				Selection* selection = new Selection(scan_customer);
				selection->predicates.push_back(std::make_pair(Attribute("c_id","Integer",scan_customer->getTableName()), Query::Constant("322", "Integer")));
				selection->predicates.push_back(std::make_pair(Attribute("c_w_id","Integer",scan_customer->getTableName()), Query::Constant("1", "Integer")));
				selection->predicates.push_back(std::make_pair(Attribute("c_d_id","Integer",scan_customer->getTableName()), Query::Constant("1", "Integer")));


				HashJoin* c_hj_o = new HashJoin(selection, scan_order);
				c_hj_o->equijoins.push_back(std::make_pair(Attribute("c_w_id","Integer",scan_customer->getTableName()),Attribute("o_w_id","Integer",scan_order->getTableName())));
				c_hj_o->equijoins.push_back(std::make_pair(Attribute("c_d_id","Integer",scan_customer->getTableName()),Attribute("o_d_id","Integer",scan_order->getTableName())));
				c_hj_o->equijoins.push_back(std::make_pair(Attribute("c_id","Integer",scan_customer->getTableName()),Attribute("o_c_id","Integer",scan_order->getTableName())));
				c_hj_o->hashTableName ="hashJoin1";


				HashJoin* c_o_hj_ol = new HashJoin(c_hj_o, scan_orderline);
				c_o_hj_ol->equijoins.push_back(std::make_pair(Attribute("o_w_id","Integer",scan_order->getTableName()),Attribute("ol_w_id","Integer",scan_orderline->getTableName())));
				c_o_hj_ol->equijoins.push_back(std::make_pair(Attribute("o_d_id","Integer",scan_order->getTableName()),Attribute("ol_d_id","Integer",scan_orderline->getTableName())));
				c_o_hj_ol->equijoins.push_back(std::make_pair(Attribute("o_id","Integer",scan_order->getTableName()),Attribute("ol_o_id","Integer",scan_orderline->getTableName())));
				c_o_hj_ol->hashTableName = "hashJoin2";

				Print* print = new Print(c_o_hj_ol);
				print->required.push_back(Attribute("c_first","Varchar<16>", scan_customer->getTableName()));
				print->required.push_back(Attribute("c_last","Varchar<16>", scan_customer->getTableName()));
				print->required.push_back(Attribute("o_all_local","Numeric<1,0>", scan_order->getTableName()));
				print->required.push_back(Attribute("ol_amount","Numeric<6,2>", scan_orderline->getTableName()));

				print->produce();
			}

			else{
				auto& customer = tpcc->customer;
				auto& order = tpcc->order;
				auto& orderline = tpcc->orderline;
				auto& item = tpcc->item;
				auto& stock = tpcc->stock;
				auto& warehouse = tpcc->warehouse;
				auto& district = tpcc->district;
				auto& neworder = tpcc->neworder;
				unordered_multimap<tuple<Integer, Integer, Integer>, tuple<Varchar<16>, Integer>> hashJoin_2;
				unordered_multimap<tuple<Integer>, tuple<Varchar<16>>> hashJoin_1;
				for (auto& t : customer) {
					 try{
						 auto c_last = t.c_last;
						 auto c_id = t.c_id;
						 auto tupK = make_tuple(c_id);
						 auto tupV = make_tuple(c_last);
						 hashJoin_1.insert(make_pair(tupK, tupV));
					 } catch(...){ }
				}
				for (auto& t : order) {
					 try{
					 auto range = hashJoin_1.equal_range(make_tuple(t.o_c_id));
					 for(auto itr = range.first; itr != range.second; ++itr){
						 auto c_last = get<0>(itr->second);
						 auto o_id = t.o_id;
						 auto o_d_id = t.o_d_id;
						 auto o_w_id = t.o_w_id;
						 auto tupK = make_tuple(o_id, o_d_id, o_w_id);
						 auto tupV = make_tuple(c_last, o_id);
						 hashJoin_2.insert(make_pair(tupK, tupV));
					 }
					 } catch(...){ }
				}
				for (auto& t : orderline) {
					 try{
					 if(t.ol_number.value == 1 ) {
					 auto range = hashJoin_2.equal_range(make_tuple(t.ol_o_id, t.ol_d_id, t.ol_w_id));
					 for(auto itr = range.first; itr != range.second; ++itr){
						 auto c_last = get<0>(itr->second);
						 auto o_id = get<1>(itr->second);
						 auto ol_dist_info = t.ol_dist_info;
					 std::cout << c_last <<"\t " <<o_id <<"\t " <<ol_dist_info<< endl;
					 }
					 }
					 } catch(...){ }

				}
			}


		} catch (ParserError& e) {
			std::cerr << e.what() << std::endl;
		}
	}
		break;
/*-------------------------------------------------------Task03---------------------------------------------------------------*/
	case 3:
	{
//		for(unsigned i = 1 ; i<=10; ++i){
//			auto start=high_resolution_clock::now();
//			cout <<  "Query " << i << " returns: " << (Numeric<12,2>) sumQuery();
//			auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
//			cout <<  ", accomplished in " << end << "s\n";
//		}
//
//		struct sigaction sa;
//		sa.sa_flags = SA_SIGINFO;
//		sigemptyset(&sa.sa_mask);
//		sa.sa_sigaction = CHILD_handler;
//		sigaction(SIGCHLD, &sa, NULL);
//
//		struct timespec forkStart, forkEnd;
//		clock_gettime(CLOCK_REALTIME, &forkStart);
//		pid_t pid = fork();
//		cout<<"PID returned from fork:" << pid << "\n";
//		clock_gettime(CLOCK_REALTIME, &forkEnd);
//		cout << "fork() took "  << (forkEnd.tv_sec - forkStart.tv_sec)
//						+ ( forkEnd.tv_nsec - forkStart.tv_nsec )/1E9<< "s to run.\n";
//
//		if(pid == 0){
//			auto start=high_resolution_clock::now();
//			cout <<  "Query returns: " << (Numeric<12,2>) sumQuery();
//			auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
//			cout << ", accomplished in " << end << "s\n";
//			_exit(0);
//		} else if (pid == -1){
//			cerr << "Fork failed!\n";
//		}

		size_t times = 1000000;
		cout << "\nPK Index of Order: " << tpcc->order_ix.size() << " indexes\n";
		cout << "PK Index of NewOrder: " << tpcc->neworder_ix.size() << " indexes\n";
		cout << "PK Index of OrderLine: " << tpcc->orderline_ix.size() << " indexes\n";

		cout << "Order: " << tpcc->order.size() << " tuples\n";
		cout << "NewOrder: " << (tpcc->neworder).size() << " tuples\n";
		cout << "OrderLine: " << (tpcc->orderline).size() << " tuples\n\n";
		cout << "Executing "<< times<< " transactions ... \n\n";
		auto start=high_resolution_clock::now();
		for(size_t i = 0; i<times; i++){
			//newOrderRandom(0);
			oltp(0);
			//deliveryRandom(0);
		}
		auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
		cout << "Transactions accomplished in " << end << "s\n";
		cout << "Speed: " << times/(double)end << " transactions/s\n";

		cout << "Order: " << (tpcc->order).size() << " tuples\n";
		cout << "NewOrder: " << (tpcc->neworder).size() << " tuples\n";
		cout << "OrderLine: " << (tpcc->orderline).size() << " tuples\n";

		cout << "PK Index of Order: " << (tpcc->order_ix).size() << " indexes\n";
		cout << "PK Index of NewOrder: " << (tpcc->neworder_ix).size() << " indexes\n";
		cout << "PK Index of OrderLine: " << (tpcc->orderline_ix).size() << " indexes\n\n";
	}
		break;
/*-------------------------------------------------------Default---------------------------------------------------------------*/
	default:
		cerr<<"Please set the 'task' variable (3 to 5) to run the specified task";
	}






	/*--------------------------------------Task01 + Task02-----------------------------------------*/
//	size_t times = 1000000;
//	cout << "\nPK Index of Order: " << tpcc->o_ix.size() << " indexes\n";
//	cout << "PK Index of NewOrder: " << tpcc->no_ix.size() << " indexes\n";
//	cout << "PK Index of OrderLine: " << tpcc->ol_ix.size() << " indexes\n";
//
//	cout << "Order: " << tpcc->o.size() << " tuples\n";
//	cout << "NewOrder: " << (tpcc->no).size() << " tuples\n";
//	cout << "OrderLine: " << (tpcc->ol).size() << " tuples\n\n";
//	cout << "Executing "<< times<< " transactions ... \n\n";
//	auto start=high_resolution_clock::now();
//	for(size_t i = 0; i<times; i++){
//		//newOrderRandom(0);
//		oltp(0);
//		//deliveryRandom(0);
//	}
//	auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
//	cout << "Transactions accomplished in " << end << "s\n";
//	cout << "Speed: " << times/(double)end << " transactions/s\n";
//
//	cout << "Order: " << (tpcc->o).size() << " tuples\n";
//	cout << "NewOrder: " << (tpcc->no).size() << " tuples\n";
//	cout << "OrderLine: " << (tpcc->ol).size() << " tuples\n";
//
//	cout << "PK Index of Order: " << (tpcc->o_ix).size() << " indexes\n";
//	cout << "PK Index of NewOrder: " << (tpcc->no_ix).size() << " indexes\n";
//	cout << "PK Index of OrderLine: " << (tpcc->ol_ix).size() << " indexes\n\n";

/*------------------------------------------Task02---------------------------------------------------*/

//	   if (argc != 2) {
//	      std::cerr << "usage: " << argv[0] << " <schema file>" << std::endl;
//	      return -1;
//	   }
//
//	   SchemaParser sp(argv[1]);
//
//	   try {
//		   std::unique_ptr<Schema> schema = sp.parse();
//		   std::cout << schema->toString() << std::endl;
//		   codeGenerator(*schema);
//	   } catch (ParserError& e) {
//		   std::cerr << e.what() << std::endl;
//	   }
//

	return 0;
}

#include "./hpp/TPCC_Schema.hpp" 
#include "./operators/Operator.hpp" 

using namespace std; 

extern "C" void qExecution(TPCC* tpcc){ 
auto& customer = tpcc->customer; 
auto& order = tpcc->order; 
auto& orderline = tpcc->orderline; 
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
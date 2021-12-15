#include "./src/TPCC_Schema.hpp" 
#include "./src/Operator.hpp" 

using namespace std; 

extern "C" void _run(){ 
	 std::cout << "c_first" <<"\t\t " <<"c_last" <<"\t\t " <<"o_all_local" <<"\t\t " <<"ol_amount"<< endl; 
	 std::cout << "_______________________________________________________________________________"<< endl; 
unordered_multimap<tuple<Integer, Integer, Integer>, tuple<Varchar<16>, Varchar<16>, Numeric<1,0>>> hashJoin2; 
unordered_multimap<tuple<Integer, Integer, Integer>, tuple<Varchar<16>, Varchar<16>>> hashJoin1; 
for (auto& t : customer.data) { 
	 try{
	 if(t.c_id == 322 && t.c_w_id == 1 && t.c_d_id == 1 ) { 
		 auto c_first = t.c_first;
		 auto c_last = t.c_last;
		 auto c_w_id = t.c_w_id;
		 auto c_d_id = t.c_d_id;
		 auto c_id = t.c_id;
		 auto tupK = make_tuple(c_w_id, c_d_id, c_id); 
		 auto tupV = make_tuple(c_first, c_last); 
		 hashJoin1.insert(make_pair(tupK, tupV)); 
	 } 
	 } catch(...){ } 
} 
for (auto& t : order.data) { 
	 try{
	 auto range = hashJoin1.equal_range(make_tuple(t.o_w_id, t.o_d_id, t.o_c_id)); 
	 for(auto itr = range.first; itr != range.second; ++itr){ 
		 auto c_first = get<0>(itr->second); 
		 auto c_last = get<1>(itr->second); 
		 auto o_all_local = t.o_all_local;
		 auto o_w_id = t.o_w_id;
		 auto o_d_id = t.o_d_id;
		 auto o_id = t.o_id;
		 auto tupK = make_tuple(o_w_id, o_d_id, o_id); 
		 auto tupV = make_tuple(c_first, c_last, o_all_local); 
		 hashJoin2.insert(make_pair(tupK, tupV)); 
	 } 
	 } catch(...){ } 
} 
for (auto& t : orderline.data) { 
	 try{
	 auto range = hashJoin2.equal_range(make_tuple(t.ol_w_id, t.ol_d_id, t.ol_o_id)); 
	 for(auto itr = range.first; itr != range.second; ++itr){ 
		 auto c_first = get<0>(itr->second); 
		 auto c_last = get<1>(itr->second); 
		 auto o_all_local = get<2>(itr->second); 
		 auto ol_amount = t.ol_amount;
	 std::cout << c_first <<"\t\t " <<c_last <<"\t\t " <<o_all_local <<"\t\t " <<ol_amount<< endl; 
	 } 
	 } catch(...){ } 
} 
}
#include "./src/TPCC_Schema.hpp" 
#include "./src/Operator.hpp" 

using namespace std; 

extern "C" void _run(){ 
	 std::cout << "o_id" <<"\t\t " <<"ol_dist_info"<< endl; 
	 std::cout << "______________________________________________________________________________________________________________________"<< endl; 
unordered_multimap<tuple<Integer, Integer, Integer>, tuple<Integer>> hashJoin_1; 
for (auto& t : order.data) { 
	 try{
		 auto o_id = t.o_id;
		 auto o_d_id = t.o_d_id;
		 auto o_w_id = t.o_w_id;
		 auto tupK = make_tuple(o_id, o_d_id, o_w_id); 
		 auto tupV = make_tuple(o_id); 
		 hashJoin_1.insert(make_pair(tupK, tupV)); 
	 } catch(...){ } 
} 
for (auto& t : orderline.data) { 
	 try{
	 if(t.ol_number == 1 && t.ol_o_id == 100 ) { 
	 auto range = hashJoin_1.equal_range(make_tuple(t.ol_o_id, t.ol_d_id, t.ol_w_id)); 
	 for(auto itr = range.first; itr != range.second; ++itr){ 
		 auto o_id = get<0>(itr->second); 
		 auto ol_dist_info = t.ol_dist_info;
	 std::cout << o_id <<"\t\t " <<ol_dist_info<< endl; 
	 } 
	 } 
	 } catch(...){ } 
} 
}
#include "./src/TPCC_Schema.hpp" 
#include "./src/Operator.hpp" 

using namespace std; 

extern "C" void _run(){ 
	 std::cout << "w_id"<< endl; 
	 std::cout << "______________________________________________________________________________________________________________________"<< endl; 
for (auto& t : warehouse.data) { 
	 try{
		 auto w_id = t.w_id;
	 std::cout << w_id<< endl; 
	 } catch(...){ } 
} 
}
//#include <vector>
//#include <string>
//#include <iostream>
//#include "Selection.hpp"
//using namespace std;
//
//void Selection::produce(){
//	input->produce();
//}
//
//void Selection::consume(Operator* op){
//	size_t pr_size = predicates.size();
//	cout<<"if(";
//	for(unsigned z = 0; z < pr_size; ++z){
//		const auto& p = predicates[z];
//		cout<< tablename+"[i]."+get<0>(p).name+".value == "+get<1>(p).value;
//		if (z+1 < pr_size)
//			cout<<" && ";
//	}
//	cout << " ) { \n";
//	consumer->consume(this);
//	cout << "} \n";
//}

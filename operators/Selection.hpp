//#include "Operator.hpp"
//#include "../hpp/Query.hpp"
//#include "../hpp/Table.hpp"
//#include <vector>
//#include <string>
//using namespace std;
//
//class Selection : public Operator {
//public:
//	Operator* input;
//	string tablename;
//	std::vector<pair<Table::Attribute, Query::Constant>> predicates;
//	Selection(Operator* input): input(input){
//		input->setConsumer(this);
//		this->getProduced = input->getProduced;
//		this->tablename = input->tablename;
//		for(unsigned i = 0; i< predicates.size(); ++i)
//			getRequired.push_back(get<0>(predicates[i]));
//
//		for(unsigned i = 0; i< getProduced.size(); ++i){
//			for(unsigned j = 0; j< consumer->getRequired.size(); ++j){
//				if(getProduced[i].name == consumer->getRequired[j].name){
//					getRequired.push_back(getProduced[j]);
//				}
//			}
//		}
//	}
//	~Selection(){ };
//
//	void produce();
//	void consume(Operator* op);
//};

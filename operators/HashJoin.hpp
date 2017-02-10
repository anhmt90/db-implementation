///*
// * HashJoin.hpp
// *
// *  Created on: Nov 17, 2016
// *      Author: anhmt90
// */
//#include "Operator.hpp"
//#include <vector>
//#include <string>
//using namespace std;
//
//class HashJoin : public Operator {
//	Operator* leftInput;
//	Operator* rightInput;
//public:
//	vector<string> tablenames;
//	vector<pair<Table::Attribute, Table::Attribute>> equijoins;
//	string hashTableName;
//	HashJoin(Operator* leftInput, Operator* rightInput):
//		leftInput(leftInput), rightInput(rightInput){
//		isJoin = true;
//		leftInput->setConsumer(this);
//		rightInput->setConsumer(this);
//		this->getProduced.reserve(leftInput->getProduced.size() + rightInput->getProduced.size());
//		this->getProduced.insert(this->getProduced.end(), leftInput->getProduced.begin(), leftInput->getProduced.end());
//		this->getProduced.insert(this->getProduced.end(), rightInput->getProduced.begin(), rightInput->getProduced.end());
//
//		if(!consumer->isJoin){ ///consumer is not a HashJoin
//			///getRequired comprises all attributes of consumer.getRequired and equijoins.
//			this->getRequired.reserve(consumer->getRequired.size() + equijoins.size()*2);
//			this->getRequired.insert(this->getRequired.end(), consumer->getRequired.begin(), consumer->getRequired.end());
//
//			///Take out all attributes in equijoins into getRequired.
//			vector<Table::Attribute> first;
//			vector<Table::Attribute> second;
//			for(unsigned j = 0; j<equijoins.size(); ++j){
//				first.push_back(get<0>(equijoins[j]));
//				second.push_back(get<1>(equijoins[j]));
//			}
//			this->getRequired.insert(this->getRequired.end(), first.begin(), first.end());
//			this->getRequired.insert(this->getRequired.end(), second.begin(), second.end());
//		}
//		else { ///Consumer is another HashJoin
//
//			///getRequired comprises a set of intersect between (consumer.getRequired) and (this.getProduced)
//			size_t prod_size = this->getProduced.size();
//			size_t csm_req_size = consumer->getRequired.size();
//
//			for(unsigned i =0; i<prod_size; ++i){
//				for(unsigned j = 0; j < csm_req_size; ++j){
//					string prod_name = this->getProduced[i].name;
//					string csm_req_name = consumer->getRequired[j].name;
//					if(prod_name == csm_req_name)
//						this->getRequired.push_back(this->getProduced[i]);
//				}
//			}
//		}
//
//	}
//	void produce ();
//	void consume (Operator* op);
//};
//
//

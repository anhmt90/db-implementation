///*
// * HashJoin.cpp
// *
// *  Created on: Nov 17, 2016
// *      Author: anhmt90
// */
//
//#include <vector>
//#include <string>
//#include <iostream>
//#include <stdlib.h>
//#include "HashJoin.hpp"
//using namespace std;
//
//void HashJoin::produce(){
//	cout << "unordered_map<tuple<";
//	for(unsigned z = 0, eq_size = equijoins.size(); z<eq_size; ++z){
//		const auto& eqj = equijoins[z];
////		if(get<0>(eqj).type != get<1>(eqj).type)
////			throw OperatorError("Not same attribute type at equi-join "+
////					get<0>(eqj).name + " = " + get<1>(eqj).name);
//		cout << get<0>(eqj).type;
//		(z+1 < eq_size)?cout << ", ":cout<< ">, ";
//	}
//	cout << "tuple<";
//	for(unsigned z = 0, req_size = getRequired.size(); z<req_size; ++z){
//		cout<< getRequired[z].type;
//		(z+1 < req_size)?cout << ", ":cout<< ">> "<<hashTableName<<"; \n";
//	}
//	leftInput->produce();
//	rightInput->produce();
//}
//
//void HashJoin::consume(Operator* op){
//	if(leftInput == op){
//
//		if(leftInput->isJoin){
//			cout <<"auto tupK = make_tuple(" ;
//			size_t joins_size = equijoins.size();
//
//			for(unsigned i=0; i<joins_size; ++i){
//				cout<<"get<"<<i<<">(ht_val)";
//				if(i+1 < joins_size)
//					cout<<", ";
//			}
//			cout<< "); \n";
//			cout <<"auto tupV = make_tuple(" ;
//
//			///value size of the input hashtable
//			unsigned val_size_input = 0;
//			for(unsigned i=0; i<leftInput->getRequired.size(); ++i){
//				for(unsigned j=0; j<getRequired.size(); ++j){
//					if(leftInput->getRequired[i].name == this->getRequired[j].name)
//						++val_size_input;
//				}
//			}
//			int t = equijoins.size() - val_size_input;
//			size_t val_size = abs(t);
//			for(unsigned i=0; i<val_size; ++i){
//				cout<<"get<"<<i<<">(ht_val)";
//				if(i+1<val_size)
//					cout<<", ";
//			}
//
//			///check if more value should come in tupV
//			if((getRequired.size() - 2*equijoins.size() - val_size) == 0) {
//				cout<<"); \n";
//			}
//		}
//
//
//		else{
//			auto inputRequired = leftInput->getRequired;
//			vector<Table::Attribute> buildKeys;
//			//		string l;
//			//		l = get<0>(equijoins[0]).tablename;
//
//			///Keys of the build phase of HashTable is the intersection of all attributes in equijoins and the leftInput.getRequired
//			for(unsigned i=0; i<equijoins.size(); ++i){
//				auto& join = equijoins[i];
//				//			(l == get<0>(join).tablename)?leftOfEqui.push_back(get<0>(join)):rightOfEqui.push_back(get<0>(join));
//				//			(l == get<1>(join).tablename)?leftOfEqui.push_back(get<1>(join)):rightOfEqui.push_back(get<1>(join));
//				for(unsigned j=0; j<inputRequired.size(); ++j){
//					if(get<0>(join).name == inputRequired[j].name)
//						buildKeys.push_back(get<0>(join));
//					else if (get<1>(join).name == inputRequired[j].name){
//						buildKeys.push_back(get<1>(join));
//					}
//				}
//			}
//
//			cout <<"auto tupK = make_tuple(" ;
//			for(unsigned i=0, size = buildKeys.size(); i<size; ++i){
//				auto& key = buildKeys[i];
//				cout<<"("<<key.type<<")"<<key.tablename<<"[i]."<<key.name;
//				if(i+1 < size)
//					cout<<", ";
//			}
//			cout<< "); \n";
//
//			///value of each HashTable entry is an attibute set from the intersection of (getRequired) and (consumer.getRequired)
//			vector<Table::Attribute> buildValues;
//			for(unsigned i=0, size = getRequired.size(); i<size; ++i){
//				auto& req = getRequired[i];
//				for(unsigned j=0, csm_req_size = consumer->getRequired.size(); j< csm_req_size; ++j){
//					auto csm_req = consumer->getRequired[j];
//					if(req.name == csm_req.name)
//						buildValues.push_back(req);
//				}
//			}
//			cout <<"auto tupV = make_tuple(" ;
//			for(unsigned i=0, size = buildValues.size(); i<size; ++i){
//				auto& val = buildValues[i];
//				cout<<"("<<val.type<<")"<<val.tablename<<"[i]."<<val.name;
//				if(i+1 < size)
//					cout<<", ";
//			}
//			cout<< "); \n";
//			cout<< hashTableName<<".insert(make_pair(tupK, tupV)); \n";
//			cout<< "} \n";
//		}
//
//
//	}
//	else{
//
//		auto inputRequired = rightInput->getRequired;
//		vector<Table::Attribute> probeKeys;
//
//		for(unsigned i=0; i<equijoins.size(); ++i){
//			auto& join = equijoins[i];
//			for(unsigned j=0; j<inputRequired.size(); ++j){
//				auto first = get<0>(join);
//				auto second = get<1>(join);
//				if(first.name == inputRequired[j].name)
//					probeKeys.push_back(get<0>(join));
//				else if (second.name == inputRequired[j].name){
//					probeKeys.push_back(get<1>(join));
//				}
//			}
//		}
//		cout<< "auto ht_val = " << hashTableName <<".at(make_tuple(";
//		for(unsigned i=0, size = probeKeys.size(); i<size; ++i){
//			auto& key = probeKeys[i];
//			cout<<"("<<key.type<<")"<<key.tablename<<"[i]."<<key.name;
//			if(i+1 < size)
//				cout<<", ";
//		}
//		cout<< "); \n";
//
////		if(leftInput->isJoin){
////
////
////		}
//
//		consumer->consume(this);
//
//
//	}
//}
//

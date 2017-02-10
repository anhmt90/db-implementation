/*
 * Operator.hpp
 *
 *  Created on: Nov 16, 2016
 *      Author: anhmt90
 */

#ifndef H_OPERATORS_OPERATOR_HPP
#define H_OPERATORS_OPERATOR_HPP

#include <iostream>
#include <fstream>

#include "../hpp/Table.hpp"
#include "../hpp/Query.hpp"
#include "../hpp/Attribute.hpp"

using namespace std;



class Operator {
private:
//	unique_ptr<Query> init(unique_ptr<Query>);
//	void end();

public:
	Operator* consumer;
	string tablename;

	///stores Attributes to be produced by this Operator
	vector<Attribute> produced;

	///stores Attributes to be required by this Operator
	vector<Attribute> required;

	vector<Attribute> ht_vals;
	vector<Attribute> ht_keys;
	vector<Attribute> probe_keys;

	virtual void produce() =0;
	virtual void consume(Operator* op)=0;

	///if not be overwritten, consumer of an Op. is itself
	virtual void setConsumer(Operator* consumer);

	vector<Attribute> getRequired() {
		return this->required;
	}

	vector<Attribute> getProduced(){
		return this->produced;
	}

//	Operator() {
//		ofs.open("TranslatedQuery.cpp", ofstream::app);
//	}
	//		Operator();
	virtual ~Operator(){ };
	};

//////---------------------------------------------------------------------------
//Operator::~Operator() { }

/*=========================================================================================================*/

class TableScan : public Operator {
Table input;

public:
	TableScan(Table input): input(input){
		this->produced = input.attributes;
		tablename = input.name;
	}
	~TableScan(){ };
	string getTableName(){
		return input.name;
	}
	void produce();
	void consume(Operator* op);
};

/*=========================================================================================================*/

class Selection : public Operator {
public:
	Operator* input;
	std::vector<pair<Attribute, Query::Constant>> predicates;
	Selection(Operator* input): input(input){
		input->setConsumer(this);
		this->produced = input->produced;
		this->tablename = input->tablename;

		for(auto& p: predicates){
			required.push_back(p.first);
		}
	}
	~Selection(){ };

	void produce();
	void consume(Operator* op);
};



struct Predicate {
	const Attribute& attr;
	Predicate(const Attribute& attr) : attr(attr) {}
	bool operator()(const Attribute& a) const {
		return a.name == attr.name;
	}
};

/*=========================================================================================================*/

class HashJoin : public Operator {
	Operator* leftInput;
	Operator* rightInput;
	vector<Attribute> ht;
public:
	vector<string> tablenames;

	vector<pair<Attribute, Attribute>> equijoins;
	string hashTableName;

	HashJoin(Operator* leftInput, Operator* rightInput):
		leftInput(leftInput), rightInput(rightInput){

		/*
		 * set this HashJoin as the Consumer of the left child and right child
		 * */
		leftInput->setConsumer(this);
		rightInput->setConsumer(this);

		/*
		 * Prepare for this.produced
		 * produced comprises all produced attributes of left child and right child.
		 * */
		this->produced.reserve(leftInput->produced.size() + rightInput->produced.size());
		this->produced.insert(this->produced.end(), leftInput->produced.begin(), leftInput->produced.end());
		this->produced.insert(this->produced.end(), rightInput->produced.begin(), rightInput->produced.end());

	}
	void produce ();
	void consume (Operator* op);
};

/*=========================================================================================================*/

class Print : public Operator {
	Operator* input;
public:
	Print(Operator* input) : input(input) {
		input->setConsumer(this);
	}
	//Projection
	void produce();
	void consume(Operator* op);
};

/*=========================================================================================================*/
#endif

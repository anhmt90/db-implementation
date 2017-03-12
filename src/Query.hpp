/*
 * Query.hpp
 *
 *  Created on: Nov 13, 2016
 *      Author: anhmt90
 */
#ifndef H_QUERY_HPP
#define H_QUERY_HPP

#include "Table.hpp"
#include "Attribute.hpp"
#include <string>
#include <vector>

using namespace std;


template<typename T>
struct NamePredicate {
	const string& name;
	NamePredicate(const string& name) : name(name) {}
	bool operator()(const T& t) const {
		return t.name == name;
	}
};

struct Query {

	struct Constant{
		string value;
		parserTypes::Tag type;
		string typeStr;
		Constant() {}
		Constant(string value, string typeStr):value(value), typeStr(typeStr){ }
		~Constant(){ };
	};


	std::vector<Table> relations;
	std::vector<Attribute> projections;
	std::vector<pair<Attribute, Query::Constant>> selections;
	std::vector<pair<Attribute, Attribute>> joins;

	std::string toString() const;
};

#endif


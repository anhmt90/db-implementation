/*
 * SemanticAnalyser.hpp
 *
 *  Created on: Nov 23, 2016
 *      Author: anhmt90
 */
#ifndef H_SEMANTICANALYSER_HPP
#define H_SEMANTICANALYSER_HPP

#include "../hpp/Query.hpp"
#include "../hpp/Schema.hpp"
#include "../operators/Operator.hpp"

using namespace std;


class SemanticError : std::exception {
   std::string msg;
   public:
   SemanticError(const std::string& m) : msg(m) {}
   ~SemanticError() throw() {}
   const char* what() const throw() {
	  return msg.c_str();
   }
};

class SemanticAnalyser {
	unique_ptr<Schema> schema;
	unique_ptr<Query> query;
public:
	SemanticAnalyser(unique_ptr<Schema> schema): schema(move(schema)){ }
	~SemanticAnalyser(){ };

	void setQuery(unique_ptr<Query>	query){
		this->query = move(query);
	}

	/// check if tables, attributes and join conditions exist.
	void analyse();

	///build algebra tree
	void build();


};


#endif

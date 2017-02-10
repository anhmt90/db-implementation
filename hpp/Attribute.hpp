/*
 * Attribute.hpp
 *
 *  Created on: Nov 18, 2016
 *      Author: anhmt90
 */
#ifndef H_Attribute
#define H_Attribute

#include <string>
#include <vector>
#include <regex>
#include <stdlib.h>
#include "Types.hpp"

using namespace std;



struct Attribute{
	string name;
	string typeStr;
	parserTypes::Tag type;
	unsigned len;
	unsigned dec;
	string tablename;
	bool notNull;
	Attribute(){
		notNull = true;
		len = 0;
		dec = 0;
	};
	Attribute(string name){
		this->name = name;
		notNull = true;
		len = 0;
		dec = 0;
	}
//	Attribute(string name, string type):
//		name(name), type(type), notNull(true){
//	}
	Attribute(string name, string type, string tablename):
		name(name), typeStr(type), tablename(tablename), notNull(true){

		len = 0;
		dec = 0;

		std::vector<std::string> tokens;
		std::regex re("\\w+");

		//start/end points of tokens in type
		std::sregex_token_iterator begin(type.begin(), type.end(), re), end;

		std::copy(begin, end, std::back_inserter(tokens));

		if(tokens[0] == "Integer")
			this->type = parserTypes::Tag::Integer;
		else if (tokens[0] == "Char")
			this->type = parserTypes::Tag::Char;
		else if (tokens[0] == "Varchar")
			this->type = parserTypes::Tag::Varchar;
		else if (tokens[0] == "Numeric")
			this->type = parserTypes::Tag::Numeric;
		else if (tokens[0] == "Timestamp")
			this->type = parserTypes::Tag::Timestamp;
		else
			throw;

		if(tokens.size()>1){
			this->len = atoi(tokens[1].c_str());
			tokens.size()==3?this->dec = atoi(tokens[2].c_str()): this->dec = ~0;
		}


		//			string str = type.substr(0,3);
		//			if(str == "Int")
		//				ptype = "int32_t";
		//			else if(str == "Var" || str == "Cha")
		//				ptype = "string";
		//			else if(str == "Num")
		//				ptype = "int64_t";
		//			else
		//				ptype = "uint64_t";
	}
	~Attribute(){ };

	bool operator< (const Attribute& attr) const {
		 if(type != attr.type )
			 return type < attr.type;
		 return  name < attr.name;
	}

	bool operator==(const Attribute& attr) const{
		return (name == attr.name) && (type == attr.type) && (len == attr.len) && (dec == attr.dec) && (tablename == attr.tablename);
	}
	bool operator!=(const Attribute& attr) const{
		return !((name == attr.name) && (type == attr.type) && (len == attr.len) && (dec == attr.dec) && (tablename == attr.tablename));
	}

	string toString(){
		return name+", "+type+", "+to_string(len)+", "+to_string(dec)+", "+tablename+"\n";
	}
};

#endif

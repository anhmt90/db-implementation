/*
 * Generator.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: anhmt90
 */
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <cctype>

#include <algorithm>
#include <chrono>
#include "../hpp/Schema.hpp"
#include "../hpp/Generator.hpp"
#include "../hpp/Parser.hpp"
#include "../hpp/Attribute.hpp"
#include "../hpp/Table.hpp"

using namespace std;

namespace keyword {
   const string Const = "const ";
   const string Template = "template ";
   const string Typename = "typename ";
   const string TypeDef = "typedef ";
   const string Include = "#include ";
   const string Map = "unordered_map";
   const string MultiMap = "unordered_multimap";
   const string Array = "vector ";
   const string Void = "void ";
   const string Struct = "struct ";

}

//namespace literal {
//   const char ParenthesisLeft = '(';
//   const char ParenthesisRight = ')';
//   const char Comma = ',';
//   const char ChevronLeft = '<';
//   const char ChevronRight = '>';
//   const string BraceLeft = "{\n";
//   const string BraceRight = "}\n";
//   const string Semicolon = ";\n";
//   const string Endline = "\n";
//}
//
//namespace varlist {
//   const string Tup = "tup ";
//   const string For = "i ";
//   const string ForEach = "t ";
//;
//
//}
using namespace keyword;
//using namespace literal;

string firstUpper(string s){
	s[0] = toupper(s[0]);
	return s;
}
string allUpper(string& s){
	string str;
	std::transform(s.begin(), s.end(), back_inserter(str), ::toupper);
	return str;
}

string buff;

void print(string str){
	buff += str;
	cout<< str;
}

void codeGenerator(Schema schema){
	vector<string> headers = {"<fstream>", "<sstream>", "<unordered_map>","<vector>","<string>", "<tuple>", "<utility>", "<algorithm>",
			"\"../hpp/Types.hpp\""};
	for(auto& h : headers){
		print(Include+" "+h+";");
	}
	print("using namespace std;");
	string str =
			"\ntemplate<typename Relation, typename PKey, typename Row>\n "
			"void _insert(Relation rel, PKey key, Row row){\n "
			"rel.insert(make_pair(key, row));\n}\n"

			"\ntemplate<typename Relation, typename PKey>\n"
			"bool _find(Relation rel, PKey key){\n"
			"return rel.find(key)!=rel.end();\n}\n"

			"\ntemplate<typename Relation, typename PKey, typename Row>\n"
			"Row* _select(Relation rel, PKey key){\n"
			"try{\n"
			"return &(rel.at(key));\n"
			"} catch(out_of_range& e){\n"
			"cerr << e.what() << \"\\n\";\n"
			"return nullptr;\n"
			"}\n}\n"

			"\ntemplate<typename Attribute, typename Value>\n"
			"void _update(Attribute *attr, Value val){\n"
			"*attr = val;\n}\n"

			"\ntemplate<typename Relation, typename PKey>\n"
			"void _delete(Relation rel, PKey key){\n"
			"rel.erease(key);\n}\n"
			;
	print(str);

	str = "vector<string> split(const string &s) {\n"
			"vector<string> splitted;\n"
			"stringstream ss(s);\n"
			"string item;\n"
			"while (std::getline(ss, item, '|')) {\n"
			"splitted.push_back(item);\n}\nreturn splitted;\n"
			"}\n";

	print(str);
//	str = "typedef tuple<Integer, Integer> tup_2Int;\n"
//			"typedef tuple<Integer, Integer, Integer> tup_3Int;\n"
//			"typedef tuple<Integer, Integer, Integer, Integer> tup_4Int;\n";

	//vector<Table> *rels = &schema.relations;

	//generating relations schema
	for(const auto& rel : schema.relations){
		size_t c_pk = rel.primaryKey.size();
		string relname = rel.name;
		auto& attr = rel.attributes;

		str = Struct+relname+"{\n";

		for(size_t i = (c_pk == attr.size()?0:c_pk); i<attr.size();i++)
			str += type(attr[i])+"\t"+attr[i].name+";\n";

		str+="\n"+relname+"(){ }\n";
		str+="\n"+relname+"(\n";

		for(size_t i = (c_pk == attr.size()?0:c_pk); i<attr.size();i++){
			str += type(attr[i])+" "+attr[i].name;
			str+=(i+1 < attr.size())?",\n":"\n):\n";
		}

		for(size_t i = (c_pk == attr.size()?0:c_pk); i<attr.size();i++){
			str += attr[i].name+"("+attr[i].name+")";
			str+=(i+1 < attr.size())?",":"{ }\n}\n";
		}

		if(c_pk==0){
			str+= "vector<"+relname+"> "+firstUpper(relname)+";\n";
		} else if(c_pk == 1){
			str+= "unordered_map<"+type(attr[0])+","+relname+"> "+firstUpper(relname)+";\n";
		} else {
			str+= "unordered_map<tuple<";
			for(unsigned i = 0; i < c_pk; i++){
				str+= type(attr[i]);
				if(i+1<c_pk){
					str+=",";
				}
			}
			str+= ">,"+relname+"> "+firstUpper(relname)+";\n";
		}
		str+="\n\n";
		print(str);
	}

	//generating import functions for *.tbl files
	for(const auto& rel : schema.relations){
		size_t c_pk = rel.primaryKey.size();
		string relname = rel.name;
		string tblname = firstUpper(relname);
		auto& attr = rel.attributes;


		str = Void+relname+"_import(ifstream& itbl) {\n"
				"string line;\n"
				"if (itbl.is_open()) {\n"
				"while (getline(itbl, line)) {\n"
				"vector<string> elm = split(line);\n";
		str+= relname+" row;\n";
		for(size_t i = 0 ; i<c_pk;i++){
			str+= type(attr[i])+" "+attr[i].name+" = "+attr[i].name+".castString("
					"elm["+to_string(i)+"].c_str(), elm["+to_string(i)+"].length());\n";
		}
		if(c_pk>1){
			str+= "auto tup = make_tuple(";
			for(size_t i = 0 ; i<c_pk;i++){
				str+=attr[i].name;
				str+=(i+1<c_pk)?",":");\n";
			}
		}

		string temp = (c_pk>1)?"tup":attr[0].name;
		str+="if("+tblname+".find(";
		str+= temp+") != "+tblname+".end())\n"
				"cerr << \"Duplicate PK \" <<"+temp+" << \" on table"+tblname+"! Entry inserting aborted!\\n\";\n"
						"else{\n";

		for(size_t i = c_pk; i<attr.size();i++){
			str+= "row."+attr[i].name+" = row."+attr[i].name+".castString("
					"elm["+to_string(i)+"].c_str(), elm["+to_string(i)+"].length());\n";
		}
		str+=(c_pk==0)?relname+".push_back(row);\n":(c_pk==1)?relname+".insert(make_pair("+attr[0].name+",row));\n":
				relname+".insert(make_pair(tup,row));\n";
		str+="}\n}\n}\n}\n\n";
		print(str);

	}

	//gernerating indexes
	for(const auto& index : schema.indexes){
		auto c_col = index.columns.size();
		string tblname = firstUpper(index.tableName);
		str = "unordered_multimap<";
		str+= (c_col==1)?type(index.columns[0])+", "+index.tableName+"*> ":"tuple<";
		if(c_col>1){
			for(unsigned i = 0; i < c_col; i++){
				str+= type(index.columns[i]);
				str += (i+1<c_col)?", ":">> ";
			}
			str+=index.tableName+"*> ";
		}
		str+= index.name+";\n ";
		str+= "for(auto& t: "+ tblname +"){\n"
				"auto tup = make_tuple(";

		Table *rel_ptr = nullptr;
		for(auto& rel : schema.relations){
			if(rel.name == index.tableName){
				rel_ptr = &rel;
				//break;
			}
		}

		for(unsigned i = 0; i < c_col; i++){
			bool is_pk = false;
			for(unsigned j = 0; j < rel_ptr->primaryKey.size(); j++){
				if(index.columns[i].name == rel_ptr->attributes[j].name){
					str+= "get<"+to_string(j)+">(t.first)";
					is_pk = true;
					//break;
				}
			}
			if(!is_pk){
				str+= "t.second."+index.columns[i].name;
			}
			str+= (i+1<c_col)?", ":");\n";
		}
		str+= index.name+".insert(make_pair(";
		str+= (c_col==1)?index.columns[0].name+", ":"tup, ";
		str+= "&t.second));\n}\n\n";

		print(str);
	}

}




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//void TableScan::consume(Operator* op) {
//
//}
//
//void TableScan::produce() {
//	cout << "for (unsigned i = 0; i < "+tbl.size+"; ++i) { \n";
//	consumer->consume(this);
//	cout<< "} \n";
//}
//void codeGenerator(Query query){
//
//	for(int i=0; i<query.relations.size();++i){
//		auto name = query.relations[i].name;
//		cout<<"auto "+name+" = tpcc->"+name<<"; \n";
//	}
//	cout<<"\n";
//
//	for(int i=0; i<query.joins.size();++i){
//
//	}
//}

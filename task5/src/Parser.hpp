#ifndef H_Parser_hpp
#define H_Parser_hpp

#include <iostream>
#include <exception>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

#include "Schema.hpp"
#include "Query.hpp"
#include "Attribute.hpp"
#include "Table.hpp"


class ParserError : std::exception {
   std::string msg;
   unsigned line;
   public:
   ParserError(unsigned line, const std::string& m) : msg(m), line(line) {}
   ~ParserError() throw() {}
   const char* what() const throw() {
	  return msg.c_str();
   }
};

struct SchemaParser {
   std::string fileName;
   std::ifstream in;
   enum class State : unsigned { Init, Create, Table, CreateTableBegin, CreateTableEnd, TableName,
	   Primary, Key, KeyListBegin, KeyName, KeyListEnd, AttributeName, AttributeTypeInt, AttributeTypeChar,
	   CharBegin, CharValue, CharEnd, AttributeTypeNumeric, NumericBegin, NumericValue1, NumericSeparator,
	   NumericValue2, NumericEnd, Not, Null, Separator, Semicolon,
	   AttributeTypeVarchar, VarcharBegin, VarcharEnd, VarcharValue, AttributeTypeTimestamp,
   	   Index, IndexName, On, ColumnListBegin, ColumnName, ColumnListEnd, TableName4Index};
   State state;
   SchemaParser(const std::string& fileName) : fileName(fileName), state(State::Init) {}
   ~SchemaParser() {};
   std::unique_ptr<Schema> parse();

   private:
   void nextToken(unsigned line, const std::string& token, Schema& s);
};

struct QueryParser {
   std::string fileName;
   std::ifstream in;
   enum class State : unsigned { Init, Select, SelectEnd, QueryEnd, AttributeName,
	   From, FromEnd, RelationName, Where, EqualsSign, LeftOfEqualsSign, RightOfEqualsSign};
   State state;
//   QueryParser() { };
   QueryParser() : state(State::Init) {}
   ~QueryParser() {};
   std::unique_ptr<Query> parseFile(const string& fileName);
   std::unique_ptr<Query> parseCommand();

   private:
   void nextToken(unsigned line, const std::string& token, Query& s);
};

#endif

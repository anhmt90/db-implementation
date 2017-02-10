#include "../hpp/Parser.hpp"
#include "../hpp/TPCC_Schema.hpp"
//#include "..Types.hpp"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <cstdlib>

using namespace std;



namespace keyword {
const string Primary = "primary";
const string Key = "key";
const string Create = "create";
const string Table = "table";
const string Integer = "integer";
const string Numeric = "numeric";
const string Not = "not";
const string Null = "null";
const string Char = "char";
const string Varchar = "varchar";
const string Timestamp = "timestamp";
const string Index = "index";
const string On = "on";
const string Select = "select";
const string From = "from";
const string Where = "where";
const string And = "and";
}

namespace literal {
const char ParenthesisLeft = '(';
const char ParenthesisRight = ')';
const char Comma = ',';
const char Semicolon = ';';
const char Asterisk = '*';
const char NewLine = '\n';
const char EqualsSign = '=';
}

/*
 * Select clause will be stored at first but will be checked at last
 * because this check need the data of From-clause and Where-clause
 */
void checkSelect(unsigned line, Query& query){
	size_t proj_size = query.projections.size();
	if(proj_size == 0)
		throw ParserError(line, "SELECT clause is empty!");

	if(query.projections[0].name == "*")
		return;

	for(auto& pro : query.projections){
		bool found = false;
		for(auto& rel : query.relations){
			for(auto& attr : rel.attributes){
				if(pro.name == attr.name){
					pro = attr;
					found = true;

					break;
				}
			}
		}
		if(!found)
			throw ParserError(line,"Column '"+pro.name+"' in SELECT clause does not exist!");
	}

}

std::unique_ptr<Schema> SchemaParser::parse() {
	string token;
	unsigned line=1;
	unique_ptr<Schema> s(new Schema());
	in.open(fileName.c_str());

	if (!in.is_open())
		throw ParserError(line, "cannot open file '"+fileName+"'");

	while (in >> token) {
		string::size_type pos;
		string::size_type prevPos = 0;

		while ((pos = token.find_first_of(",)(;", prevPos)) != string::npos) {
			nextToken(line, token.substr(prevPos, pos-prevPos), *s);
			nextToken(line, token.substr(pos,1), *s);
			prevPos=pos+1;
		}
		nextToken(line, token.substr(prevPos), *s);
		if (token.find("\n")!=std::string::npos)
			++line;
	}
	in.close();
	return move(s);
}

std::unique_ptr<Query> QueryParser::parseFile(const string& fileName) {
	string token;
	unsigned line=1;
	unique_ptr<Query> q(new Query());
	in.open(fileName.c_str());

	if (!in.is_open())
		throw ParserError(line, "cannot open file '"+fileName+"'");

	while (in >> token) {
		string::size_type pos;
		string::size_type prevPos = 0;

		while ((pos = token.find_first_of(",)(;", prevPos)) != string::npos) {
			nextToken(line, token.substr(prevPos, pos-prevPos), *q);
			nextToken(line, token.substr(pos,1), *q);
			prevPos=pos+1;
		}
		nextToken(line, token.substr(prevPos), *q);
		if (token.find("\n")!=std::string::npos)
			++line;
	}
	in.close();
	checkSelect(line, *q);
	return move(q);
}

std::unique_ptr<Query> QueryParser::parseCommand() {
	string token;
	unsigned line=0;
	unique_ptr<Query> q(new Query());

	string input;
	getline(cin, input);

	//Checking for command exit
//	string check_exit;
//	std::transform(input.begin(), input.end(), back_inserter(check_exit), ::tolower);
	if(input == "exit")
		return nullptr;

	stringstream ss(input);
	while (ss >> token) {
		string::size_type pos;
		string::size_type prevPos = 0;

		while ((pos = token.find_first_of(",)(;", prevPos)) != string::npos) {
			nextToken(line, token.substr(prevPos, pos-prevPos), *q);
			nextToken(line, token.substr(pos,1), *q);
			prevPos=pos+1;
		}
		nextToken(line, token.substr(prevPos), *q);
		if (token.find("\n")!=std::string::npos)
			return move(q);
	}
	if(token!="exit")
		checkSelect(line, *q);
	return move(q);
}

static bool isIdentifier(const string& str) {
	if (
			//For Schema
			str==keyword::Primary ||
			str==keyword::Key ||
			str==keyword::Table ||
			str==keyword::Create ||
			str==keyword::Integer ||
			str==keyword::Numeric ||
			str==keyword::Not ||
			str==keyword::Null ||
			str==keyword::Char ||
			str==keyword::Varchar ||
			str==keyword::Timestamp ||
			str==keyword::Index ||
			str==keyword::On ||

			//For Query
			str==keyword::Select ||
			str==keyword::From ||
			str==keyword::Where ||
			str==keyword::And
	)
		return false;
	return str.find_first_not_of("abcdefghijklmnopqrstuvwxyz_\"1234567890") == string::npos;
}

static bool isInt(const string& str) {
	return str.find_first_not_of("0123456789") == string::npos;
}

void SchemaParser::nextToken(unsigned line, const string& token, Schema& schema) {
	if (getenv("DEBUG"))
		std::cerr << line << ": " << token << endl;
	if (token.empty())
		return;
	string tok;
	std::transform(token.begin(), token.end(), back_inserter(tok), ::tolower);
	switch(state) {
	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Semicolon: /* fallthrough */
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Init:
		if (tok==keyword::Create)
			state=State::Create;
		else
			throw ParserError(line, "Expected 'CREATE', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Create:
		if (tok==keyword::Table)
			state=State::Table;
		else if (tok==keyword::Index)
			state=State::Index;
		else
			throw ParserError(line, "Expected 'TABLE', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Table:
		if (isIdentifier(tok)) {
			auto& rel = schema.relations;
			NamePredicate<Table> p(token);
			if(find_if(rel.begin(), rel.end(), p) != rel.end()){
				throw ParserError(line, "There is already another Table named '"+token+"'.");
			}
//			std::replace( token.begin(), token.end(), '"', '');
			string tablename = token;
			tablename.erase (std::remove(tablename.begin(), tablename.end(), '"'), tablename.end()); //remove '"' from tablename
			schema.relations.push_back(Table(tablename));
			state=State::TableName;
		} else {
			throw ParserError(line, "Expected TableName, found '"+token+"'");
		}
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::TableName:
		if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
			state=State::CreateTableBegin;
		else
			throw ParserError(line, "Expected '(', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Separator: /* fallthrough */
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::CreateTableBegin:
		if (tok.size()==1 && tok[0]==literal::ParenthesisRight) {
			state=State::CreateTableEnd;
		} else if (tok==keyword::Primary) {
			state=State::Primary;
		} else if (isIdentifier(tok)) {
			schema.relations.back().attributes.push_back(Attribute());
			schema.relations.back().attributes.back().name = token;
			schema.relations.back().attributes.back().tablename = schema.relations.back().name;
			state=State::AttributeName;
		} else {
			throw ParserError(line, "Expected attribute definition, primary key definition or ')', found '"+token+"'");
		}
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::CreateTableEnd:
		if (tok.size()==1 && tok[0]==literal::Semicolon)
			state=State::Semicolon;
		else
			throw ParserError(line, "Expected ';', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Primary:
		if (tok==keyword::Key)
			state=State::Key;
		else
			throw ParserError(line, "Expected 'KEY', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Key:
		if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
			state=State::KeyListBegin;
		else
			throw ParserError(line, "Expected list of key attributes, found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::KeyListBegin:
		if (isIdentifier(tok)) {
			const auto& attributes = schema.relations.back().attributes;
			NamePredicate<Attribute> p(token);
			auto it = find_if(attributes.begin(), attributes.end(), p);
			if (it == attributes.end())
				throw ParserError(line, "'"+token+"' is not an attribute of '"+schema.relations.back().name+"'");
			schema.relations.back().primaryKey.push_back(std::distance(attributes.begin(), it));
			state=State::KeyName;
		} else {
			throw ParserError(line, "Expected key attribute, found '"+token+"'");
		}
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::KeyName:
		if (tok.size()==1 && tok[0] == literal::Comma)
			state=State::KeyListBegin;
		else if (tok.size()==1 && tok[0] == literal::ParenthesisRight)
			state=State::KeyListEnd;
		else
			throw ParserError(line, "Expected ',' or ')', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::KeyListEnd:
		if (tok.size()==1 && tok[0] == literal::Comma)
			state=State::Separator;
		else if (tok.size()==1 && tok[0] == literal::ParenthesisRight)
			state=State::CreateTableEnd;
		else
			throw ParserError(line, "Expected ',' or ')', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::AttributeName:
		if (tok==keyword::Integer) {
			schema.relations.back().attributes.back().type=parserTypes::Tag::Integer;
			state=State::AttributeTypeInt;
		} else if (tok==keyword::Char) {
			schema.relations.back().attributes.back().type=parserTypes::Tag::Char;
			state=State::AttributeTypeChar;
		} else if (tok==keyword::Numeric) {
			schema.relations.back().attributes.back().type=parserTypes::Tag::Numeric;
			state=State::AttributeTypeNumeric;
		} else if (tok==keyword::Varchar) {
			schema.relations.back().attributes.back().type=parserTypes::Tag::Varchar;
			state=State::AttributeTypeVarchar;
		} else if (tok==keyword::Timestamp) {
			schema.relations.back().attributes.back().type=parserTypes::Tag::Timestamp;
			state=State::AttributeTypeTimestamp;
		}
		else throw ParserError(line, "Expected type after attribute name, found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	case State::AttributeTypeChar:
		if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
			state=State::CharBegin;
		else
			throw ParserError(line, "Expected '(' after 'CHAR', found'"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::CharBegin:
		if (isInt(tok)) {
			schema.relations.back().attributes.back().len=std::atoi(tok.c_str());
			state=State::CharValue;
		} else {
			throw ParserError(line, "Expected integer after 'CHAR(', found'"+token+"'");
		}
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::CharValue:
		if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
			state=State::CharEnd;
		else
			throw ParserError(line, "Expected ')' after length of CHAR, found'"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::CharEnd: /* fallthrough */
		if (tok.size()==1 && tok[0]==literal::Comma)
			state=State::Separator;
		else if (tok==keyword::Not)
			state=State::Not;
		else if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
			state=State::CreateTableEnd;
		else throw ParserError(line, "Expected ',' or 'NOT NULL' after attribute type, found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	case State::AttributeTypeVarchar:
		if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
			state=State::VarcharBegin;
		else
			throw ParserError(line, "Expected '(' after 'VARCHAR', found'"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::VarcharBegin:
		if (isInt(tok)) {
			schema.relations.back().attributes.back().len=std::atoi(tok.c_str());
			state=State::VarcharValue;
		} else {
			throw ParserError(line, "Expected integer after 'VARCHAR(', found'"+token+"'");
		}
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::VarcharValue:
		if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
			state=State::VarcharEnd;
		else
			throw ParserError(line, "Expected ')' after length of VARCHAR, found'"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::VarcharEnd: /* fallthrough */
		if (tok.size()==1 && tok[0]==literal::Comma)
			state=State::Separator;
		else if (tok==keyword::Not)
			state=State::Not;
		else if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
			state=State::CreateTableEnd;
		else throw ParserError(line, "Expected ',' or 'NOT NULL' after attribute type, found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


	case State::AttributeTypeNumeric:
		if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
			state=State::NumericBegin;
		else
			throw ParserError(line, "Expected '(' after 'NUMERIC', found'"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::NumericBegin:
		if (isInt(tok)) {
			schema.relations.back().attributes.back().len=std::atoi(tok.c_str());
			state=State::NumericValue1;
		} else {
			throw ParserError(line, "Expected integer after 'NUMERIC(', found'"+token+"'");
		}
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::NumericValue1:
		if (tok.size()==1 && tok[0]==literal::Comma)
			state=State::NumericSeparator;
		else
			throw ParserError(line, "Expected ',' after first length of NUMERIC, found'"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::NumericValue2:
		if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
			state=State::NumericEnd;
		else
			throw ParserError(line, "Expected ')' after second length of NUMERIC, found'"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::NumericSeparator:
		if (isInt(tok)) {
			schema.relations.back().attributes.back().dec=std::atoi(tok.c_str());
			state=State::NumericValue2;
		} else {
			throw ParserError(line, "Expected second length for NUMERIC type, found'"+token+"'");
		}
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::NumericEnd: /* fallthrough */
		if (tok.size()==1 && tok[0]==literal::Comma)
			state=State::Separator;
		else if (tok==keyword::Not)
			state=State::Not;
		else if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
			state=State::CreateTableEnd;
		else throw ParserError(line, "Expected ',' or 'NOT NULL' after attribute type, found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::AttributeTypeInt:
		if (tok.size()==1 && tok[0]==literal::Comma)
			state=State::Separator;
		else if (tok==keyword::Not)
			state=State::Not;
		else if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
			state=State::CreateTableEnd;
		else throw ParserError(line, "Expected ',' or 'NOT NULL' after attribute type, found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Not:
		if (tok==keyword::Null) {
			schema.relations.back().attributes.back().notNull=true;
			state=State::Null;
		}
		else throw ParserError(line, "Expected 'NULL' after 'NOT' name, found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Null:
		if (tok.size()==1 && tok[0]==literal::Comma)
			state=State::Separator;
		else if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
			state=State::CreateTableEnd;
		else throw ParserError(line, "Expected ',' or ')' after attribute definition, found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::AttributeTypeTimestamp:
		if (tok.size()==1 && tok[0]==literal::Comma)
			state=State::Separator;
		else if (tok==keyword::Not)
			state=State::Not;
		else if (tok.size()==1 && tok[0]==literal::ParenthesisRight)
			state=State::CreateTableEnd;
		else throw ParserError(line, "Expected ',' or 'NOT NULL' after attribute type, found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	case State::Index:
		if (isIdentifier(tok)) {
			schema.indexes.push_back(Schema::Index(token));
			state=State::IndexName;
		} else {
			throw ParserError(line, "Expected IndexName, found '"+token+"'");
		}
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::IndexName:
		if (tok==keyword::On)
			state=State::On;
		else
			throw ParserError(line, "Expected 'ON', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::On:
		if (isIdentifier(tok)) {
			auto& rel = schema.relations;
			string tablename = token;
			tablename.erase (std::remove(tablename.begin(), tablename.end(), '"'), tablename.end()); //remove '"' from tablename
			NamePredicate<Table> p(tablename);
			if(find_if(rel.begin(), rel.end(), p) == rel.end()){
				schema.indexes.pop_back();
				throw ParserError(line, "'"+token+"' is not a valid table '");
			}

			struct IndexNamePredicate {
				const string& name;
				const string& tableName;
				IndexNamePredicate(const string& name, const string& tableName)
				: name(name), tableName(tableName) {}
				bool operator()(const Schema::Index& index) const {
					return (index.name == name) && (index.tableName == tableName);
				}
			};
			auto& indexes = schema.indexes;
			auto& indexName = indexes.back().name;
			IndexNamePredicate p1(indexName, token);
			if(find_if(indexes.begin(), indexes.end(), p1) != indexes.end()){
				schema.indexes.pop_back();
				throw ParserError(line, "There is already an index named '"+indexName+"' on table"+token+"'");
			}

			schema.indexes.back().tableName = tablename;
			state=State::TableName4Index;
		} else {
			throw ParserError(line, "Expected TableName, found '"+token+"'");
		}
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::TableName4Index:
		if (tok.size()==1 && tok[0]==literal::ParenthesisLeft)
			state=State::ColumnListBegin;
		else
			throw ParserError(line, "Expected '(', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::ColumnListBegin:
		if (isIdentifier(tok)) {
			const auto& rel = schema.relations;
			NamePredicate<Table> p(schema.indexes.back().tableName);
			auto itr = find_if(rel.begin(), rel.end(), p);

			//Take out the attribute list of a specific table that 'itr' is pointing to
			auto& attr = itr->attributes;
			NamePredicate<Attribute> p1(token);
			auto attr_itr = find_if(attr.begin(), attr.end(), p1);

			if ( attr_itr == attr.end())
				throw ParserError(line, "'"+token+"' is not an attribute of '"+itr->name+"'");

			schema.indexes.back().columns.push_back(*attr_itr);
			state=State::ColumnName;
		} else {
			throw ParserError(line, "Expected key index, found '"+token+"'");
		}
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::ColumnName:
		if (tok.size()==1 && tok[0] == literal::Comma)
			state=State::ColumnListBegin;
		else if (tok.size()==1 && tok[0] == literal::ParenthesisRight)
			state=State::ColumnListEnd;
		else
			throw ParserError(line, "Expected ',' or ')', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::ColumnListEnd:
		if (tok.size()==1 && tok[0] == literal::Semicolon)
			state=State::Semicolon;
		else
			throw ParserError(line, "Expected ';' or ')', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	default:
		throw;
	}
}

/*---------------------------------------------------------------------------------------------*/
Attribute null = Attribute("null");
Attribute attrLeft = null;
Attribute attrRight = null;

Query::Constant constant;

Attribute parseAttribute(const string& token, Query& query){
	for(auto& rel : query.relations){
		for(auto& attr : rel.attributes){
			if(token == attr.name){
				return attr;
			}
		}
	}
	return null;
}

void QueryParser::nextToken(unsigned line, const string& token, Query& query){
	if (token.empty())
		return;
	string tok;
	std::transform(token.begin(), token.end(), back_inserter(tok), ::tolower);
	switch(state) {
//	case State::Separator:
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Init:
		if (tok==keyword::Select)
			state=State::Select;
		else
			throw ParserError(line, "Expected 'SELECT', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Select:
		if (tok.size()==1 && tok[0]==literal::Asterisk){
			query.projections.push_back(Attribute("*"));
			state=State::SelectEnd;
		}

		else if(isIdentifier(tok)){
			query.projections.push_back(Attribute(token));
			state = State::AttributeName;
		}

		else
			throw ParserError(line, "Syntax error after keyword 'SELECT'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::AttributeName:
		if (tok.size()==1 && tok[0]==literal::Comma)
			state = State::Select;
//		else if(tok.size()==1 && tok[0] == literal::NewLine)
//			state = State::SelectEnd;
		else if(tok==keyword::From)
			state=State::From;
		else
			throw ParserError(line, "Syntax error in 'SELECT' clause at '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::From:
		//Check if this table name exists
		for(unsigned j = 0; j<query.relations.size(); ++j){
			if(query.relations[j].name == token)
				throw ParserError(line, "Duplicate relation names in 'FROM' clause at '"+token+"'");
		}

		for(unsigned j = 0; j<tables.size(); ++j){
			if(tables[j].name == token){
				query.relations.push_back(tables[j]);
				break;
			}
			if(j+1==tables.size())
				throw ParserError(line, "Relation '"+token+"' does not exist in schema.");
		}
		state=State::RelationName;
		break;

		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::RelationName:
		if (tok.size()==1 && tok[0] == literal::Comma)
			state=State::From;
//		else if(tok.size()==1 && tok[0] == literal::NewLine)
//			state=State::FromEnd;
		else if (tok==keyword::Where)
			state=State::Where;
		else
			throw ParserError(line, "Syntax error in 'FROM' clause at '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::Where:
		attrLeft = parseAttribute(token, query);
		if(attrLeft.name == "null")
			throw ParserError(line, "Attribute '"+token+"' does not exist in any table.");
		state = State::LeftOfEqualsSign;

		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::LeftOfEqualsSign:
		if (tok.size()==1 && tok[0] == literal::EqualsSign)
			state = State::EqualsSign;
		else
			throw ParserError(line, "Expected '=', found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::EqualsSign:
		attrRight = parseAttribute(token, query);
		if(attrRight.name == "null"){	///is a selection
			//remove quotation marks from value
			string value = token;
			value.erase (std::remove(value.begin(), value.end(), '\''), value.end());
			constant.value = value;
			constant.type = attrLeft.type;
			constant.typeStr = attrLeft.typeStr;
			query.selections.push_back(make_pair(attrLeft, constant));
		}
		else  ///is a join
			query.joins.push_back(make_pair(attrLeft, attrRight));


		attrLeft = null;
		attrRight = null;

		state = State::RightOfEqualsSign;
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	case State::RightOfEqualsSign:
		if (tok == keyword::And){
			state = State::Where;
		}
//		else if (tok[0] == literal::NewLine)
//			state = State::WhereEnd;
		else
			throw ParserError(line, "Expected 'AND' or End of Query, but found '"+token+"'");
		break;
		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//	case State::WhereEnd:
//	{
//		size_t proj_size = query.projections.size();
//		if(proj_size == 0)
//			throw ParserError(line, "SELECT clause is empty!");
//
//		for(unsigned k = 0; k<proj_size; ++k){
//			unsigned i = 0;
//			size_t rel_size = query.relations.size();
//			while(i<rel_size){
//				bool found = false;
//				vector<Attribute> attrs = query.relations[i].attributes;
//				for(unsigned j = 0, attrs_size = attrs.size(); j < attrs_size; ++j){
//					if(token == attrs[j].name){
//						found = true;
//						break;
//					}
//				}
//				if(!found)
//					throw ParserError(line,"Column '"+query.projections[k].name+"' in SELECT clause does not exist!");
//				++i;
//			}
//		}
//	}
//	state = State::Init;
//	break;
//		/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	default:
		cerr<<"Error!";
		throw;
		break;
	}
}


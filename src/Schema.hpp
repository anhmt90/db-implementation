#ifndef H_SCHEMA_HPP
#define H_SCHEMA_HPP

#include <vector>
#include <string>
#include "Types.hpp"
#include "Attribute.hpp"
#include "Table.hpp"

struct Schema {
//   struct Relation {
//      struct Attribute {
//         std::string name;
//         parserTypes::Tag type;
//         unsigned len;
//         unsigned dec;
//         bool notNull;
//         Attribute() : len(~0), notNull(true) {}
//      };
//
//      std::string name;
//      std::vector<Schema::Relation::Attribute> attributes;
//      std::vector<unsigned> primaryKey;
//
//      Relation(const std::string& name) : name(name) {}
//   };

   struct Index {
	   std::string name;
	   std::string tableName;
	   std::vector<Attribute> columns;
	   Index(const std::string& name) : name(name) {}
   };


   std::vector<Table> relations;
   std::vector<Schema::Index> indexes;
   std::string toString() const;
};

std::string type(const Attribute&);
#endif

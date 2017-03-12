#include "Schema.hpp"

#include <sstream>

std::string type(const Attribute& attr) {
   parserTypes::Tag type = attr.type;
   switch(type) {
      case parserTypes::Tag::Integer:
         return "Integer";

      case parserTypes::Tag::Numeric: {
         std::stringstream ss;
         ss << "Numeric(" << attr.len << ", " << attr.dec << ")";
         return ss.str();
      }
      case parserTypes::Tag::Char: {
         std::stringstream ss;
         ss << "Char(" << attr.len << ")";
         return ss.str();
      }
      case parserTypes::Tag::Varchar: {
    	  std::stringstream ss;
    	  ss << "Varchar(" << attr.len << ")";
    	  return ss.str();
      }
      case parserTypes::Tag::Timestamp: {
    	  std::stringstream ss;
    	  return "Timestamp";
      }
   }
   throw;
}

std::string Schema::toString() const {
   std::stringstream out;
   for (const Table& rel : relations) {
      out << rel.name << std::endl;
      out << "\tPrimary Key:";
      for (unsigned keyId : rel.primaryKey)
         out << ' ' << rel.attributes[keyId].name;
      out << std::endl;
      for (const auto& attr : rel.attributes)
    	  out << '\t' << attr.name << '\t' << type(attr) << (attr.notNull ? " not null " : " ") << attr.tablename << std::endl;
//         out << '\t' << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << std::endl;
   }

   for (const Schema::Index& index : indexes) {
	   out << index.name << " on table "<< index.tableName<< std::endl;
	   out << "\tIndex Key:";
	   for (const auto& attr : index.columns)
		   out << ' ' << attr.name;
	   out << std::endl;
	   for (const auto& attr : index.columns)
		   out << '\t' << attr.name << '\t' << type(attr) << (attr.notNull ? " not null" : "") << std::endl;
   }
   return out.str();
}

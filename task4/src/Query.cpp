/*
 * Query.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: anhmt90
 */
#include "Query.hpp"
#include <string>
#include <vector>
#include <sstream>

using namespace std;


std::string Query::toString() const {
   std::stringstream out;
   out << "From tables:"<< std::endl;
   for (const Table& rel : relations) {
      out << '\t' << rel.name << std::endl;
   }

   out << "Selections:\n";
   for (const auto& s : selections)
	   out << '\t'<< get<0>(s).name << " = "<< get<1>(s).value << "\n";

   out << "Joins:\n";
   for (const auto& j : joins)
	   out << '\t'<< get<0>(j).name << " = "<< get<1>(j).name << "\n";

   out << "Projections:\n";
   for (const auto& attr : projections) {
	   out << '\t'<< attr.name << "\n";
   }
   return out.str();
}

#ifndef H_Table
#define H_Table
//---------------------------------------------------------------------------

#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>

#include "Attribute.hpp"
#include "Types.hpp"
//---------------------------------------------------------------------------
/// A database table
using namespace std;

//bool sortByType	(const Attribute& lhs, const Attribute& rhs){
//	return lhs.type < rhs.type;
//}
//bool sortByType (const Attribute& lhs, const Attribute& rhs);

class Table
{
public:

   /// Table name
   string name;
   /// The attributes
   vector<Attribute> attributes;
   /// Indexes of the pkeys in attributes
   std::vector<unsigned> primaryKey;
   /// Constructors
   Table(){};
   Table(const string name):name(name){};
   /// Destructor
   ~Table(){};

//   virtual insert()
   bool operator==(const Table& table) const{
	   return (name == table.name) && (attributes == table.attributes)  && (primaryKey == table.primaryKey);
   }
   bool operator!=(const Table& table) const{
	   return !((name == table.name) && (attributes == table.attributes) && (primaryKey == table.primaryKey));
   }

};

extern vector<Table> tables;
//---------------------------------------------------------------------------
#endif


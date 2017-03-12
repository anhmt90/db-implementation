#include <dlfcn.h>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Attribute.hpp"
#include "Operator.hpp"
#include "Parser.hpp"
#include "Query.hpp"
#include "Schema.hpp"

using namespace std;
using namespace std::chrono;


/*=========================================================================================================*/
int main() {
	SchemaParser sp("schema.sql");
	QueryParser qp;

	///Parse the Schema
	std::unique_ptr<Schema> schema = sp.parse();
//	std::cout << schema->toString() << std::endl; // uncomment to print the parsed data of the schema

	///Parse the Query
	std::unique_ptr<Query> query = qp.parseFile("_task4_query.sql");
//	std::cout << query->toString() << std::endl; // uncomment to print the parsed data of the query


	///Manually build the algebra tree
	auto start=high_resolution_clock::now();
	auto customer = query->relations[0];
	auto order = query->relations[1];
	auto orderline = query->relations[2];

	TableScan* scan_customer = new TableScan(customer);
	TableScan* scan_order = new TableScan(order);
	TableScan* scan_orderline = new TableScan(orderline);


	Selection* selection = new Selection(scan_customer);
	for(auto& s : query->selections){
		selection->predicates.push_back(make_pair(s.first, s.second));
	}

	HashJoin* c_hj_o = new HashJoin(selection, scan_order);
	for(auto& j : query->joins){
		if(j.first.tablename == "customer" && j.second.tablename == "order")
			c_hj_o->equijoins.push_back(make_pair(j.first, j.second));
		else if(j.second.tablename == "customer" && j.first.tablename == "order")
			c_hj_o->equijoins.push_back(make_pair(j.second, j.first));
	}
	c_hj_o->hashTableName ="hashJoin1";

	HashJoin* c_o_hj_ol = new HashJoin(c_hj_o, scan_orderline);
	for(auto& j : query->joins){
		if(j.first.tablename == "order" && j.second.tablename == "orderline")
			c_o_hj_ol->equijoins.push_back(make_pair(j.first, j.second));
		else if(j.second.tablename == "orderline" && j.first.tablename == "order")
			c_o_hj_ol->equijoins.push_back(make_pair(j.second, j.first));
	}
	c_o_hj_ol->hashTableName = "hashJoin2";

	Print* print = new Print(c_o_hj_ol);
	for(auto& proj : query->projections){
		print->required.push_back(proj);
	}


	cout << endl;
	ofstream ofs("TranslatedQuery.cpp", ofstream::out);
	ofs << "#include \"./src/TPCC_Schema.hpp\" \n";
	ofs << "#include \"./src/Operator.hpp\" \n\n";
	ofs << "using namespace std; \n\n";
	ofs << "extern \"C\" void _run(){ \n";
	ofs << "\t std::cout << ";
	for(unsigned i = 0, _size = query->projections.size(); i< _size; ++i){
		ofs<<"\"" << query->projections[i].name <<"\"";
		if(i+1 < _size)
			ofs << " <<\"\\t\\t \" <<";
	}
	ofs << "<< endl; \n";
	ofs << "\t std::cout << \"_______________________________________________________________________________\"";
	ofs << "<< endl; \n";
	ofs.close();
	ofs.clear();

	print->produce();  /*Start printing the algebra tree here*/

	ofs.open("TranslatedQuery.cpp", ofstream::app);
	ofs<<"}";
	ofs.close();
	ofs.clear();

	system("./run_query.sh");
	void* handle=dlopen("./TranslatedQuery.so",RTLD_NOW);
	if (!handle) {
		std::cerr << "dlopen-error: " << dlerror() << std::endl;
		exit(1);
	}
	auto exe =reinterpret_cast<void (*)()>(dlsym(handle, "_run"));
	if (!exe) {
		std::cerr << "dlsym-error: " << dlerror() << std::endl;
		exit(1);
	}
	auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();

	auto exeStart=high_resolution_clock::now();
	exe();
	auto exeEnd = duration_cast<duration<double>>(high_resolution_clock::now()-exeStart).count();
	cout << fixed << "\nQuery compiled in " << end << "s\n";
	cout << fixed << "Query executed in " << exeEnd << "s\n";
	if (dlclose(handle)) {
		std::cerr << "dlclose-error: " << dlerror() << std::endl;
		exit(1);
	}
}

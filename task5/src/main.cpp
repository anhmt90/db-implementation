#include <iostream>
#include <memory>
#include <algorithm>
#include <climits>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <atomic>
#include <dlfcn.h>


#include "Parser.hpp"
#include "Schema.hpp"
#include "TPCC_Schema.hpp"
#include "Types.hpp"
#include "Table.hpp"
#include "Attribute.hpp"
#include "SemanticAnalyser.hpp"

using namespace std;

/*=========================================================================================================*/
int main(int argc, char* argv[]) {
	SchemaParser sp("schema.sql");
	QueryParser qp;
	try {
		// parse the schema
		std::unique_ptr<Schema> schema = sp.parse();
		//std::cout << schema->toString() << std::endl; // uncomment to see the parsed data of the schema
		SemanticAnalyser semanal(move(schema));
		cout<<"Type 'exit' to quit.\n";

		while(true){
			QueryParser qp;
			cout<<"\nQUERY> ";
			std::unique_ptr<Query> query = qp.parseCommand();
			if(!query)
				exit(1);
			//std::cout << query->toString() << std::endl; // uncomment to see the parsed data of the query

			auto start=high_resolution_clock::now();

			semanal.setQuery(move(query));
			semanal.analyse();
			semanal.build();
			//				if(system("./run_query.sh") != 0)
			//					continue;
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
			/*----------------------------------------------------------------------------------*/
			auto end = duration_cast<duration<double>>(high_resolution_clock::now()-start).count();
			/*----------------------------------------------------------------------------------*/
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

	}catch (...){

	}
}

g++ -std=c++11 -g -O3 -Wall -fPIC -rdynamic TranslatedQuery.cpp -shared -o TranslatedQuery.so

#/usr/bin/g++ -obin/main -g -std=c++11 -Wall -Wextra -Isrc -lrt -Wl,--no-as-needed,-E -ldl   bin/main.o bin/Attribute.o bin/Generator.o bin/Table.o bin/Parser.o bin/Query.o bin/Schema.o bin/TPCC_Schema.o bin/Types.o bin/SemanticAnalyser.o bin/operators/Operator.o

#g++ -std=c++11 -g -O3 -Wall -fPIC -rdynamic TranslatedQuery.cpp -shared -o TranslatedQuery.so
/*
 * SemanticAnalyser.cpp
 *
 *  Created on: Nov 23, 2016
 *      Author: anhmt90
 */

#include "SemanticAnalyser.hpp"

void SemanticAnalyser::analyse(){
	/*
	 * check if tables in the query exists
	 */
//	for(auto& qrel : query->relations){
//		auto itr = find(schema->relations.begin(), schema->relations.end(), qrel);
//		if(itr == schema->relations.end())
//			throw SemanticError("Table "+qrel.name+" does not exists");
//	}
//
//	for(auto& sel : query->selections){
//		NamePredicate<Table> np(sel.first.tablename);
//		auto tbl = find_if(schema->relations.begin(), schema->relations.end(), np);
//		auto itr = find(tbl->attributes.begin(), tbl->attributes.end(), sel.first);
//		if(itr == tbl->attributes.end())
//			throw SemanticError("Column "+sel.first.name+" does not exists");
//	}
//
//	for(auto& pro : query->projections){
//		if(pro != "*"){
//			for(auto& rel : query->relations){
//				NamePredicate<Attribute> np(pro);
//				auto tbl = find_if(rel.attributes.begin(), schema->relations.end(), np);
//				auto itr = find(tbl->attributes.begin(), tbl->attributes.end(), sel.first);
//				if(itr == tbl->attributes.end())
//					throw SemanticError("Column "+sel.first.name+" does not exists");
//			}
//		}
//
//	}

	/*
	 * check if all tables in From clause have at least 1 equi-join in Where clause
	 *
	 */
	if(query->relations.size() > 1){
		vector<string> joinTables;
		for(auto& join : query->joins){
			joinTables.push_back(join.first.tablename);
			joinTables.push_back(join.second.tablename);
		}
		for(auto& rel : query->relations){
			auto itr = find(joinTables.begin(), joinTables.end(), rel.name);
			if(itr==joinTables.end())
				throw SemanticError("Table " + rel.name + " does not have any equi-join. This system doesn't support cross product!");
		}
	}

}

void SemanticAnalyser::build(){
	//	query = init(move(query));
	ofstream ofs;
	ofs.open("TranslatedQuery.cpp", ofstream::out);
	ofs << "#include \"./src/TPCC_Schema.hpp\" \n";
	ofs << "#include \"./src/Operator.hpp\" \n\n";
	ofs << "using namespace std; \n\n";
	ofs << "extern \"C\" void _run(){ \n";
	// output label
//
//	for(unsigned i = 0, req_size = required.size(); i< req_size; ++i){

	ofs << "\t std::cout << ";
	for(unsigned i = 0, _size = query->projections.size(); i< _size; ++i){
		ofs<<"\"" << query->projections[i].name <<"\"";
		if(i+1 < _size)
			ofs << " <<\"\\t\\t \" <<";
	}
	ofs << "<< endl; \n";
	ofs << "\t std::cout << \"______________________________________________________________________________________________________________________\"";
	ofs << "<< endl; \n";

	ofs.close();
	ofs.clear();
	vector<pair<string,TableScan*>> scans;
	vector<pair<string, Selection*>> sels;
	vector<pair<string, HashJoin*>> hjs;

	Operator* print_this;
	/*
	 * write TableScans and Selections
	 */
	vector<bool> haveSels;
	for(auto& rel : query->relations){		/*
		 * TableScans
		 */
		TableScan* scan_this = new TableScan(rel);
		scans.push_back(make_pair("scan_"+rel.name, scan_this));

		print_this = scan_this;
		/*
		 * Selections
		 * If the table has selection(s), initiate this selection right away
		 */
		bool hasSelection = false;
		for(auto& sel : query->selections){
			if(sel.first.tablename == rel.name && !hasSelection){
				hasSelection = true;
				Selection* select_this = new Selection(scan_this);
				select_this->predicates.push_back(make_pair(sel.first, sel.second));
				sels.push_back(make_pair("selection_"+rel.name, select_this));
				print_this = select_this;
			}
			else if (sel.first.tablename == rel.name && hasSelection){
				sels.back().second->predicates.push_back(make_pair(sel.first, sel.second));
				print_this = sels.back().second;
			}
		}
		haveSels.push_back(hasSelection);
	}

	/*
	 * HashJoins
	 */
	if(query->joins.size() > 0){
		string hjName;
		Operator* left;
		Operator* right;
		for(unsigned i = 0, rels_size = query->relations.size(); i<rels_size; ++i){
			/*
			 * For the first join
			 */
			if(i < 2){
				auto& rel1 = query->relations[i];
				if (haveSels[i]){
					for(auto& sel : sels){
						if(sel.second->tablename == rel1.name)
							left = sel.second;
					}
				} else {
					left = scans[i].second;
				}
				++i;
				auto& rel2 = query->relations[i];
				hjName = "hashJoin_"+to_string(i);

				if (haveSels[i]){
					for(auto& sel : sels){
						if(sel.second->tablename == rel2.name)
							right = sel.second;
					}
				} else {
					right = scans[i].second;
				}


				HashJoin* hash_this = new HashJoin(left, right);
				for(auto& join : query->joins){
					if((join.first.tablename == rel1.name && join.second.tablename == rel2.name)){
						hash_this->equijoins.push_back(make_pair(join.first, join.second));
					}
					else if ((join.first.tablename == rel2.name && join.second.tablename == rel1.name)){
						hash_this->equijoins.push_back(make_pair(join.second, join.first));
					}
				}

				hash_this->hashTableName = hjName;
				hjs.push_back(make_pair(hjName, hash_this));
			}
			/*
			 * For the second join and further joins
			 */
			else{
				hjName = "hashJoin_"+to_string(i);
				auto& rel = query->relations[i];

				if (haveSels[i]){
					for(auto& sel : sels){
						if(sel.second->tablename == rel.name)
							right = sel.second;
					}
				} else {
					right = scans[i].second;
				}

				HashJoin* hash_this = new HashJoin(hjs.back().second, right);
				for(auto& join : query->joins){
					/*
					 * The NamePredicate and find_if are to guarantee that we take into account
					 * only the equi-joins between tables on the left side (tables that are already scanned)
					 * and the table on the table right side of this HashJoin
					 */
					NamePredicate<Table> npFirst(join.first.tablename);
					NamePredicate<Table> npSecond(join.second.tablename);

					if((join.second.tablename == rel.name) && (find_if(query->relations.begin(), query->relations.begin()+i, npFirst) != query->relations.begin()+i)){
						hash_this->equijoins.push_back(make_pair(join.first, join.second));
					}
					else if ((join.first.tablename == rel.name) && (find_if(query->relations.begin(), query->relations.begin()+i, npSecond) != query->relations.begin()+i)){
						hash_this->equijoins.push_back(make_pair(join.second, join.first));
					}
				}
				hash_this->hashTableName = hjName;
				hjs.push_back(make_pair(hjName, hash_this));
			}
		}
		print_this = hjs.back().second;
	}


	/*
	 * Print/Projection
	 */
	Print* print = new Print(print_this);
	for(auto& proj : query->projections){
		print->required.push_back(Attribute(proj.name, proj.typeStr, proj.tablename));
	}
	print->produce();

	ofs.open("TranslatedQuery.cpp", ofstream::app);
	ofs<<"}";
}





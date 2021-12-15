The practical course comprises weekly assignments for the first 5 weeks and a [final project](#final_project) spanning the last 9 weeks. For more details, see https://db.in.tum.de/teaching/ws1617/imlab/?lang=en

# Weekly Tasks


### Task 1

1. Create two folders named "tbl" and "bin" in root folder and put the *.tbl-files into the "tbl" folder so that the program can import the data.
2. Import the project in Eclipse IDE as New Makefile project or use command " make && ./bin/main " to compile and run the program on linux shell.
3. main() function is in src/main.cpp.
4. The newOrder and the delivery transactions as well as the sumQuery is written as functions in main.cpp.
5. In edge cases, a transaction runs as follows:
	+ SELECT: if the required tuples donot exist, a transaction must abort because the variables from this SELECT-statement will be needed below.
	+ INSERT: if inserting an duplicate PK into a table, the transaction ignores this statement and keeps running 
	+ UPDATE and DELETE: if updating/deleting a non-existing tuple, the statement is ignored.
	+ For INSERT and DELETE, We can let the transaction abort in the edge cases by uncommenting the program lines that are commented next to the current running ones and commenting the current running lines for that statement. The program will run much more faster (better performance due to aborts).
6. The programm is built and tested by Eclipse IDE on Xubuntu 16.04 LTS.

### Task 2

1. Create two folders named "tbl" and "bin" in root folder and put the *.tbl-files into the "tbl" folder so that the program can import the data.
2. Import the project in Eclipse IDE as New Makefile project to run it or use command " make && ./bin/main " to compile and run the program on linux shell.
3. main() function is in src/main.cpp.
4. The generated code is in file './src/TPCC_Schema.hpp' and './src/TPCC_Schema.cpp', which are the running program files for the schema right after generated . 
5. Use the  variable _use_generated_code to toggle between generating code for the schema and run the mixed transaction oltp.
6. In edge cases, a transaction runs as follows:
	+ SELECT: if the required tuples donot exist, a transaction must abort because the variables from this SELECT-statement will be needed below.
	+ INSERT: if inserting an duplicate PK into a table, the transaction ignores this statement and keeps running 
	+ UPDATE and DELETE: if updating/deleting a non-existing tuple, the statement is ignored.
	+ For INSERT and DELETE, We can let the transaction abort in the edge cases by uncommenting the program lines that are commented next to the current running ones and commenting the current running lines for that statement. The program will run much more faster (better performance due to aborts).
7. The programm is built and tested on Eclipse IDE neon.1 on Xubuntu 16.04 LTS.

### Task 3

1. Create two folders named "tbl" and "bin" in root folder and put the *.tbl-files into the "tbl" folder so that the program can import the data.
2. Import the project in Eclipse IDE as New Makefile project or use command " make && ./bin/main " to compile and run the program on linux shell.
3. main() function is in src/main.cpp.
4. The newOrder and the delivery transactions as well as the sumQuery is written as functions in main.cpp.
5. In edge cases, a transaction runs as follows:
	+ SELECT: if the required tuples donot exist, a transaction must abort because the variables from this SELECT-statement will be needed below.
	+ INSERT: if inserting an duplicate PK into a table, the transaction ignores this statement and keeps running 
	+ UPDATE and DELETE: if updating/deleting a non-existing tuple, the statement is ignored.
	+ For INSERT and DELETE, We can let the transaction abort in the edge cases by uncommenting the program lines that are commented next to the current running ones and commenting the current running lines for that statement. The program will run much more faster (better performance due to aborts).
6. The programm is built and tested by Eclipse IDE on Xubuntu 16.04 LTS.

### Task 4

1. Create two folders named "tbl" and "bin" in root folder and put the *.tbl-files into the "tbl" folder so that the program can import the data.
2. Import the project in Eclipse IDE as New Makefile project to run it or use command " make && ./bin/main " to compile and run the program on linux shell.
3. main() function is in src/main.cpp.
4. The query that is parsed is stored in file '_task4_query.sql' in the root folder.
5. The generated code is in file 'TranslatedQuery.cpp' in the root folder and will be run automatically after created.
6. The programm is built and tested on Eclipse IDE neon.1 on Xubuntu 16.04 LTS.

### Task 5

1. Create two folders named "tbl" and "bin" in root folder and put the *.tbl-files into the "tbl" folder so that the program can import the data.
2. Import the project in Eclipse IDE as New Makefile project or use command " make && ./bin/main " to compile and run the program on linux shell.
3. main() function is in src/main.cpp.
4. The program can only process queries WITHOUT a semicolon ";" at the end of the statements.
5. The programm is built and tested on Eclipse IDE neon.1 on Xubuntu 16.04 LTS.

<a name="final_project"></a>
# Final Project Implementing the MVCC feature of Hekaton DBMS

Repo: https://github.com/anhmt90/hekaton-db

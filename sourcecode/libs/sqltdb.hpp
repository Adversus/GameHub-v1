/***********************************************
* sqltdb.hpp
* Created By: Randy Campbell
* Modified By: Donald Franklin
*
* Database Interfaces 
************************************************/

#ifndef SQLTDB_HPP
#define SQLTDB_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>

#include "sqlite3.h"

using namespace std;

/**************************************************
***************************************************
CLASSES

dbDataItem:
***************************************************
***************************************************/

class dbStrDataItem
//*** HOLDS DATA FOR ONE COLUMN OF ONE ROW
{
string data;
unsigned char nullVal; //*** NZ means NULL in which case data is ""

public:
dbStrDataItem() {data = ""; nullVal = 0;}
operator string() {return data;}
const string &operator=(const string &item) 
                                     {data = item; nullVal = 0; return item;}
int isNull() {return nullVal;}
void setNull() {data = ""; nullVal = 1;}
void clearNull() {nullVal = 0;}
void clear() {data = ""; nullVal = 0;}
string getData() {return data;}
friend ostream &operator<<(ostream &ofl, const dbStrDataItem &item);
};

class dbStrRow
{
vector < dbStrDataItem > data;

public:
dbStrDataItem &operator[](int idx) {return data[idx];}
void push_back(const dbStrDataItem &item) {data.push_back(item);}
int size(){return data.size();}
void clear(){data.clear();}
};


class dbStrResults
{
vector< dbStrRow > data;


public:
void clear() {data.clear(); }
int numRows() {return data.size(); }
dbStrRow &operator[](int idx) {return data[idx]; }
void push_back(const dbStrRow &row) {data.push_back(row);}
};


/**************************************************
Database Types
***************************************************/

class sqltWrap
{
string dbName;
dbStrResults data;
sqlite3 *dbPtr;
int sqltResult; //*** SQLite RESULT CODE
sqlite3_stmt *prepare_stmt;

public:
sqltWrap (const string &pname = "")
                           {dbName = pname; dbPtr = NULL; data.clear(); prepare_stmt = NULL;}

int numRows() {return data.numRows(); }
dbStrRow &operator[](int idx) {return data[idx]; }
int open(const string &dbFullName = "");
void close();
int doCommand(const string &command);
int prepare(const string &command);
int bind(const int idx, const int value);
int bind(const int idx, const long int value);
int bind(const int idx, const char * value);
int bind(const int idx, string &value);
int lastInsertId();
int runPrepared();
dbStrResults getData(){return data;}
//int doCommand(const string &command, const vector < vector <string> > &args);

};

/***********************************************
WRAPPER RESULT CODES

***********************************************/

#define DB_SUCCESS                    0   /* Success */
#define DB_ALREADY_OPEN               1   /* Tried to open when already open */
#define DB_OPEN_FAIL                  2   /* General failure to open error */
#define DB_NOT_OPEN                   3   /* Attemped op on unopened DB */
#define DB_SQL_COMMAND_ERROR          4   /* General SQL command error */
#define DB_SQL_COMMAND_RUN_ERROR      5   /* General error running SQL */

/***********************************************
SQLite RESULT CODES

#define SQLITE_OK           0    Successful result 
**** beginning-of-error-codes 
#define SQLITE_ERROR        1    SQL error or missing database 
#define SQLITE_INTERNAL     2    Internal logic error in SQLite 
#define SQLITE_PERM         3    Access permission denied 
#define SQLITE_ABORT        4    Callback routine requested an abort 
#define SQLITE_BUSY         5    The database file is locked 
#define SQLITE_LOCKED       6    A table in the database is locked 
#define SQLITE_NOMEM        7    A malloc() failed 
#define SQLITE_READONLY     8    Attempt to write a readonly database 
#define SQLITE_INTERRUPT    9    Operation terminated by sqlite3_interrupt()
#define SQLITE_IOERR       10    Some kind of disk I/O error occurred 
#define SQLITE_CORRUPT     11    The database disk image is malformed 
#define SQLITE_NOTFOUND    12    Unknown opcode in sqlite3_file_control() 
#define SQLITE_FULL        13    Insertion failed because database is full 
#define SQLITE_CANTOPEN    14    Unable to open the database file 
#define SQLITE_PROTOCOL    15    Database lock protocol error 
#define SQLITE_EMPTY       16    Database is empty 
#define SQLITE_SCHEMA      17    The database schema changed 
#define SQLITE_TOOBIG      18    String or BLOB exceeds size limit 
#define SQLITE_CONSTRAINT  19    Abort due to constraint violation 
#define SQLITE_MISMATCH    20    Data type mismatch 
#define SQLITE_MISUSE      21    Library used incorrectly 
#define SQLITE_NOLFS       22    Uses OS features not supported on host 
#define SQLITE_AUTH        23    Authorization denied 
#define SQLITE_FORMAT      24    Auxiliary database format error 
#define SQLITE_RANGE       25    2nd parameter to sqlite3_bind out of range 
#define SQLITE_NOTADB      26    File opened that is not a database file 
#define SQLITE_ROW         100   sqlite3_step() has another row ready 
#define SQLITE_DONE        101   sqlite3_step() has finished executing 
***********************************************/
#endif

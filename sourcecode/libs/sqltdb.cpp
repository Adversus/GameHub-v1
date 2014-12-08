/***********************************************
* sqltdb.cpp
* Created By: Randy Campbell
*
* Database interface for instructor web system
************************************************/

#include "sqltdb.hpp"

using namespace std;


/**************************************************
Public Functions
***************************************************/

ostream &operator<<(ostream &ofl, const dbStrDataItem &item)
{
if (item.nullVal)
   ofl << "(NULL)";
else
   ofl << item.data;
   
return ofl;
}

//*********************************************
//*********************************************

int sqltWrap::open(const string &dbFullName)
// RETURNS 0 ON SUCCESS, NZ ON FAILURE TO OPEN THE DB
{ int sqlRslt;

if (dbPtr != NULL)
   return DB_ALREADY_OPEN;
   
sqlite3_initialize();
sqltResult = sqlite3_open_v2(dbFullName.c_str(), &dbPtr, SQLITE_OPEN_READWRITE |
                                                     SQLITE_OPEN_CREATE, NULL);
                                                     
if (sqltResult != SQLITE_OK)
   {
   sqlite3_close(dbPtr);
   return 1;
   }
   
dbName = dbFullName;
return DB_SUCCESS;
}

//*********************************************
//*********************************************

void sqltWrap::close()
{
if (dbPtr)
   {
   sqlite3_close(dbPtr);
   sqlite3_shutdown();
   }
dbPtr = NULL;
}

//*********************************************
//*********************************************

int sqltWrap::doCommand(const string &command)
{ sqlite3_stmt *stmt = NULL;
  const char   *sqltData = NULL;
  int numColumns;
  dbStrRow tempRow;
  dbStrDataItem tempItem;

//*** CLEAR PREVIOUS DATA AND ENSURE THAT THE DB IS OPEN
data.clear();
if (!dbPtr)
   return DB_NOT_OPEN;

//*** PREP STATEMENT... RETURN IF INCORRECT
sqltResult = sqlite3_prepare_v2(dbPtr, command.c_str(), -1, &stmt, NULL);
if (sqltResult != SQLITE_OK)
   return DB_SQL_COMMAND_ERROR;

//*** RUN THE STATEMENT... ACCUMULATE RESULTS IF NECESSARY
sqltResult = sqlite3_step(stmt);

//*** CHECK FOR NO DATA OR PROBLEM RUNNING THE STATEMENT
if (sqltResult == SQLITE_DONE) //*** GOOD COMMAND, NO RETURN DATA
   {
   sqlite3_finalize(stmt);
   return DB_SUCCESS;
   }

if (sqltResult != SQLITE_ROW) //*** PROBLEM TRYING TO RUN THE STATEMENT
   return DB_SQL_COMMAND_RUN_ERROR;

//*** ACCUMULATE DATA FROM ROWS
while (sqltResult == SQLITE_ROW)
   {
   //*** PREP FOR GETTING THE CURRENT ROW
   tempRow.clear();
   numColumns = sqlite3_column_count( stmt);
   
   //*** ACTUALLY RETRIEVE THE DATA FOR THE CURRENT ROW
   for (int i = 0; i < numColumns; i++)
      {
      if ((sqltData = (const char *)sqlite3_column_text(stmt, i)) == NULL)
         tempItem.setNull();
      else
         tempItem = sqltData;
         
      tempRow.push_back(tempItem);
      }
   //*** ADD ROW TO RESULTS
   data.push_back(tempRow);
   
   //*** GET NEXT ROW
   sqltResult = sqlite3_step(stmt);
   }

//*** 
sqlite3_finalize(stmt);
return DB_SUCCESS;
}

//*********************************************
//*********************************************

int sqltWrap::prepare(const string &command)
{ const char *sqltData = NULL;
  int numColumns;
  dbStrRow tempRow;
  dbStrDataItem tempItem;

//*** CLEAR PREVIOUS DATA AND ENSURE THAT THE DB IS OPEN
data.clear();
if (!dbPtr)
   return DB_NOT_OPEN;

//*** PREP STATEMENT... RETURN IF INCORRECT
sqltResult = sqlite3_prepare_v2(dbPtr, command.c_str(), -1, &prepare_stmt, NULL);
if (sqltResult != SQLITE_OK)
   return DB_SQL_COMMAND_ERROR;
  
return DB_SUCCESS;
}

//*********************************************
//*********************************************

int sqltWrap::bind(const int idx, const int value)
{
	sqltResult = sqlite3_bind_int(prepare_stmt,
                                        idx,
                                        value);
	return sqltResult;
}

//*********************************************
//*********************************************

int sqltWrap::bind(const int idx, const long int value)
{
	sqltResult = sqlite3_bind_int64(prepare_stmt,
                                        idx,
                                        value);
	return sqltResult;
}

//*********************************************
//*********************************************

int sqltWrap::bind(const int idx, const char * value)
{
	sqltResult = sqlite3_bind_text(prepare_stmt,
                                        idx,
                                        value,
                                        strlen(value),
                                        SQLITE_TRANSIENT);
	
	return sqltResult;
}

//*********************************************
//*********************************************

int sqltWrap::lastInsertId(){
	return sqlite3_last_insert_rowid(dbPtr);
}

//*********************************************
//*********************************************

int sqltWrap::bind(const int idx, string &value)
{
	const char *cPtr = value.c_str();
	sqltResult = sqlite3_bind_text(prepare_stmt,
                                        idx,
                                        cPtr,
                                        strlen(cPtr),
                                        SQLITE_TRANSIENT);
	
	return sqltResult;
}

//*********************************************
//*********************************************

int sqltWrap::runPrepared()
{ const char   *sqltData = NULL;
  int numColumns;
  dbStrRow tempRow;
  dbStrDataItem tempItem;
  
//** AVOID NULL HANDLES  
if (prepare_stmt == NULL) return DB_NOT_OPEN;

//*** RUN THE STATEMENT... ACCUMULATE RESULTS IF NECESSARY
sqltResult = sqlite3_step(prepare_stmt);

//*** CHECK FOR NO DATA OR PROBLEM RUNNING THE STATEMENT
if (sqltResult == SQLITE_DONE) //*** GOOD COMMAND, NO RETURN DATA
   {
   sqlite3_finalize(prepare_stmt);
   return DB_SUCCESS;
   }

if (sqltResult != SQLITE_ROW) //*** PROBLEM TRYING TO RUN THE STATEMENT
   return DB_SQL_COMMAND_RUN_ERROR;

//*** ACCUMULATE DATA FROM ROWS
while (sqltResult == SQLITE_ROW)
   {
   //*** PREP FOR GETTING THE CURRENT ROW
   tempRow.clear();
   numColumns = sqlite3_column_count( prepare_stmt );
   
   //*** ACTUALLY RETRIEVE THE DATA FOR THE CURRENT ROW
   for (int i = 0; i < numColumns; i++)
      {
      if ((sqltData = (const char *)sqlite3_column_text(prepare_stmt, i)) == NULL)
         tempItem.setNull();
      else
         tempItem = sqltData;
         
      tempRow.push_back(tempItem);
      }
   //*** ADD ROW TO RESULTS
   data.push_back(tempRow);
   
   //*** GET NEXT ROW
   sqltResult = sqlite3_step(prepare_stmt);
   }

//*** 
sqlite3_finalize(prepare_stmt);
return DB_SUCCESS;
}
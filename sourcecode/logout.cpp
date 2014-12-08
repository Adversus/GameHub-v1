/***********************************************************
logout.cpp

CGI script to handle a login
************************************************************/

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>

#include "libs/common.hpp"
#include "libs/query.hpp"
#include "libs/sqltdb.hpp"
#include "libs/dateTime.hpp"

using namespace std;

const int pd_SKEY = 0;

/***********************************************************
main
main()

Logout a user.
************************************************************/

main()
{ string postText = retrievePost(), 
		 uid,
         pwd,
         uFilePathAndName,
		 query;
  int dbResult = 0;
  sqltWrap db;
  vector<string> postData;
         
//*** OUTPUT CONTENT TYPE HEADER
cout << "Content-Type: text/plain\n\n";

//** No data sent
if (postText == "" || postText == "||"){
	cout << "ERROR||No message";
	return 0;
}

//** Split input into vector
tokenizeStr(postText, "||", postData);

//** Connect to DB
db.open(DIR_DB);

//** Confirm logged in
dbResult = checkSession(db, postData[pd_SKEY].c_str());
if (dbResult != DB_SUCCESS){
	cout << "ERROR" << DLM << "Failed to check session id [" << dbResult << "]" << endl;
	return 0;
}
if (db.numRows() == 0){
	cout << "ERROR" << DLM << "Not logged in.";
	return 0;
}

//** TODO: Should we check if the session exists first?
query = "DELETE FROM HubSessions WHERE uid = '?'";
dbResult = db.prepare(query);
if (dbResult != DB_SUCCESS) {
	cout << "ERROR||Failed to prepare query. [" << dbResult << "]" << endl;
	return 0;
}
db.bind(1, postData[pd_SKEY].c_str());

//** Run query (retrieve matching user data)
dbResult = db.runPrepared();
if (dbResult != DB_SUCCESS) {
	cout << "ERROR||Failed to run prepared query [" << dbResult << "]" << endl;
	return 0;
}

cout << "LOGOUT" << endl;
}

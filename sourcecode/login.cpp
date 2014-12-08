/***********************************************************
login.cpp

CGI script to handle a login
************************************************************/
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stdlib.h>
#include <ctype.h>
#include "libs/common.hpp"
#include "libs/query.hpp"
#include "libs/randStrHash.hpp"
#include "libs/sqltdb.hpp"

using namespace std;

const int pd_USERNAME = 0,
		  pd_PASSWORD = 1;
const int q2_UID = 0,
		  q2_USERNAME = 1,
		  q2_PASSWORD = 2,
		  q2_PASSSALT = 3;

/***********************************************************
main
main()

Check username and password.  If correct, creates session key and 
timestamp which are stored in the user's file.  Returns skey.
If login is incorrect returns message to that effect.
************************************************************/

int main()
{ string uid,
		 combinedPassword,
         convertedPassword,
         timeout,
		 postText = retrievePost(),
		 query,
		 skey,
		 userID,
		 userLevel,
		 userName,
		 userBan,
		 userMute,
		 strTmp;
	int dbResult = 0;
	sqltWrap db;
	vector<string> postData;
	long int currentTime = static_cast<long int>(time(NULL));
         
	//*** OUTPUT CONTENT TYPE HEADER
	cout << "Content-Type: text/plain\n\n";

	//** No data sent
	if (postText == "" || postText == DLM){
		cout << "ERROR" << DLM << "No message";
		return 0;
	}

	//** Split input into vector
	tokenizeStr(postText, DLM, postData);

	//** Invalid data length
	if (postData.size() < 2){
		cout << "ERROR" << DLM << "Invalid data";
		return 0;
	}

	//** Convert username to uppercase for case insensitive matching
	string strConvert = postData[pd_PASSWORD],
		   strResult = toUpper(strConvert);

	//** Connect to DB
	db.open(DIR_DB);

	//** Prepare query
	query = "SELECT UserID, Username, Password, PasswordSalt, UserLevel, BanLevel, MuteLevel FROM HubUsers WHERE Username = ? AND BanLevel < UserLevel";
	dbResult = db.prepare(query);
	if (dbResult != DB_SUCCESS) {
		cout << "ERROR" << DLM << "Failed to prepare query. [" << dbResult << "]" << endl;
		return 0;
	}
	db.bind(1, postData[pd_USERNAME].c_str());

	//** Run query (retrieve matching user data)
	dbResult = db.runPrepared();
	if (dbResult != DB_SUCCESS) {
		cout << "ERROR" << DLM << "Failed to run prepared query [" << dbResult << "]" << endl;
		return 0;
	}

	if (db.numRows() < 1) //*** ERROR - RETURN ERROR AND MESSAGE
	{
	   cout << "ERROR" << DLM << "UNKNOWN USER OR INCORRECT PASSWORD" << endl;
	   exit(0);
	}

	userID = db[0][0];
	userName = db[0][1];
	userLevel = db[0][4];
	userBan = db[0][5];
	userMute = db[0][6];
	   
	//** Password conversion and matching
	combinedPassword = postData[pd_PASSWORD] + db[0][q2_PASSSALT].getData();
	sha256Hash64(convertedPassword, combinedPassword);

	//*** TEST THE PASSWORD, IF BAD, SEND ERROR MESSAGE AND QUIT
	if (convertedPassword != db[0][q2_PASSWORD].getData())
	   {
	   cout << "ERROR" << DLM << "UNKNOWN USER OR INCORRECT PASSWORD" << endl;
	   exit(0);
	   }

	//*** EVERYTHING CHECKS OUT - SET UP SKEY
	randStr(skey, 20);
	uid = db[0][q2_UID].getData();

	//** Clear old sessions
	dbResult = clearOldSessions(db, uid);
	if (dbResult != DB_SUCCESS) {
		cout << "ERROR" << DLM << "Failed to clear old sessions" << endl;
		return 0;
	}

	//** Create new session
	dbResult = createSession(db, uid, userLevel, skey);
	if (dbResult != DB_SUCCESS) {
		cout << "ERROR" << DLM << "Failed to create new session" << endl;
		return 0;
	}

	//*** INDICATE OK AND RETURN skey to the user
	cout << "LOGIN" << DLM << skey << DLM << userID << DLM << userName << DLM << userLevel << DLM << userBan << DLM << userMute;
}

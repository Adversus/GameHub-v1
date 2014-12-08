#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>
#include "libs/common.hpp"
#include "libs/sqltdb.hpp"
#include "libs/query.hpp"

using namespace std;

const int pd_SKEY = 0,
		  pd_COMMAND = 1;
const string COMMAND_START = "START",
			 COMMAND_UPDATE = "UP";
			 
int updateSessionTime(time_t &currentTime, string &userID){
	sqltWrap db;
	int dbResult;
	string queryStr;
	
	//** Update session data with new time
	queryStr = "UPDATE Sessions SET UpdateTime = ? WHERE UserID = ?";
	db.prepare(queryStr);
	db.bind(1, currentTime);
	db.bind(2, userID);
	dbResult = db.runPrepared();
	if (dbResult != DB_SUCCESS) {
		cout << "ERROR" << DLM << "Failed to update chat time [" << dbResult << "]" << endl;
		return 0;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	string postText = retrievePost(),
		   postStr,
		   queryStr,
		   sendStr = "",
		   tmpName,
		   tmpMessage,
		   tmpColor,
		   lastUpdate,
		   userID,
		   strCommand = COMMAND_UPDATE;
	int dbResult = 0,
		iUpdateTime = 0;
	time_t currentTime = time(0);
	long int updateTime = 0;
	sqltWrap db;
	vector <string> userData;
	
	cout << "Content-Type: text/html\n\n";
	
	//** Split input into vector
	tokenizeStr(postText, DLM, userData);
	
	//** Get messages from database
	db.open(DIR_DB);
	
	//** Confirm logged in
	dbResult = checkSession(db, userData[pd_SKEY].c_str(), ", UpdateTime") != DB_SUCCESS;
	if (dbResult != DB_SUCCESS){
		cout << "ERROR" << DLM << "Failed to check session id [" << dbResult << "]" << endl;
		return 0;
	}
	if (db.numRows() == 0){
		cout << "ERROR" << DLM << "Not logged in.";
		return 0;
	}
	userID = db[0][0];
	lastUpdate = db[0][2];
	iUpdateTime = atoi(lastUpdate.c_str());
	
	//** Check for commands
	if (userData.size() > 1){
		if (userData[pd_COMMAND] == COMMAND_START){
			strCommand = COMMAND_START;
		}
	}
	
	//** Update session data with new time
	queryStr = "UPDATE Sessions SET UpdateTime = ? WHERE UserID = ?";
	db.prepare(queryStr);
	db.bind(1, currentTime);
	db.bind(2, userID);
	if (db.runPrepared() != DB_SUCCESS) {
		cout << "ERROR" << DLM << "Failed to update chat time [" << dbResult << "]" << endl;
		return 0;
	};
	
	//** Get messages from database
	//** Limiting the max time eliminates the chance of duplicates when...
	//**	a message is posted in the ~8ms it can take to run this script.
	queryStr = "SELECT Username, Msg, Color FROM HubMessages WHERE CreateTime >= ? AND CreateTime < ? ORDER BY CreateTime ASC";
	db.prepare(queryStr);
	db.bind(1, lastUpdate);
	db.bind(2, currentTime);
	
	//** Run command and send output
	if (db.runPrepared() == DB_SUCCESS) {
		sendStr = "";
		for (int i=0; i < db.numRows();i++){
			tmpName = db[i][0];
			tmpMessage = db[i][1];
			tmpColor = db[i][2];
			sendStr += DLM + tmpName + DLM + tmpMessage + DLM + tmpColor;
		}
		cout << currentTime << sendStr << endl;
	} else {
		cout << "ERROR" << DLM << "Invalid query \"" << queryStr << "\"" << endl;
	}
}
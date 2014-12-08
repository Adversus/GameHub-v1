#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>
#include "libs/common.hpp"
#include "libs/sqltdb.hpp"
#include "libs/query.hpp"
#include "libs/randStrHash.hpp"

using namespace std;

const int pd_SKEY = 0,
		  pd_COMMAND = 1;
		  
const string COMMAND_MYPROFILE = "SHOWME",
			 COMMAND_OTHERPROFILE = "SHOWOTHER",
			 COMMAND_COLOR = "COLOR",
			 COMMAND_NEWREG = "NEWREG",
			 COMMAND_NEWPASS = "NEWPASS",
			 COMMAND_NEWSTATS = "NEWSTATS";
			 
//** Other fields
const int pd_PROFID = 2,
		  pc_COLOR = 2;

int profileCommandColor(sqltWrap &db, string &userID, vector<string> &userData){
	string queryStr = "UPDATE HubUsers SET Color = ? WHERE UserID = ?";
	int dbResult;
	
	//** Error checking (force user to send skey, command, and color)
	if (userData.size() < 3){
		cout << "ERROR" << DLM << "Invalid command" << endl;
		return 0;
	}
	
	//** Force color size to be exactly 6 characters
	if (userData[pc_COLOR].size() < 6){
		cout << "ERROR" << DLM << "Invalid command" << endl;
		return 0;
	}
	
	//** TODO Confirm color is 6 numbers
	
	//** Update user's chat color
	db.prepare(queryStr);
	db.bind(1, userData[pc_COLOR]);
	db.bind(2, userID);
	dbResult = db.runPrepared();
	
	if (dbResult != DB_SUCCESS) {
		cout << "ERROR||Failed to run prepared query [" << dbResult << "]" << endl;
		return 0;
	}
	
	//** Return command signalling success
	cout << COMMAND_COLOR << DLM << userData[pc_COLOR] << endl;
	return 0;
}

int main(int argc, char* argv[])
{
	string postText = retrievePost(),
		   postStr,
		   queryStr,
		   sendStr = "",
		   tmpName,
		   tmpLevel,
		   tmpBan,
		   tmpMute,
		   tmpID,
		   tmpRegCode,
		   tmpFname,
		   tmpLname,
		   userID,
		   skey;
	int dbResult = 0;
	time_t currentTime = time(0);
	long int profileID = 0,
			 userLevel = 0;
	sqltWrap db;
	vector <string> userData;
	
	cout << "Content-Type: text/html\n\n";
	
	//** Split input into vector
	tokenizeStr(postText, DLM, userData);
	
	//** Error checking (force user to send skey and command)
	if (userData.size() < 2){
		cout << "ERROR" << DLM << "Invalid command" << endl;
		return 0;
	}
	
	//** Connect to DB
	db.open(DIR_DB);
	
	//** Confirm logged in
	dbResult = checkSession(db, userData[pd_SKEY].c_str());
	if (dbResult != DB_SUCCESS){
		cout << "ERROR" << DLM << "Failed to check session id [" << dbResult << "]" << endl;
		return 0;
	}
	if (db.numRows() == 0){
		cout << "ERROR" << DLM << "Not logged in.";
		return 0;
	}
	userID = db[0][0];
	
	//** Get profile action
	if (userData[pd_COMMAND] == COMMAND_MYPROFILE || userData[pd_COMMAND] == COMMAND_OTHERPROFILE){
		//** Load profile of this user
		if (userData[pd_COMMAND] == COMMAND_OTHERPROFILE){
			//** Confirm client send id
			if (userData.size() < 3){
				cout << "ERROR" << DLM << "Invalid command" << endl;
				return 0;
			}
	
			profileID = strtol(userData[pd_PROFID].c_str(), NULL, 10);
		} else {
			profileID = strtol(userID.c_str(), NULL, 10);
		}
		
		queryStr = "SELECT Username, UserLevel, BanLevel, MuteLevel, RegistrationCode, FirstName, LastName FROM HubUsers WHERE UserID = ? LIMIT 1";
		
		db.prepare(queryStr);
		db.bind(1, userID.c_str());
		
		//** Run command and send output
		if (db.runPrepared() != DB_SUCCESS) {
			cout << "ERROR" << DLM << "Invalid query \"" << queryStr << "\"" << endl;
			return 0;
		}
		
		//** Check count
		if (db.numRows() < 1){
			cout << "ERROR" << DLM << "User does not exist." << endl;
			return 0;
		}
		
		tmpName = db[0][0];
		tmpLevel = db[0][1];
		tmpBan = db[0][2];
		tmpMute = db[0][3];
		
		//** Only return a registration code if the user level is high enough
		if (tmpLevel != "1" && tmpLevel != "0"){
			tmpRegCode = db[0][4];
		} else {
			tmpRegCode = "";
		}
		
		tmpFname = db[0][5];
		tmpLname = db[0][6];
		
		//** Return command and data signalling success
		cout << userData[pd_COMMAND] << DLM << tmpName <<
										DLM << tmpLevel <<
										DLM << tmpBan <<
										DLM << tmpMute <<
										DLM << tmpRegCode << 
										DLM << tmpFname <<
										DLM << tmpLname << endl;
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_COLOR){
		//** Update user color
		profileCommandColor(db, userID, userData);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_NEWREG){
		//** Randomize used registration key
		randStr(skey, 20);
		if (setRegKey(db, skey.c_str(), userID.c_str())){
			//** Return command and data signalling success
			cout << COMMAND_NEWREG << DLM << skey << endl;
		}
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_NEWPASS){
		//** Change user's password
		cout << COMMAND_NEWPASS << endl;
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_NEWSTATS){
		//** Update users basic info
		cout << COMMAND_NEWSTATS << endl;
		return 0;
	}
	
	//** Default to error
	cout << "ERROR" << DLM << "Invalid command" << endl;
}
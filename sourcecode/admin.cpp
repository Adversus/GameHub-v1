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
const int pd_NEWTIMEOUT = 2;
const int pd_NEWLEVEL = 2,
		  pd_TARGETUSER = 3;
const int pd_DELUSER = 3;
		  
const string COMMAND_USERS = "USERS",					//Get list of users +(optional: name)
			COMMAND_TIMEOUT = "TIMEOUT",				//Set new global timeout +(new value)
			COMMAND_USERLEVEL = "USERLEVEL",			//Set user's level +(user id, new userlevel)
			COMMAND_REGLIST = "REGLIST",				//Get list of registration requests
			COMMAND_REGDELETE = "REGDELETE",
			COMMAND_REGINVITE = "REGINVITE",
			COMMAND_BANLIST = "BANLIST",				//Get list of banned or muted users
			COMMAND_BANUSER = "SETBAN",					//Set user ban level +(user id)
			COMMAND_MUTEUSER = "SETMUTE",				//Set user mute level +(user id)
			COMMAND_USERDELETE = "USERDELETE",			//Remove user's account +(user id)
			COMMAND_LOG = "LOG";						//Get list of recent changes
			
int adminSendUserList(sqltWrap &db){
	cout << COMMAND_USERS << endl;
	return 0;
}

int adminSetTimeout(sqltWrap &db, vector<string> &userData, string &userID){
	//** Check data size
	if (userData.size() < 3){
		cout << "ERROR" << "Invalid command data." << endl;
		return 0;
	}
	
	//** Record action
	logMessage(db, userID, "User " + userID + " set global timeout to " + userData[pd_NEWTIMEOUT]);
	
	cout << COMMAND_TIMEOUT << endl;
	return 0;
}

int adminSetUserLevel(sqltWrap &db, vector<string> &userData, string &userID){
	string queryStr = "UPDATE HubUsers SET UserLevel = ? WHERE UserID = ?",
			tmpStr;
	int dbResult = 0,
		newLevel = 0,
		targetID = 0;
	
	//** Check data size
	if (userData.size() < 4){
		cout << "ERROR" << DLM << "Invalid command data." << endl;
		return 0;
	}
	
	//** Get new mute level
	tmpStr = userData[pd_NEWLEVEL];
	newLevel = strtol(tmpStr.c_str(), NULL, 10);
	
	cout << COMMAND_USERLEVEL << endl;
	return 0;
}

int adminSendRegList(sqltWrap &db){
	string queryStr = "SELECT RequestID, FirstName, LastName, Email, sentID FROM RegRequest",
			sendData = "";
			
	db.prepare(queryStr);
	db.runPrepared();
	
	for (int i=0; i<db.numRows(); i++){
		sendData += DLM + db[i][0].getData() + DLM + db[i][1].getData() + DLM + db[i][2].getData() + DLM + db[i][3].getData() + DLM + db[i][4].getData();
	}
	
	//** We don't have a table for registration requests
	cout << COMMAND_REGLIST << DLM << db.numRows() << sendData << endl;
	return 0;
}

int adminSendBanList(sqltWrap &db){
	string queryStr = "SELECT UserID, Username, UserLevel FROM HubUsers WHERE BanLevel > 0 OR MuteLevel > 0";
	cout << COMMAND_BANLIST << endl;
	return 0;
}

int adminBanUser(sqltWrap &db, vector<string> &userData, string userID){
	string queryStr = "UPDATE HubUsers SET BanLevel = ? WHERE UserID = ?",
			tmpStr;
	int dbResult = 0,
		newLevel = 0,
		targetID = 0;
	
	//** Check data size
	if (userData.size() < 4){
		cout << "ERROR" << DLM << "Invalid command data." << endl;
		return 0;
	}
	
	//** Get new mute level
	tmpStr = userData[pd_NEWLEVEL];
	newLevel = strtol(tmpStr.c_str(), NULL, 10);
	
	//** Get user id
	if (userData[pd_TARGETUSER] == userID){
		cout << "ERROR" << "You cannot ban yourself." << endl;
		return 0;
	}
	
	//** Record action
	logMessage(db, userID, "User " + userID + " set user " + userData[pd_TARGETUSER] + "'s ban level to " + userData[pd_NEWLEVEL]);
	
	//** Run query
	db.prepare(queryStr);
	db.bind(1, newLevel);
	db.bind(2, targetID);
	dbResult = db.runPrepared();
	if (dbResult != DB_SUCCESS){
		cout << "ERROR" << DLM << "Failed to run command [" << dbResult << "]" << endl;
		return 0;
	}
	
	cout << COMMAND_BANUSER << endl;
	return 0;
}

int adminMuteUser(sqltWrap &db, vector<string> &userData, string userID){ //** TODO: Error checking
	string queryStr = "UPDATE HubUsers SET MuteLevel = ? WHERE UserID = ?",
			tmpStr;
	int dbResult = 0,
		newLevel = 0,
		targetID = 0;
	
	//** Check data size
	if (userData.size() < 4){
		cout << "ERROR" << DLM << "Invalid command data." << endl;
		return 0;
	}
	
	//** Get new mute level
	tmpStr = userData[pd_NEWLEVEL];
	newLevel = strtol(tmpStr.c_str(), NULL, 10);
	
	//** Get user id
	if (userData[pd_TARGETUSER] == userID){
		cout << "ERROR" << "You cannot mute yourself." << endl;
		return 0;
	}
	
	//** Record action
	logMessage(db, userID, "User " + userID + " set user " + userData[pd_TARGETUSER] + "'s mute level to " + userData[pd_NEWLEVEL]);
	
	//** Run query
	db.prepare(queryStr);
	db.bind(1, newLevel);
	db.bind(2, targetID);
	dbResult = db.runPrepared();
	if (dbResult != DB_SUCCESS){
		cout << "ERROR" << DLM << "Failed to run command [" << dbResult << "]" << endl;
		return 0;
	}
	
	cout << COMMAND_MUTEUSER << endl;
	return 0;
}

int adminDeleteUser(sqltWrap &db, vector<string> &userData, string &userID){
	//** Check data size
	if (userData.size() < 3){
		cout << "ERROR" << "Invalid command data." << endl;
		return 0;
	}
	
	//** This one is gonna take some work
	//**	should we also delete all the games?
	//**	... and everything else?
	
	//** Record action
	logMessage(db, userID, "User " + userID + " deleted user " + userData[pd_DELUSER]);
	
	cout << COMMAND_USERDELETE << endl;
	return 0;
}

int adminSendLog(sqltWrap &db){
	string queryStr = "SELECT LogText FROM AdminLog ORDER BY CreateTime DESC LIMIT 100",
		   sendStr = "";
	int dbResult = 0;
	
	//** Run query
	db.prepare(queryStr);
	dbResult = db.runPrepared();
	if (dbResult != DB_SUCCESS){
		cout << "ERROR" << DLM << "Failed to retrieve log [" << dbResult << "] " << queryStr << endl;
		return 0;
	}
	for (int i=0; i<db.numRows(); i++){
		sendStr += DLM + db[i][0].getData();
	}
		
	cout << COMMAND_LOG << DLM << db.numRows() << sendStr << endl;
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
	userLevel = atol(db[0][1].getData().c_str());
	
	//** Prevent non-admin access
	if (userLevel < ADMIN_PANEL_LEVEL){
		cout << "ERROR" << DLM << "Invalid access level." << endl;
		return 0;
	}
	
	//** Get profile action
	if (userData[pd_COMMAND] == COMMAND_USERS){
		//** Get list of all users
		adminSendUserList(db);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_TIMEOUT){
		//** Set global system timeout
		adminSetTimeout(db, userData, userID);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_USERLEVEL){
		//** Set users access level
		adminSetUserLevel(db, userData, userID);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_REGLIST){
		//** Get registration request list
		adminSendRegList(db);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_REGDELETE){
		//** Delete reg line
		string tmpID = userData[2];
		int convertedID = strtol(tmpID.c_str(), NULL, 10);
		
		//** Delete the reg
		if (convertedID != -1){
			queryStr = "DELETE FROM RegRequest WHERE id = ?";
			db.prepare(queryStr);
			db.bind(1, convertedID);
			db.runPrepared();
		}
		
		//** Send updated list
		adminSendRegList(db);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_REGINVITE){
		//** Send invite
		string tmpID = userData[2];
		int convertedID = strtol(tmpID.c_str(), NULL, 10);
		
		//** Send the invite
		if (convertedID != -1){
			system("email.pl gamehub@gamehub.org df0004@morningside.edu 'Just testing'");
		}
		
		//** Send updated list
		adminSendRegList(db);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_BANLIST){
		//** Get list of banned and muted users
		adminSendBanList(db);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_BANUSER){
		//** Set users ban level
		adminBanUser(db, userData, userID);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_MUTEUSER){
		//** Set users mute level
		adminMuteUser(db, userData, userID);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_USERDELETE){
		//** Delete a user
		adminDeleteUser(db, userData, userID);
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_LOG){
		//** Get the log
		adminSendLog(db);
		return 0;
	}
	
	//** Default to error
	cout << "ERROR" << DLM << "Invalid command" << endl;
}
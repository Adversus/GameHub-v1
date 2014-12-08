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
		  pd_COMMAND = 1,
		  pd_GAMEID = 2,
		  pd_SESNAME = 3,
		  pd_MAXPLAYERS = 4,
		  pd_PASSWORD = 5;
const string COMMAND_UPDATE = "UPDATE",
			COMMAND_QUIT = "QUIT",
			COMMAND_START = "START",
			COMMAND_POST = "POST";

int main(int argc, char* argv[])
{
	string postText = retrievePost(),
		queryStr,
		instanceID,
		sendStr,
		hasStarted = "F",
		userID,
		pwSalt = "",
		instanceKey = "",
		convertedPassword,
		combinedPassword = "";
	vector<string> userData;
	int dbResult = 0,
		userLevel = 0,
		playerCount = 0,
		lastID = -1;
	sqltWrap db;
	
	cout << "Content-Type: text/plain\n\n";
	
	//** No data sent
	if (postText == "" || postText == DLM){
		cout << "ERROR" << DLM << "No message" << endl;
		return 0;
	}
	
	//** Split input into vector
	tokenizeStr(postText, DLM, userData);
	
	//** Connect to DB
	db.open(DIR_DB);
	
	//** Confirm logged in
	dbResult = checkSession(db, userData[pd_SKEY].c_str()) != DB_SUCCESS;
	if (dbResult != DB_SUCCESS){
		cout << "ERROR" << DLM << "Failed to check session id [" << dbResult << "]" << endl;
		return 0;
	}
	if (db.numRows() == 0){
		cout << "ERROR" << DLM << "Not logged in.";
		return 0;
	}
	userID = db[0][0].getData();
	userLevel = atol(db[0][1].getData().c_str());
	
	//** Handle commands
	if (userData[pd_COMMAND] == COMMAND_POST){
		int numPlayers = 0;
		
		//** Check values
		if (userData.length < 5){
			cout << "ERROR" << DLM << "Invalid post data." << endl;
			return 0;
		}
		
		//** Game ID
		if (userData[1].length < 1){
			cout << "ERROR" << DLM << "Invalid post game ID." << endl;
			return 0;
		}
		
		//** Session Name
		if (userData[2].length < 1){
			cout << "ERROR" << DLM << "Invalid post session name." << endl;
			return 0;
		}
		
		//** Max Players
		numPlayers = strtol(userData[3].c_str(), NULL, 10);
		if (numPlayers < 2){
			cout << "ERROR" << DLM << "Invalid post max players." << endl;
			return 0;
		}
		
		//** Game Pass
		if (userData[4].length > 20){
			cout << "ERROR" << DLM << "Session password cannot be longer than 20 characters." << endl;
			return 0;
		}
		
		//** Check game
		queryStr = "SELECT blah FROM Games";
		dbResult = db.prepare(queryStr);
		
		//** Upload session
		queryStr = "INSERT INTO GameSessions () VALUES ()";
		dbResult = db.prepare(queryStr);
	} else if (userData[pd_COMMAND] == COMMAND_UPDATE){
		//** Get game instance ID
		queryStr = "SELECT InstanceID FROM GameSessions WHERE UserID = ?";
		dbResult = db.prepare(queryStr);
		db.bind(1, userID);
		dbResult = db.runPrepared();
		if (dbResult != DB_SUCCESS){
			cout << "ERROR" << DLM << "Failed to retrieve instance ID [" << dbResult << "]." << endl;
			return 0;
		}
		if (db.numRows() == 0){
			cout << "ERROR" << DLM << "Not in a game.";
			return 0;
		}
		instanceID = db[0][0];
		
		queryStr = "SELECT b.Username FROM GameSessions a, HubUsers b WHERE a.UserID = b.UserID AND a.InstanceID = ?";
		dbResult = db.prepare(queryStr);
		db.bind(1, instanceID);
		dbResult = db.runPrepared();
		if (dbResult != DB_SUCCESS){
			cout << "ERROR" << DLM << "Failed to retrieve instance ID [" << dbResult << "]." << endl;
			return 0;
		}
		
		//** Check player count
		if (hasStarted == "F"){
			if (playerCount == db.numRows()){
				//** Trigger has started message
				queryStr = "UPDATE GameSessions SET HasStarted = 'T' WHERE InstanceID = ?";
				dbResult = db.prepare(queryStr);
				db.bind(1, instanceID);
				dbResult = db.runPrepared();
				if (dbResult != DB_SUCCESS){
					cout << "ERROR" << DLM << "Failed to remove session" << endl;
					return 0;
				}
			}
		}
		
		//** Check for users
		queryStr = "SELECT b.Username FROM GameSessions a, HubUsers b WHERE a.InstanceID = ? AND a.UserID = b.UserID";
		dbResult = db.prepare(queryStr);
		db.bind(1, instanceID);
		dbResult = db.runPrepared();
		if (dbResult != DB_SUCCESS){
			cout << "ERROR" << DLM << "Failed to remove session" << endl;
			return 0;
		}
		if (db.numRows() == 0){
			cout << "ERROR" << DLM << "Not in a game.";
			return 0;
		}
		
		//** Send data
		if (hasStarted == "F"){
			//** Send back the update signal
			sendStr = COMMAND_UPDATE;
		} else {
			//** Send back the ready signal
			sendStr = COMMAND_START;
		}
		
		//** Send users
		sendStr += DLM + toString(db.numRows());
		for (int i=0;i<db.numRows();i++){
			sendStr += DLM + db[i][0].getData();
		}
		cout << sendStr;
	} else if (userData[pd_COMMAND] == COMMAND_QUIT || userData[pd_COMMAND] == COMMAND_START){
		//** Remove user from session
		queryStr = "DELETE FROM GameSessions WHERE UserID = ?";
		dbResult = db.prepare(queryStr);
		db.bind(1, userID);
		dbResult = db.runPrepared();
		if (dbResult != DB_SUCCESS){
			cout << "ERROR" << DLM << "Failed to remove session" << endl;
			return 0;
		}
		
		//** Create new session
		if (userData[pd_COMMAND] == COMMAND_START){
			if (userData[pd_PASSWORD] != ""){
				randStr(pwSalt, 20);
				combinedPassword = userData[pd_PASSWORD] + pwSalt;
				sha256Hash64(convertedPassword, combinedPassword);
			}
			
			randStr(instanceKey, 20);
			
			queryStr = "INSERT INTO GameInstance (InstanceName, InstanceKey, GameID, PosterID, MaxPlayers, PlayerCount, Password, PasswordSalt) VALUES (?, ?, ?, ?, ?, 1, ?, ?)";
			dbResult = db.prepare(queryStr);
			if (dbResult != DB_SUCCESS){
				cout << "ERROR" << DLM << "Failed to prepare query [" << dbResult << "]" << endl;
				return 0;
			}
			string tmp = "";
			db.bind(1, tmp);
			db.bind(2, instanceKey);
			db.bind(3, userData[pd_GAMEID]);
			db.bind(4, userID);
			db.bind(5, userData[pd_MAXPLAYERS]);
			db.bind(6, convertedPassword);
			db.bind(7, pwSalt);
			dbResult = db.runPrepared();
			if (dbResult != DB_SUCCESS){
				cout << "ERROR" << DLM << "Failed to start session [" << dbResult << "]" << endl;
				return 0;
			}
			
			lastID = db.lastInsertId();
			
			queryStr = "INSERT INTO GameSessions (UserID, InstanceID, PlayerPos) VALUES (?, ?, 1)";
			db.prepare(queryStr);
			db.bind(1, userID);
			db.bind(2, lastID);
			dbResult = db.runPrepared();
			if (dbResult != DB_SUCCESS){
				cout << "ERROR" << DLM << "Failed to join new instance [" << dbResult << "]" << endl;
				return 0;
			}
			
			sendStr += DLM + toString(lastID);
		}
		
		cout << userData[pd_COMMAND];
		if (userData[pd_COMMAND] == COMMAND_START){
			cout << DLM << instanceKey;
		}
		cout << sendStr << endl;
	}
	
	return 0;
}
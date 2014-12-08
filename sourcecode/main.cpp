#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>
#include "libs/common.hpp"
#include "libs/sqltdb.hpp"
#include "libs/query.hpp"

const int pd_SKEY = 0,
		  pd_COMMAND = 1;
const int pd_GAMEID = 2;
const string COMMAND_SESSIONLIST = "SESSIONS",
			 COMMAND_POSTGAME = "POST",
			 COMMAND_GAMEUPDATE = "UPDATE",
			 COMMAND_CHANGEUSER = "CHANGEUSER",
			 COMMAND_STARTGAME = "START",
			 COMMAND_JOIN = "JOIN",
			 COMMAND_REGREQUEST = "REGREQUEST";

using namespace std;

int mainSendSessionList(sqltWrap &db, string gameID, string orderBy){
	string queryStr = "SELECT a.SessionID, b.Username FROM GameSessions a, HubUsers b WHERE UserID ORDER BY CreateTime ASC ";

	//** Build query
	if (orderBy != ""){
		//** Add order to query and pad with whitespace if necessary
		queryStr += "ORDER BY";
		if (orderBy.substr(0,1) != " ") queryStr += " ";
		queryStr += orderBy;
		if (orderBy.substr(orderBy.size()-1,1) != " ") queryStr += " ";
	}
	queryStr += "LIMIT 10";
	
	
}

int mainRegRequest(sqltWrap &db, vector<string> &userData){
	string queryStr = "INSERT INTO RegRequest (Email, FirstName, LastName) VALUES (?, ?, ?)";
	int dbResult = 0;
	string fName,
			lName,
			email;
	
	email = userData[1];
	fName = userData[2];
	lName = userData[3];
	
	//** TODO: Better error checking
	if (email == "" || strchr(email.c_str(),'@') == NULL){
		cout << "ERROR" << DLM << "Invalid email.";
		return 0;
	}
	if (fName == ""){
		cout << "ERROR" << DLM << "Invalid first name.";
		return 0;
	}
	if (lName == ""){
		cout << "ERROR" << DLM << "Invalid last name.";
		return 0;
	}
	
	db.prepare(queryStr);
	db.bind(1, email);
	db.bind(2, fName);
	db.bind(3, lName);
	db.runPrepared();
	
	cout << COMMAND_REGREQUEST << DLM << endl;
	return 0;
}

int main(int argc, char* argv[])
{
	string postText = retrievePost(),
		queryStr,
		sendStr = "",
		userID;
	vector<string> userData;
	int dbResult = 0,
		userLevel = 0;
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
	userID = db[0][0];
	userLevel = atol(db[0][1].getData().c_str());
	
	//** Handle commands
	if (userData[pd_COMMAND] == COMMAND_SESSIONLIST){ //**TODO: Finish this
		//** Send list of sessions for a game
		queryStr = "SELECT * FROM GameSessions WHERE ";
		db.prepare(queryStr);
		for (int i=0; i<db.numRows(); i++){
			sendStr += DLM + "";
		}
		
		cout << COMMAND_SESSIONLIST << DLM << db.numRows() << sendStr << endl;
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_GAMEUPDATE){
		//** Send list of players in a game and ok message if all slots are taken
		cout << COMMAND_GAMEUPDATE << endl;
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_CHANGEUSER){
		//** Change slot of player in game
		cout << COMMAND_CHANGEUSER << endl;
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_STARTGAME){
		queryStr = "INSERT INTO GameInstance (GameID, PosterID, maxPlayers, playerCount, Password, PasswordSalt) VALUES (?, ?, ?, 1, ?, ?)";
		db.prepare(queryStr);
		//db.bind(1, gameID);
		db.bind(2, userID);
		//db.bind(3, playerCount);
		//db.bind(4, Password);
		//db.bind(5, PasswordSalt);
		
		//** Initialize a game session
		cout << COMMAND_STARTGAME << endl;
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_JOIN){
		//** Initialize a game session
		cout << COMMAND_JOIN << endl;
		return 0;
	} else if (userData[pd_COMMAND] == COMMAND_REGREQUEST){
		//** Add a registration request
		mainRegRequest(db, userData);
		//cout << COMMAND_REGREQUEST << endl;
		return 0;
	}
	
	cout << "ERROR" << DLM << "Invalid command." << endl;
	return 0;
}
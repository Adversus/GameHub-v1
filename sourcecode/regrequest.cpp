#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime>
#include "libs/common.hpp"
#include "libs/sqltdb.hpp"
#include "libs/query.hpp"

const int pd_COMMAND = 0;
const int pd_GAMEID = 2;
const string COMMAND_REGREQUEST = "REGREQUEST";

using namespace std;

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
	
	//** Handle commands
	if (userData[pd_COMMAND] == COMMAND_REGREQUEST){
		//** Add a registration request
		mainRegRequest(db, userData);
		return 0;
	}
	
	cout << "ERROR" << DLM << "Invalid command." << endl;
	return 0;
}
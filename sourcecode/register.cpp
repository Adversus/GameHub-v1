/***********************************************************
register.cpp
SUBMITTED BY: RANDY
EDITED BY: MARTIN
CGI script to handle registration
************************************************************/

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>

#include "libs/common.hpp"
#include "libs/query.hpp"
#include "libs/sqltdb.hpp"
#include "libs/randStrHash.hpp"

using namespace std;

/***********************************************************
main
main()

Register user if this user doesn't already exist
************************************************************/

const int pd_USERNAME = 0,
		  pd_PASSWORD = 1,
		  pd_FIRSTNAME = 2,
		  pd_LASTNAME = 3,
		  pd_EMAIL = 4,
		  pd_REGCODE = 5;

main()
{ string uid,
		 combinedPassword,
		 convertedPassword,
		 pwSalt,
         email,
         fName,
         lName,
         desiredTimeout,
		 postText = retrievePost(),
		 query,
		 skey,
		 inviteID;
  int unameSize = 0,
	  dbResult = 0;
  sqltWrap db;
  vector<string> postData;
         
//*** OUTPUT CONTENT TYPE HEADER
cout << "Content-Type: text/plain\n\n";

//** No data sent
if (postText == "" || postText == DLM){
	cout << "ERROR" << DLM << "Invalid data";
	return 0;
}

//** Split input into vector
tokenizeStr(postText, DLM, postData);

//** Invalid data length
if (postData.size() < 6){
	cout << "ERROR" << DLM << "Invalid data";
	return 0;
}

//** Invalid data checking
if (!( is_alnum(postData[pd_USERNAME]) )){
	cout << "ERROR" << DLM << "Username must be alphanumeric.";
	return 0;
}
if (!( postData[pd_USERNAME].length() > 4 || postData[pd_USERNAME].length() < 20 )){
	cout << "ERROR" << DLM << "Username must be between 5 and 20 characters.";
	return 0;
}
if (postData[pd_USERNAME] == "ERROR"){
	cout << "ERROR" << DLM << "Invalid username.";
	return 0;
}
if (!( postData[pd_PASSWORD].length() > 6 || postData[pd_PASSWORD].length() < 40 )){
	cout << "ERROR" << DLM << "Password must be between 6 and 40.";
	return 0;
}
if (!( postData[pd_FIRSTNAME].length() < 20 )){
	cout << "ERROR" << DLM << "Firstname can be no more than 20 character.";
	return 0;
}
if (!( postData[pd_LASTNAME].length() < 20 )){
	cout << "ERROR" << DLM << "Firstname can be no more than 20 character.";
	return 0;
}
if (!( is_alnum(postData[pd_FIRSTNAME]) )){
	cout << "ERROR" << DLM << "Invalid First Name \"" + postData[pd_FIRSTNAME] + "\"";
	return 0;
}
if (!( is_alnum(postData[pd_LASTNAME]) )){
	cout << "ERROR" << DLM << "Invalid Last Name \"" + postData[pd_LASTNAME] + "\"";
	return 0;
}

//** Convert username to uppercase for case insensitive matching
string strConvert = postData[pd_PASSWORD],
	   strResult = strConvert;

//** Connect to DB
db.open(DIR_DB);

//** Check if the name is taken
query = "SELECT Username FROM HubUsers WHERE Username = ?";
db.prepare(query);
db.bind(1, postData[pd_USERNAME].c_str());
dbResult = db.runPrepared();
if (dbResult != DB_SUCCESS) {
	cout << "ERROR" << DLM << "Failed to run prepared query [" << dbResult << "]" << strResult << " \"" << query << "\"" << endl;
	return 0;
}
if (db.numRows() > 0){
	cout << "ERROR" << DLM << "Username is already taken." << endl;
	return 0;
}

//** Confirm validity of invite code (required user level > 1)
query = "SELECT UserID FROM HubUsers WHERE RegistrationCode = ? AND UserLevel > 1 AND BanLevel = 0 AND MuteLevel = 0";
db.prepare(query);
db.bind(1, postData[pd_REGCODE].c_str());
dbResult = db.runPrepared();
if (dbResult != DB_SUCCESS) {
	cout << "ERROR" << DLM << "Failed to run prepared query [" << dbResult << "]" << strResult << " \"" << query << "\"" << endl;
	return 0;
}
if (db.numRows() < 1){
	cout << "ERROR" << DLM << "Invalid invite code." << endl;
	return 0;
}
inviteID = db[0][0];

//** Password conversion
randStr(pwSalt, 20);
combinedPassword = postData[pd_PASSWORD] + pwSalt;
sha256Hash64(convertedPassword, combinedPassword);

//** Add user to table
query = "INSERT INTO HubUsers (Username, Password, PasswordSalt, FirstName, LastName, Email, RegistrationCode, RegistrationID, UserLevel, Color, MuteLevel, BanLevel) VALUES (?, ?, ?, ?, ?, ?, '', ?, 1, 'FFFFFF', 0, 0)";
db.prepare(query);
db.bind(1, postData[pd_USERNAME]);
db.bind(2, convertedPassword);
db.bind(3, pwSalt);
db.bind(4, postData[pd_FIRSTNAME]);
db.bind(5, postData[pd_LASTNAME]);
db.bind(6, postData[pd_EMAIL]);
db.bind(7, inviteID);
dbResult = db.runPrepared();
if (dbResult != DB_SUCCESS) {
	cout << "ERROR" << DLM << "Failed to run prepared query [" << dbResult << "] " << " \"" << query << "\"" << endl;
	return 0;
}

//** Randomize used registration key
randStr(skey, 20);
query = "UPDATE HubUsers SET RegistrationCode = ? WHERE RegistrationCode = ? LIMIT 1";
db.prepare(query);
db.bind(1, skey);
db.bind(2, postData[pd_REGCODE].c_str());
dbResult = db.runPrepared();
if (dbResult != DB_SUCCESS) {
	cout << "ERROR" << DLM << "Failed to change registration key [" << dbResult << "] " << " \"" << query << "\"" << endl;
	return 0;
}

//*** SET UP SKEY
randStr(skey, 20);

//** Create new session
string tmpLevel = "1";
dbResult = createSession(db, uid, tmpLevel, skey);
if (dbResult != DB_SUCCESS) {
	cout << "ERROR" << DLM << "Failed to create new session" << endl;
	return 0;
}

//*** INDICATE OK AND RETURN skey to the user
cout << "LOGIN" << DLM << skey << endl;
}

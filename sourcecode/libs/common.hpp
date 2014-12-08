#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <vector>
#include <string>
#include <string.h>

using namespace std;

#ifndef LIB_COMMON_HPP_
#define LIB_COMMON_HPP_

//** Global constants
const string DIR_DB = "db/hub.sqlite",
			 DLM = "_|";
			 
//** User level requirements
const int CHAT_VIEW_LEVEL = 1,			//** Getting chat updates
		  CHAT_SAY_LEVEL = 1, 			//** Adding chat messages
		  POST_GAMES_LEVEL = 2, 		//** Posting games
		  REMOVE_GAMES_LEVEL = 3,		//** Deleting other people's games
		  ADMIN_PANEL_LEVEL = 3,		//** Admin panel access level
		  ADMIN_MUTE_USER = 3,			//** Block user's ability to chat
		  ADMIN_BAN_USER = 3;			//** Ban user from accessing games

//** Methods
void toString (string &str, int num);
void toString (string &str, long num);
void toString (string &str, long long num);

string toString (int num);
string toString (long num);
string toString (long long num);

string toUpper(string &str);

bool is_number(const string& s);

bool is_alnum(const string& s);

bool is_safe(const string& s);

bool is_hex(const string& s);

void replaceInStr (string &str, const string inStr, const string repStr);

string retrievePost();

//** Method adapted from http://www.cplusplus.com/faq/sequences/strings/split/
void tokenizeStr(string &str, const string &delim, vector<string> &dest);

//** Method adapted from http://www.cplusplus.com/faq/sequences/strings/split/
void tokenizeStr(string &str, const char* delim, vector<string> &dest);

template <class A, class B>
void Swap(A a, B b);

#endif /* LIB_COMMON_HPP_ */
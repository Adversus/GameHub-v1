#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <vector>
#include <string>
#include <string.h>
#include "common.hpp"

using namespace std;

//** Methods
void toString (string &str, int num) {
	ostringstream ss;
	ss << num;
	str = ss.str();
}

void toString (string &str, long num) {
	ostringstream ss;
	ss << num;
	str = ss.str();
}

void toString (string &str, long long num) {
	ostringstream ss;
	ss << num;
	str = ss.str();
}

string toString (int num) {
	string str; 
	ostringstream ss;
	ss << num;
	str = ss.str();
	return str;
}

string toString (long num) {
	string str;
	ostringstream ss;
	ss << num;
	str = ss.str();
	return str;
}

string toString (long long num) {
	string str;
	ostringstream ss;
	ss << num;
	str = ss.str();
	return str;
}


string toUpper(string &str) {
	string strResult = "";
	for (int i=0;i<str.size();i++){
		strResult += toupper(str[i]);
	}
	return strResult;
}

bool is_number(const string& s) {
	//** Shamelessly borrowed from http://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
    string::const_iterator it = s.begin();
    while (it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool is_alnum(const string& s) {
	string::const_iterator it = s.begin();
    while (it != s.end() && isalnum(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool is_safe(const string& s) {
	string::const_iterator it = s.begin();
    while (it != s.end() && (isalnum(*it) || isspace(*it))) ++it;
    return !s.empty() && it == s.end();
}

bool is_hex(const string& s) {
	string::const_iterator it = s.begin();
    while (it != s.end() && (isxdigit(*it))) ++it;
    return !s.empty() && it == s.end();
}

void replaceInStr (string &str, const string inStr, const string repStr) {
	int loc = str.find( inStr );
	while ((loc = str.find(inStr, loc)) != string::npos){
		str.replace( loc, inStr.size(), repStr );
		loc+=repStr.size();
	};
}

string retrievePost(){
	char Buffer[1024]={0};
	char* data = getenv("CONTENT_LENGTH");
    int InputLength;
	
	//** Null error checking
	if (data == NULL) return "";
	
	//** Get size and check content
	InputLength = atoi( data );
	if (InputLength < 1) return "";

	if (InputLength > sizeof(Buffer)-1){ InputLength = sizeof(Buffer)-1; }
    fread( Buffer, InputLength, 1, stdin );
	return string(Buffer);
}

//** Method adapted from http://www.cplusplus.com/faq/sequences/strings/split/
void tokenizeStr(string &str, const string &delim, vector<string> &dest) {
	int delLen = (delim.length()-1);
	size_t current = -delLen;
	size_t next = -1;
	
	next = str.find( delim );
	do {
	  dest.push_back(str.substr( current + delLen, next - (current + delLen) ));
	  current = next + 1;
	  next = str.find( delim, current );
	} while (next != string::npos);
	
	//** Push everything left into the vector
	dest.push_back(str.substr( current + delLen));
}

//** Method adapted from http://www.cplusplus.com/faq/sequences/strings/split/
void tokenizeStr(string &str, const char* delim, vector<string> &dest) {
	int delLen = strlen(delim) - 1;
	size_t current = -delLen;
	size_t next = -1;
	
	next = str.find( delim );
	do {
	  dest.push_back(str.substr( current + delLen, next - (current + delLen) ));
	  current = next + 1;
	  next = str.find( delim, current );
	} while (next != string::npos);
	
	//** Push everything left into the vector
	dest.push_back(str.substr( current + delLen));
}

template <class A, class B>
void Swap(A a, B b) {
	A tmp = a;
	a = b;
	b = tmp;
}
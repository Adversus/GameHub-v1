#ifndef RANDSTRHASH_HPP
#define RANDSTRHASH_HPP

#include <string>

using namespace std;

void randStr(string &rstr, int slen, 
const string 
&basechars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

void sha256Hash64(string &rstr, const string &istr);

void sha256Hash16(string &rstr, const string &istr);
#endif


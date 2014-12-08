#include <iostream>
#include <crypto++/osrng.h>
#include <crypto++/sha.h>
#include <crypto++/base64.h>

#include "randStrHash.hpp"

using namespace std;

void randStr(string &rstr, int slen, const string &basechars)
{  CryptoPP::AutoSeededRandomPool rng;
   int i;
   unsigned int bCharRangeMax = basechars.length() - 1;

// *** GENERATE THE RANDOM STRING
rstr = "";
for (i = 0; i < slen; i++)
   {
   rstr += 
     basechars[static_cast<unsigned int>(rng.GenerateWord32(0, bCharRangeMax))];
   } // -- END FOR
} // -- END 

//*********************************************************************

void sha256Hash64(string &rstr, const string &istr)
{ CryptoPP::SHA256 hash;

rstr = "";

CryptoPP::StringSource foo(istr, true,
    new CryptoPP::HashFilter(hash,
      new CryptoPP::Base64Encoder (
         new CryptoPP::StringSink(rstr))));
}
    
//**********************************************************************

void sha256Hash16(string &rstr, const string &istr)
{ char cval;
  byte retAry[CryptoPP::SHA256::DIGESTSIZE];
  char cvals[] = "0123456789ABCDEF";

rstr = "";
  
CryptoPP::SHA256().CalculateDigest(retAry, (const byte *) istr.c_str(), 
                                                                 istr.size());

for (int i = 0; i < CryptoPP::SHA256::DIGESTSIZE; i++)
   {
   rstr += cvals[retAry[i] >> 4];
   rstr += cvals[retAry[i] & 0x0F];
   }
}    


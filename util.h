#ifndef UTIL_H
#define UTIL_H
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Util {
public:
    static void split(const string &s, char delim, vector<string> &elems);
    static vector<string> split(const string &s, char delim);
    static bool isNumeric(const string pszInput, int nNumberBase);
};
#endif /* UTIL_H */


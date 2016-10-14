#include <sstream>
#include"util.h"

void Util::split(const string &s, char delim, vector<string> &elems) {
    istringstream ss;
    ss.str(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

vector<string> Util::split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

bool Util::isNumeric(const string pszInput, int nNumberBase) {
    istringstream iss(pszInput);

    if (nNumberBase == 10) {
        double dTestSink;
        iss >> dTestSink;
    } else if (nNumberBase == 8 || nNumberBase == 16) {
        int nTestSink;
        iss >> ((nNumberBase == 8) ? oct : hex) >> nTestSink;
    } else
        return false;

    // was any input successfully consumed/converted?
    if (!iss)
        return false;

    // was all the input successfully consumed/converted?
    return ( iss.rdbuf()->in_avail() == 0);
}
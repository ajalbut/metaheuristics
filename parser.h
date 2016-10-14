#ifndef PARSER_H
#define PARSER_H
#include "instance.h"
#include "util.h"

class Parser {
public:
    static vector<Instance*> parseInputFile(string filePath);
};

#endif /* PARSER_H */


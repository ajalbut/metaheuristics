#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "parser.h"

class Controller {
    string inputPath;
    int heuristicType;
    float h;
    vector<Instance*> instances;
public:
    Controller(string inputPath, int heuristicType, float h);
    void run();
    void runInstance(Instance * instance);
    void print();
};

#endif /* CONTROLLER_H */


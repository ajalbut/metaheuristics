#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "parser.h"

class Controller {
    string inputPath, heuristicType;
    float h;
    vector<Instance*> instances;
public:
    Controller(string inputPath, string heuristic, float h);
    void runInstance(Instance * instance);
    void run();
    void print();
};

#endif /* CONTROLLER_H */


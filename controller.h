#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "parser.h"

class Controller {
    vector<Instance*> instances;
public:
    Controller();
    void runAll();
    void run(int heuristicType, string n, float h, bool printSchedule);
    void loadInstances(string inputPath);
    void runInstance(Instance * instance, int heuristicType);
};

#endif /* CONTROLLER_H */


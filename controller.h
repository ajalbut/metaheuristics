#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "parser.h"

class Controller {
    vector<Instance*> instances;
public:
    Controller();
    void runAll();
    void runBenchmark(int type);
    vector<int> run(int heuristicType, string n, float h,
            bool printSummary, bool printSchedule);
    void loadInstances(string inputPath);
    int runInstance(Instance * instance, int heuristicType);
};

#endif /* CONTROLLER_H */


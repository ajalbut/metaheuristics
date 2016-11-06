#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "parser.h"

class Controller {
    vector<Instance*> instances;
public:
    Controller();
    void runAll();
    void runBenchmark(int type);
    vector<vector<int> > run(int heuristicType, string n, float h,
            bool localSearch, bool printSummary, bool printSchedule);
    void loadInstances(string inputPath);
    vector<int> runInstance(Instance * instance, int heuristicType,
            bool localSearch);
};

#endif /* CONTROLLER_H */


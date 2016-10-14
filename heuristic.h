#ifndef HEURISTIC_H
#define HEURISTIC_H
#include "instance.h"

class Heuristic {
public:
    string type;
    Instance * instance;
    Heuristic(Instance * instance, string type);
    void calculateSchedule();
    void biskup1();
    void biskup2();
    int sumProcessingTimes(vector<Job*>);
    int minProcessingTime(vector<Job*>);
    vector<int> sequenceJobs();
};


#endif /* HEURISTICS_H */


#ifndef HEURISTIC_H
#define HEURISTIC_H
#include "instance.h"

class Heuristic {
public:
    Instance * instance;
    string type;
    Heuristic(Instance * instance, string type);
    void calculateSchedule();
    void biskup1();
    void biskup2();
    void construct();
    void sequenceJobsVShaped(vector<Job*> &A, vector<Job*> &B);
    int sumProcessingTimes(vector<Job*>);
    int minProcessingTime(vector<Job*>);
};


#endif /* HEURISTICS_H */


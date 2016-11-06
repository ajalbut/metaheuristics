#ifndef HEURISTIC_H
#define HEURISTIC_H
#include "instance.h"

class Heuristic {
public:
    Instance * instance;
    int type;
    vector<Job*> A, B;
    Heuristic(Instance * instance, int type);
    void calculateSchedule();
    void biskup1();
    void biskup2();
    void construct();
    void localSearch();
    void sequenceJobsVShaped(vector<Job*> &A, vector<Job*> &B);
    int sumProcessingTimes(vector<Job*>);
    bool earlyJobFits(Job* job);
};


#endif /* HEURISTICS_H */


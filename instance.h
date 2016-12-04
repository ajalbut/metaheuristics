#ifndef INSTANCE_H
#define INSTANCE_H
#include "util.h"
#include "job.h"

class Instance {
public:
    int n, d;
    float h;
    vector<Job*> jobs;
    Instance(int n);
    void calculateDueTime();
    void sequenceJobsFirstToLast(vector<Job*> jobs, int startTime);
    void sequenceJobsLastToFirst(vector<Job*> jobs, int completionTime);
    void sequenceJobsStartingFromZero();
    void printSchedule();
    int calculateTarget();
    int calculatePartialTarget(vector<Job*> J);
    vector<int> getJobSequence();
};

#endif /* INSTANCE_H */


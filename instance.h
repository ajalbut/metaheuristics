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
    void sequenceJobsFirstToLast(vector<Job*> jobs, int startTime);
    void sequenceJobsLastToFirst(vector<Job*> jobs, int completionTime);
    void calculateDueTime();
    void printScheduling();
    int calculateTarget();
};

#endif /* INSTANCE_H */


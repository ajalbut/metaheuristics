#include <algorithm>
#include <math.h>
#include "instance.h"

struct start_time_increasing {

    bool operator()(Job * a, Job * b) {
        if (a->startTime != b->startTime) {
            return a->startTime < b->startTime;
        }
        return a->id < b->id;
    }
};

Instance::Instance(int n) {
    this->n = n;
    this->jobs = vector<Job*> (n);
}

void Instance::calculateDueTime() {
    float d = 0;
    for (int i = 0; i < this->n; i++) {
        d += this->jobs[i]->processingTime;
    }
    d *= this->h;
    this->d = floor(d);
}

void Instance::sequenceJobsFirstToLast(vector<Job*> jobs, int startTime) {
    if (!jobs.size()) {
        return;
    }
    jobs[0]->startTime = startTime;
    jobs[0]->updateCompletionTime();
    for (int i = 1; i < jobs.size(); i++) {
        jobs[i]->startTime = jobs[i - 1]->completionTime;
        jobs[i]->updateCompletionTime();
    }
}

void Instance::sequenceJobsLastToFirst(vector<Job*> jobs, int completionTime) {
    if (!jobs.size()) {
        return;
    }
    int last = jobs.size() - 1;
    jobs[last]->completionTime = completionTime;
    jobs[last]->updateStartTime();
    for (int i = last; i > 0; i--) {
        jobs[i - 1]->completionTime = jobs[i]->startTime;
        jobs[i - 1]->updateStartTime();
    }
}

void Instance::sequenceJobsStartingFromZero() {
    vector<Job*> jobs = this->jobs;
    sort(jobs.begin(), jobs.end(), start_time_increasing());
    int delta = jobs[0]->startTime;
    for (int i = 0; i < this->jobs.size(); i++) {
        this->jobs[i]->startTime = this->jobs[i]->startTime - delta;
        this->jobs[i]->updateCompletionTime();
    }
}

void Instance::printSchedule() {
    for (int i = 0; i < this->n; i++) {
        Job * job = this->jobs[i];
        cout << job->id << ": start " << job->startTime
                << ", completion " << job->completionTime << endl;
    }
    cout << "JobSequence: ";
    vector<int> jobSequence = this->getJobSequence();
    for (int i = 0; i < jobSequence.size(); i++) {
        cout << jobSequence[i] << ", ";
    }
    cout << endl;
}

int Instance::calculateTarget() {
    int target = 0;
    for (int i = 0; i < this->n; i++) {
        Job * job = this->jobs[i];
        int earliness = max(this->d - job->completionTime, 0);
        int tardiness = max(job->completionTime - this->d, 0);
        target += earliness * job->earlinessPenalty;
        target += tardiness * job->tardinessPenalty;
    }
    return target;
}

int Instance::calculatePartialTarget(vector <Job*> J) {
    int target = 0;
    for (int i = 0; i < J.size(); i++) {
        Job * job = J[i];
        int earliness = max(this->d - job->completionTime, 0);
        int tardiness = max(job->completionTime - this->d, 0);
        target += earliness * job->earlinessPenalty;
        target += tardiness * job->tardinessPenalty;
    }
    return target;
}

vector<int> Instance::getJobSequence() {
    vector<int> jobSequence = vector<int>();
    vector<Job*> jobs = this->jobs;

    sort(jobs.begin(), jobs.end(), start_time_increasing());
    for (int i = 0; i < jobs.size(); i++) {
        jobSequence.push_back(jobs[i]->id);
    }
    return jobSequence;
}
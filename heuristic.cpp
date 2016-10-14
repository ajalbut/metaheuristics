#include <algorithm>
#include <stdexcept>
#include "heuristic.h"

struct proc_earl_ratios_non_increasing {

    bool operator()(Job * a, Job * b) {
        if (a->procEarlRatio != b->procEarlRatio) {
            return a->procEarlRatio > b->procEarlRatio;
        }
        return a->id > b->id;
    }
};

struct proc_tard_ratios_non_decreasing {

    bool operator()(Job * a, Job * b) {
        if (a->procTardRatio != b->procTardRatio) {
            return a->procTardRatio < b->procTardRatio;
        }
        return a->id < b->id;
    }
};

struct tardiness_earliness_non_increasing {

    bool operator()(Job * a, Job * b) {
        if (a->tardEarlRatio != b->tardEarlRatio) {
            return a->tardEarlRatio > b->tardEarlRatio;
        }
        return a->tardinessPenalty > b->tardinessPenalty;
    }
};

Heuristic::Heuristic(Instance * instance, string type) {
    this->instance = instance;
    this->type = type;
}

void Heuristic::calculateSchedule() {
    if (this->type == "biskup1") {
        this->biskup1();
    } else if (this->type == "biskup2") {
        this->biskup2();
    } else {
        throw runtime_error("Heuristic not found");
    }
}

void Heuristic::biskup1() {
    int n = this->instance->n;
    vector<Job*> A = vector<Job*> (n);
    vector<Job*> B = vector<Job*> (0);
    for (int i = 0; i < n; i++) {
        A[i] = instance->jobs[i];
    }

    bool targetDecreased = true;
    int shiftIndex;
    int lowestTarget = instance->calculateTarget();
    while (targetDecreased) {
        targetDecreased = false;
        for (int i = 0; i < A.size(); i++) {
            vector<Job*> A1 = A, B1 = B;
            Job * testShiftedJob = A[i];
            A1.erase(A1.begin() + i);
            B1.push_back(testShiftedJob);
            sort(A1.begin(), A1.end(), proc_tard_ratios_non_decreasing());
            sort(B1.begin(), B1.end(), proc_earl_ratios_non_increasing());
            instance->sequenceJobsFirstToLast(A1, instance->d);
            instance->sequenceJobsLastToFirst(B1, instance->d);
            int target = instance->calculateTarget();
            if (target < lowestTarget and
                    instance->d - Heuristic::sumProcessingTimes(B1)
                    >= Heuristic::minProcessingTime(A1)) {
                targetDecreased = true;
                lowestTarget = target;
                shiftIndex = i;
            }
        }
        if (targetDecreased) {
            Job * shiftedJob = A[shiftIndex];
            A.erase(A.begin() + shiftIndex);
            B.push_back(shiftedJob);
        }
    }
    sort(A.begin(), A.end(), proc_tard_ratios_non_decreasing());
    sort(B.begin(), B.end(), proc_earl_ratios_non_increasing());
    instance->sequenceJobsFirstToLast(A, instance->d);
    instance->sequenceJobsLastToFirst(B, instance->d);
}

void Heuristic::biskup2() {
    int n = this->instance->n, d = this->instance->d;
    vector<Job*> P = vector<Job*> (n);
    vector<Job*> A = vector<Job*> (0);
    vector<Job*> B = vector<Job*> (0);
    for (int i = 0; i < n; i++) {
        P[i] = instance->jobs[i];
    }
    sort(P.begin(), P.end(), tardiness_earliness_non_increasing());

    int shiftIndex = 0;
    while (B.size() < n / 2 and shiftIndex < P.size()) {
        if (d - Heuristic::sumProcessingTimes(B) < P[shiftIndex]->processingTime) {
            shiftIndex++;
            continue;
        }
        B.push_back(P[shiftIndex]);
        P.erase(P.begin() + shiftIndex);
        shiftIndex = 0;
    }
    for (int i = 0; i < P.size(); i++) {
        A.push_back(P[i]);
    }

    sort(A.begin(), A.end(), proc_tard_ratios_non_decreasing());
    sort(B.begin(), B.end(), proc_earl_ratios_non_increasing());
    instance->sequenceJobsFirstToLast(A, instance->d);
    instance->sequenceJobsLastToFirst(B, instance->d);

    if (B.size() < n / 2) {
        return;
    }

    int target = 0, lowestTarget = instance->calculateTarget();
    while (target <= lowestTarget) {
        vector<Job*> A1 = P, B1 = B;
        A1.erase(A1.begin());
        B1.push_back(P[0]);
        sort(A1.begin(), A1.end(), proc_tard_ratios_non_decreasing());
        sort(B1.begin(), B1.end(), proc_earl_ratios_non_increasing());
        instance->sequenceJobsFirstToLast(A1, instance->d);
        instance->sequenceJobsLastToFirst(B1, instance->d);
        target = instance->calculateTarget();
        if (target <= lowestTarget) {
            if (instance->d - Heuristic::sumProcessingTimes(B)
                < P[0]->processingTime) {
                break;
            }
            P.erase(P.begin());
            A = A1;
            B = B1;
            lowestTarget = target;
        }
    }
    sort(A.begin(), A.end(), proc_tard_ratios_non_decreasing());
    sort(B.begin(), B.end(), proc_earl_ratios_non_increasing());
    instance->sequenceJobsFirstToLast(A, instance->d);
    instance->sequenceJobsLastToFirst(B, instance->d);
}

int Heuristic::sumProcessingTimes(vector<Job*> jobs) {
    int totalTime = 0;
    for (int i = 0; i < jobs.size(); i++) {
        totalTime += jobs[i]->processingTime;
    }
    return totalTime;
};

int Heuristic::minProcessingTime(vector<Job*> jobs) {
    int minTime = 0;
    for (int i = 0; i < jobs.size(); i++) {
        if (!minTime or minTime > jobs[i]->processingTime) {
            minTime = jobs[i]->processingTime;
        }
    }
    return minTime;
}
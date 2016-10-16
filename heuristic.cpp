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

struct tard_earl_non_increasing {

    bool operator()(Job * a, Job * b) {
        if (a->tardEarlRatio != b->tardEarlRatio) {
            return a->tardEarlRatio > b->tardEarlRatio;
        }
        if (a->tardinessPenalty != b->tardinessPenalty) {
            return a->tardinessPenalty > b->tardinessPenalty;
        }
        return a->id > b->id;
    }
};

struct min_proc_ratio_non_decreasing {

    bool operator()(Job * a, Job * b) {
        float minRatioA = min(a->procEarlRatio, a->procTardRatio),
                minRatioB = min(b->procEarlRatio, b->procTardRatio);
        if (minRatioA != minRatioB) {
            return minRatioA < minRatioB;
        }
        return a->id < b->id;
    }
};

Heuristic::Heuristic(Instance * instance, int type) {
    this->instance = instance;
    this->type = type;
}

void Heuristic::calculateSchedule() {
    if (this->type == 1) {
        this->biskup1();
    } else if (this->type == 2) {
        this->biskup2();
    } else if (this->type == 3) {
        this->construct();
    } else {
        throw runtime_error("Heuristic not found");
    }
}

void Heuristic::biskup1() {
    int n = this->instance->n, d = this->instance->d;
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
            Heuristic::sequenceJobsVShaped(A1, B1);
            int target = instance->calculateTarget();
            if (target < lowestTarget and d - Heuristic::sumProcessingTimes(B1)
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
    Heuristic::sequenceJobsVShaped(A, B);
}

void Heuristic::biskup2() {
    int n = this->instance->n, d = this->instance->d;
    vector<Job*> P = vector<Job*> (n);
    vector<Job*> A = vector<Job*> (0);
    vector<Job*> B = vector<Job*> (0);
    for (int i = 0; i < n; i++) {
        P[i] = instance->jobs[i];
    }
    sort(P.begin(), P.end(), tard_earl_non_increasing());

    int shiftIndex = 0;
    while (B.size() < n / 2 and shiftIndex < P.size()) {
        if (d - Heuristic::sumProcessingTimes(B)
                < P[shiftIndex]->processingTime) {
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

    Heuristic::sequenceJobsVShaped(A, B);

    if (B.size() < n / 2) {
        return;
    }

    int target = 0, lowestTarget = instance->calculateTarget();
    while (target <= lowestTarget) {
        vector<Job*> A1 = P, B1 = B;
        A1.erase(A1.begin());
        B1.push_back(P[0]);
        Heuristic::sequenceJobsVShaped(A1, B1);
        target = instance->calculateTarget();
        if (target <= lowestTarget) {
            if (d - Heuristic::sumProcessingTimes(B) < P[0]->processingTime) {
                break;
            }
            P.erase(P.begin());
            A = A1;
            B = B1;
            lowestTarget = target;
        }
    }
    Heuristic::sequenceJobsVShaped(A, B);
}

void Heuristic::construct() {
    int n = this->instance->n, d = this->instance->d;
    vector<Job*> P = vector<Job*> (n);
    vector<Job*> A = vector<Job*> (0);
    vector<Job*> B = vector<Job*> (0);
    for (int i = 0; i < n; i++) {
        P[i] = instance->jobs[i];
    }
    sort(P.begin(), P.end(), min_proc_ratio_non_decreasing());
    while (P.size()) {
        vector<Job*> A1 = A, B1 = B;

        if (d - Heuristic::sumProcessingTimes(B) < P[0]->processingTime) {
            A.push_back(P[0]);
        } else {
            A1.push_back(P[0]);
            Heuristic::sequenceJobsVShaped(A1, B);
            A1.insert(A1.end(), B.begin(), B.end());
            int jobAfterTarget = this->instance->calculatePartialTarget(A1);

            B1.push_back(P[0]);
            Heuristic::sequenceJobsVShaped(A, B1);
            B1.insert(B1.end(), A.begin(), A.end());
            int jobBeforeTarget = this->instance->calculatePartialTarget(B1);

            if (jobAfterTarget < jobBeforeTarget) {
                A.push_back(P[0]);
            } else {
                B.push_back(P[0]);
            }
        }
        P.erase(P.begin());
    }
    Heuristic::sequenceJobsVShaped(A, B);
}

void Heuristic::sequenceJobsVShaped(vector<Job*> &A, vector<Job*> &B) {
    sort(A.begin(), A.end(), proc_tard_ratios_non_decreasing());
    sort(B.begin(), B.end(), proc_earl_ratios_non_increasing());
    this->instance->sequenceJobsFirstToLast(A, this->instance->d);
    this->instance->sequenceJobsLastToFirst(B, this->instance->d);
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
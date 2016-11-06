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

struct tard_earl_non_decreasing {

    bool operator()(Job * a, Job * b) {
        if (a->tardEarlRatio != b->tardEarlRatio) {
            return a->tardEarlRatio < b->tardEarlRatio;
        }
        if (a->tardinessPenalty != b->tardinessPenalty) {
            return a->tardinessPenalty < b->tardinessPenalty;
        }
        return a->id < b->id;
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
    this->A = vector<Job*> ();
    this->B = vector<Job*> ();
    for (int i = 0; i < this->instance->n; i++) {
        this->A.push_back(this->instance->jobs[i]);
    }

    bool targetDecreased = true;
    int shiftIndex;
    int lowestTarget = instance->calculateTarget();
    while (targetDecreased) {
        targetDecreased = false;
        for (int i = 0; i < this->A.size(); i++) {
            vector<Job*> A1 = this->A, B1 = this->B;
            if (!Heuristic::earlyJobFits(A1[i])) {
                break;
            }
            B1.push_back(A1[i]);
            A1.erase(A1.begin() + i);
            Heuristic::sequenceJobsVShaped(A1, B1);
            int target = instance->calculateTarget();
            if (target < lowestTarget) {
                targetDecreased = true;
                lowestTarget = target;
                shiftIndex = i;
            }
        }
        if (targetDecreased) {
            this->B.push_back(this->A[shiftIndex]);
            this->A.erase(this->A.begin() + shiftIndex);
        }
    }
    Heuristic::sequenceJobsVShaped(this->A, this->B);
}

void Heuristic::biskup2() {
    int n = this->instance->n;
    vector<Job*> P = vector<Job*> ();
    this->A = vector<Job*> ();
    this->B = vector<Job*> ();
    for (int i = 0; i < n; i++) {
        P.push_back(instance->jobs[i]);
    }
    sort(P.begin(), P.end(), tard_earl_non_increasing());

    int shiftIndex = 0;
    while (this->B.size() < n / 2 and shiftIndex < P.size()) {
        if (!Heuristic::earlyJobFits(P[shiftIndex])) {
            shiftIndex++;
            continue;
        }
        this->B.push_back(P[shiftIndex]);
        P.erase(P.begin() + shiftIndex);
        shiftIndex = 0;
    }
    for (int i = 0; i < P.size(); i++) {
        this->A.push_back(P[i]);
    }

    Heuristic::sequenceJobsVShaped(this->A, this->B);

    if (this->B.size() < n / 2) {
        return;
    }

    int target = 0, lowestTarget = instance->calculateTarget();
    while (target <= lowestTarget) {
        vector<Job*> A1 = P, B1 = this->B;
        A1.erase(A1.begin());
        B1.push_back(P[0]);
        Heuristic::sequenceJobsVShaped(A1, B1);
        target = instance->calculateTarget();
        if (target <= lowestTarget) {
            if (!Heuristic::earlyJobFits(P[0])) {
                break;
            }
            P.erase(P.begin());
            this->A = A1;
            this->B = B1;
            lowestTarget = target;
        }
    }
    Heuristic::sequenceJobsVShaped(this->A, this->B);
}

void Heuristic::construct() {
    vector<Job*> P = vector<Job*> ();
    this->A = vector<Job*> ();
    this->B = vector<Job*> ();
    for (int i = 0; i < this->instance->n; i++) {
        P.push_back(instance->jobs[i]);
    }
    sort(P.begin(), P.end(), min_proc_ratio_non_decreasing());
    while (P.size()) {
        vector<Job*> A1 = this->A, B1 = this->B;

        if (!Heuristic::earlyJobFits(P[0])) {
            this->A.push_back(P[0]);
        } else {
            A1.push_back(P[0]);
            Heuristic::sequenceJobsVShaped(A1, this->B);
            A1.insert(A1.end(), this->B.begin(), this->B.end());
            int jobAfterTarget = this->instance->calculatePartialTarget(A1);

            B1.push_back(P[0]);
            Heuristic::sequenceJobsVShaped(this->A, B1);
            B1.insert(B1.end(), this->A.begin(), this->A.end());
            int jobBeforeTarget = this->instance->calculatePartialTarget(B1);

            if (jobAfterTarget < jobBeforeTarget) {
                this->A.push_back(P[0]);
            } else {
                this->B.push_back(P[0]);
            }
        }
        P.erase(P.begin());
    }
    Heuristic::sequenceJobsVShaped(this->A, this->B);
}

void Heuristic::localSearch() {
    int lowestTarget = instance->calculateTarget();
    sort(this->A.begin(), this->A.end(), tard_earl_non_increasing());
    sort(this->B.begin(), this->B.end(), tard_earl_non_decreasing());

    int i = 0;
    while (i < max(A.size(), B.size())) {
        vector<Job*> A1 = this->A, B1 = this->B;
        if (B1.size() > i) {
            A1.push_back(B1[i]);
            B1.erase(B1.begin() + i);
            Heuristic::sequenceJobsVShaped(A1, B1);
            int target = this->instance->calculateTarget();
            if (target < lowestTarget) {
                this->A = A1;
                this->B = B1;
                sort(this->A.begin(),
                        this->A.end(), tard_earl_non_increasing());
                sort(this->B.begin(),
                        this->B.end(), tard_earl_non_decreasing());
                lowestTarget = target;
                i--;
            }
        }

        A1 = this->A, B1 = this->B;
        if (A1.size() > i and Heuristic::earlyJobFits(A1[i])) {
            B1.push_back(A1[i]);
            A1.erase(A1.begin() + i);
            Heuristic::sequenceJobsVShaped(A1, B1);
            int target = this->instance->calculateTarget();
            if (target < lowestTarget) {
                this->A = A1;
                this->B = B1;
                sort(this->A.begin(),
                        this->A.end(), tard_earl_non_increasing());
                sort(this->B.begin(),
                        this->B.end(), tard_earl_non_decreasing());
                lowestTarget = target;
                i--;
            }
        }
        i++;
    }
    Heuristic::sequenceJobsVShaped(this->A, this->B);
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

bool Heuristic::earlyJobFits(Job* job) {
    return this->instance->d - Heuristic::sumProcessingTimes(this->B)
            >= job->processingTime;
}
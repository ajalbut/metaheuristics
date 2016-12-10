#include <algorithm>
#include <stdexcept>
#include <math.h>
#include "heuristic.h"

struct proc_earl_ratios_non_incr {

    bool operator()(Job * a, Job * b) {
        if (a->procEarlRatio != b->procEarlRatio) {
            return a->procEarlRatio > b->procEarlRatio;
        }
        return a->id > b->id;
    }
};

struct proc_tard_ratios_non_decr {

    bool operator()(Job * a, Job * b) {
        if (a->procTardRatio != b->procTardRatio) {
            return a->procTardRatio < b->procTardRatio;
        }
        return a->id < b->id;
    }
};

struct tard_earl_non_incr {

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

struct tard_earl_non_decr {

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

struct min_proc_ratio_non_decr {

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
            if (!this->earlyJobFits(A1[i], this->B)) {
                break;
            }
            B1.push_back(A1[i]);
            A1.erase(A1.begin() + i);
            this->sequenceJobsVShaped(A1, B1);
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
    this->sequenceJobsVShaped(this->A, this->B);
}

void Heuristic::biskup2() {
    int n = this->instance->n;
    vector<Job*> P = vector<Job*> ();
    this->A = vector<Job*> ();
    this->B = vector<Job*> ();
    for (int i = 0; i < n; i++) {
        P.push_back(instance->jobs[i]);
    }
    sort(P.begin(), P.end(), tard_earl_non_incr());

    int shiftIndex = 0;
    while (this->B.size() < n / 2 and shiftIndex < P.size()) {
        if (!this->earlyJobFits(P[shiftIndex], this->B)) {
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

    this->sequenceJobsVShaped(this->A, this->B);

    if (this->B.size() < n / 2) {
        return;
    }

    int target = 0, lowestTarget = instance->calculateTarget();
    while (target <= lowestTarget) {
        vector<Job*> A1 = P, B1 = this->B;
        A1.erase(A1.begin());
        B1.push_back(P[0]);
        this->sequenceJobsVShaped(A1, B1);
        target = instance->calculateTarget();
        if (target <= lowestTarget) {
            if (!this->earlyJobFits(P[0], this->B)) {
                break;
            }
            P.erase(P.begin());
            this->A = A1;
            this->B = B1;
            lowestTarget = target;
        }
    }
    this->sequenceJobsVShaped(this->A, this->B);
}

void Heuristic::construct() {
    vector<Job*> P = vector<Job*> ();
    this->A = vector<Job*> ();
    this->B = vector<Job*> ();
    for (int i = 0; i < this->instance->n; i++) {
        P.push_back(instance->jobs[i]);
    }
    sort(P.begin(), P.end(), min_proc_ratio_non_decr());
    while (P.size()) {
        vector<Job*> A1 = this->A, B1 = this->B;

        if (!this->earlyJobFits(P[0], this->B)) {
            this->A.push_back(P[0]);
        } else {
            A1.push_back(P[0]);
            this->sequenceJobsVShaped(A1, this->B);
            A1.insert(A1.end(), this->B.begin(), this->B.end());
            int jobAfterTarget = this->instance->calculatePartialTarget(A1);

            B1.push_back(P[0]);
            this->sequenceJobsVShaped(this->A, B1);
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
    this->sequenceJobsVShaped(this->A, this->B);
}

void Heuristic::localSearch() {
    int lowestTarget = this->calculateLowestShiftedTarget(this->A, this->B);
    int i = 0;
    bool AtoB = true;
    while (i < max(this->A.size(), this->B.size())) {
        if (i == 0 and AtoB) {
            sort(this->A.begin(), this->A.end(), tard_earl_non_incr());
            sort(this->B.begin(), this->B.end(), tard_earl_non_decr());
        }
        AtoB = !AtoB;
        vector<Job*> A1 = this->A, B1 = this->B;
        if (!AtoB and B1.size() > i) {
            A1.push_back(B1[i]);
            B1.erase(B1.begin() + i);
        } else if (AtoB and A1.size() > i
                and this->earlyJobFits(A1[i], this->B)) {
            B1.push_back(A1[i]);
            A1.erase(A1.begin() + i);
        } else {
            if (AtoB) i++;
            continue;
        }
        int target = this->calculateLowestShiftedTarget(A1, B1);
        if (target < lowestTarget) {
            this->A = A1;
            this->B = B1;
            lowestTarget = target;
            i = 0;
            AtoB = true;
            continue;
        }
        if (AtoB) i++;
    }
    this->calculateLowestShiftedTarget(this->A, this->B);
}

void Heuristic::tabuSearch() {
    vector<vector<int> > tabuList = vector<vector<int> >();
    int lowestTarget = this->calculateLowestShiftedTarget(this->A, this->B);
    vector<Job*> bestA = this->A, bestB = this->B;
    int skip = 0;
    bool lock = false, end = false;
    while (!end) {
        if (!lock and (tabuList.size() > 1000
                or skip >= max(this->A.size(), this->B.size()))) {
            skip = 0;
            lock = true;
            this->A = bestA;
            this->B = bestB;
        }
        int i = skip;
        bool AtoB = true;
        while (i < max(this->A.size(), this->B.size())) {
            if (i == skip and AtoB) {
                sort(this->A.begin(), this->A.end(), tard_earl_non_incr());
                sort(this->B.begin(), this->B.end(), tard_earl_non_decr());
            }
            AtoB = !AtoB;
            vector<Job*> A1 = this->A, B1 = this->B;
            if (!AtoB and B1.size() > i) {
                A1.push_back(B1[i]);
                B1.erase(B1.begin() + i);
            } else if (AtoB and A1.size() > i
                    and this->earlyJobFits(A1[i], this->B)) {
                B1.push_back(A1[i]);
                A1.erase(A1.begin() + i);
            } else {
                if (AtoB) i++;
                continue;
            }
            int target = this->calculateLowestShiftedTarget(A1, B1);
            if (target < lowestTarget) {
                lowestTarget = target;
                bestA = this->A = A1;
                bestB = this->B = B1;
                if (lock) {
                    i = 0;
                    AtoB = true;
                    continue;
                } else {
                    tabuList.push_back(this->instance->getJobSequence());
                    skip = 0;
                    break;
                }
            } else if (!lock and !this->currentSequenceIsTabu(tabuList)) {
                this->A = A1;
                this->B = B1;
                tabuList.push_back(this->instance->getJobSequence());
                break;
            }
            if (AtoB) i++;
        }
        if (i >= max(this->A.size(), this->B.size())) {
            if (lock) {
                end = true;
            } else {
                tabuList.clear();
                skip++;
            }
        }
    }
    this->calculateLowestShiftedTarget(bestA, bestB);
}

void Heuristic::sequenceJobsVShaped(vector<Job*> &A, vector<Job*> &B) {
    sort(A.begin(), A.end(), proc_tard_ratios_non_decr());
    sort(B.begin(), B.end(), proc_earl_ratios_non_incr());
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

bool Heuristic::earlyJobFits(Job* job, vector<Job*> &B) {
    return this->instance->d - this->sumProcessingTimes(B)
            >= job->processingTime;
}

int Heuristic::calculateLowestShiftedTarget(vector<Job*> &A, vector<Job*> &B) {
    this->sequenceJobsVShaped(A, B);
    int unshiftedTarget = this->instance->calculateTarget();

    this->instance->sequenceJobsStartingFromZero();
    int shiftedTarget = this->instance->calculateTarget();

    if (shiftedTarget < unshiftedTarget) {
        return shiftedTarget;
    } else {
        this->sequenceJobsVShaped(A, B);
        return unshiftedTarget;
    }
}

bool Heuristic::currentSequenceIsTabu(vector<vector<int> >tabuList) {
    vector<int> jobSequence = this->instance->getJobSequence();
    for (int i = 0; i < tabuList.size(); i++) {
        int isTabu = true;
        vector<int> tabuSequence = tabuList[i];
        for (int j = 0; j < tabuSequence.size(); j++) {
            if (jobSequence[j] != tabuSequence[j]) {
                isTabu = false;
            }
        }
        if (isTabu) {
            return true;
        }
    }
    return false;
}
#include "job.h"

Job::Job(int n, int processingTime, int earlinessPenalty, int tardinessPenalty) {
    this->id = n + 1;
    this->processingTime = processingTime;
    this->earlinessPenalty = earlinessPenalty;
    this->tardinessPenalty = tardinessPenalty;
    this->procEarlRatio =
            (float) this->processingTime / this->earlinessPenalty;
    this->procTardRatio =
            (float) this->processingTime / this->tardinessPenalty;
    this->tardEarlRatio =
            (float) this->tardinessPenalty / this->earlinessPenalty;
    this->startTime = 0;
    this->completionTime = 0;
}

void Job::updateStartTime() {
    this->startTime = this->completionTime - this->processingTime;
}

void Job::updateCompletionTime() {
    this->completionTime = this->startTime + this->processingTime;
}

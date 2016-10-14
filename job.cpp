#include "job.h"

Job::Job(int n){
    this->id = n + 1;
    this->startTime = 0;
    this->completionTime = 0;
}

void Job::updateStartTime() {
    this->startTime = this->completionTime - this->processingTime;
}

void Job::updateCompletionTime() {
    this->completionTime = this->startTime + this->processingTime;
}


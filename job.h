#ifndef TASK_H
#define TASK_H

class Job {
public:
    int id, processingTime, earlinessPenalty,
    tardinessPenalty, startTime, completionTime;
    float procEarlRatio, procTardRatio, tardEarlRatio;
    Job(int n, int processingTime, int earlinessPenalty, int tardinessPenalty);
    void updateStartTime();
    void updateCompletionTime();
};


#endif /* TASK_H */


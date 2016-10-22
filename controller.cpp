#include "heuristic.h"
#include "controller.h"

Controller::Controller() {
}

void Controller::runAll() {
    int hT[3] = {1, 2, 3};
    vector<int> heuristicTypes(hT, hT + 3);
    string nV[7] = {"10", "20", "50", "100", "200", "500", "1000"};
    vector<string> nValues(nV, nV + 7);
    float hV[4] = {0.2, 0.4, 0.6, 0.8};
    vector<float> hValues(hV, hV + 4);

    for (int i = 0; i < nValues.size(); i++) {
        for (int j = 0; j < hValues.size(); j++) {
            for (int k = 0; k < heuristicTypes.size(); k++) {
                this->run(
                        heuristicTypes[k], nValues[i], hValues[j],
                        true, false
                        );
            }
        }
    }
}

void Controller::runBenchmark(int type) {
    int heuristicType = -type;
    string nV[7] = {"10", "20", "50", "100", "200", "500", "1000"};
    vector<string> nValues(nV, nV + 7);
    float hV[4] = {0.2, 0.4, 0.6, 0.8};
    vector<float> hValues(hV, hV + 4);

    vector<int> actualTargets = vector<int> (10);
    for (int i = 0; i < nValues.size(); i++) {
        string inputPath = "upper_bound_" + nValues[i] + ".txt";
        vector<vector<int> > upperBoundTargets =
                Parser::parseUpperBoundFile(inputPath);
        for (int j = 0; j < hValues.size(); j++) {
            int upperBoundSum = 0, actualSum = 0;
            actualTargets = this->run(
                    heuristicType, nValues[i], hValues[j],
                    false, false);
            for (int k = 0; k < actualTargets.size(); k++) {
                upperBoundSum += upperBoundTargets[k][j];
                actualSum += actualTargets[k];
            }
            float percentage =
                    (float) (actualSum - upperBoundSum) * 100 / upperBoundSum;
            cout << "Benchmark for n = " << nValues[i]
                    << ", h = " << hValues[j]
                    << ": upper bound sum = " << upperBoundSum
                    << ", actual sum = " << actualSum
                    << ", difference = "
                    << percentage << "%" << endl;
        }
    }
}

vector<int> Controller::run(
        int heuristicType, string n, float h,
        bool printSummary, bool printSchedule
        ) {
    string inputPath = "sch" + n + ".txt";
    this->loadInstances(inputPath);
    vector<int> targets = vector<int>(this->instances.size());

    int s = this->instances.size();
    if (printSummary) {
        cout << "Running heuristic type " << heuristicType
                << " for " << s << " instances with n = "
                << n << " and h = " << h << endl;
    }

    for (int i = 0; i < s; i++) {
        Instance * instance = this->instances[i];
        instance->h = h;
        instance->calculateDueTime();
        if (printSummary) {
            cout << "Instance " << i + 1 << ":" << endl;
            cout << "Due time: " << instance->d << endl;
        }

        targets[i] = this->runInstance(instance, heuristicType);
        if (printSchedule) {
            instance->printSchedule();
        }

        if (printSummary) {
            cout << "Target: " << targets[i] << endl;
            cout << endl;
        }
    }
    return targets;
}

void Controller::loadInstances(string inputPath) {
    this->instances = Parser::parseInputFile(inputPath);
}

int Controller::runInstance(Instance * instance, int heuristicType) {
    Heuristic * heuristic = new Heuristic(instance, heuristicType);
    heuristic->calculateSchedule();
    return instance->calculateTarget();
}

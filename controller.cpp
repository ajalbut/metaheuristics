#include <algorithm>

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
                        true, true, false
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

    vector<vector<int> > runTargets = vector<vector<int> > ();
    float u_a_t_percentage = 0, a_i_t_percentage = 0, u_i_t_percentage = 0;
    for (int i = 0; i < nValues.size(); i++) {
        string inputPath = "upper_bound_" + nValues[i] + ".txt";
        vector<vector<int> > upperBoundTargets =
                Parser::parseUpperBoundFile(inputPath);
        for (int j = 0; j < hValues.size(); j++) {
            int upperBoundSum = 0, actualSum = 0, improvedSum = 0;
            runTargets = this->run(
                    heuristicType, nValues[i], hValues[j],
                    true, false, false);
            for (int k = 0; k < runTargets.size(); k++) {
                upperBoundSum += upperBoundTargets[k][j];
                actualSum += runTargets[k][0];
                improvedSum += runTargets[k][1];
            }
            float u_a_percentage =
                    (float) (actualSum - upperBoundSum) * 100 / upperBoundSum,
                    a_i_percentage =
                    (float) (improvedSum - actualSum) * 100 / actualSum,
                    u_i_percentage =
                    (float) (improvedSum - upperBoundSum) * 100 / upperBoundSum;
            cout << "Benchmark for n = " << nValues[i]
                    << ", h = " << hValues[j]
                    << ": upper bound sum = " << upperBoundSum
                    << ", actual sum = " << actualSum
                    << ", improved sum = " << improvedSum
                    << ", upper to actual = " << u_a_percentage << "%"
                    << ", actual to improved = " << a_i_percentage << "%"
                    << ", upper to improved = " << u_i_percentage << "%"
                    << endl;
            u_a_t_percentage += u_a_percentage;
            a_i_t_percentage += a_i_percentage;
            u_i_t_percentage += u_i_percentage;
        }
    }

    u_a_t_percentage /= 28.0;
    a_i_t_percentage /= 28.0;
    u_i_t_percentage /= 28.0;
    cout << "Final results: upper to actual = " << u_a_t_percentage << "%"
            << ", actual to improved = " << a_i_t_percentage << "%"
            << ", upper to improved = " << u_i_t_percentage << "%"
            << endl;
}

vector<vector<int> > Controller::run(
        int heuristicType, string n, float h, bool localSearch,
        bool printSummary, bool printSchedule
        ) {
    string inputPath = "sch" + n + ".txt";
    this->loadInstances(inputPath);
    vector<vector<int> > targets = vector<vector<int> >();

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

        targets.push_back(this->runInstance(instance, heuristicType,
                localSearch));
        if (printSchedule) {
            instance->printSchedule();
        }

        if (printSummary) {
            cout << "Target: " << targets[i][0] << endl;
            if (localSearch) {

                cout << "Improved: " << targets[i][1] << endl;
            }
            cout << endl;
        }
    }
    return targets;
}

void Controller::loadInstances(string inputPath) {

    this->instances = Parser::parseInputFile(inputPath);
}

vector<int> Controller::runInstance(Instance * instance, int heuristicType,
        bool localSearch) {
    vector<int> targets = vector<int>();
    Heuristic * heuristic = new Heuristic(instance, heuristicType);
    heuristic->calculateSchedule();
    targets.push_back(instance->calculateTarget());
    if (localSearch) {
        heuristic->localSearch();
        targets.push_back(instance->calculateTarget());
    }
    return targets;
}

#include <algorithm>
#include <ctime>
#include "heuristic.h"
#include "controller.h"

Controller::Controller() {
}

void Controller::runAll() {
    int hT[4] = {1, 2, 3, 4};
    vector<int> heuristicTypes(hT, hT + 4);
    string nV[7] = {"10", "20", "50", "100", "200", "500", "1000"};
    vector<string> nValues(nV, nV + 7);
    float hV[4] = {0.2, 0.4, 0.6, 0.8};
    vector<float> hValues(hV, hV + 4);

    for (int i = 0; i < nValues.size(); i++) {
        for (int j = 0; j < hValues.size(); j++) {
            for (int k = 0; k < heuristicTypes.size(); k++) {
                this->run(
                        heuristicTypes[k], nValues[i], hValues[j],
                        true, true, true, false
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
    float u_a_t_percentage = 0, a_l_t_percentage = 0, u_l_t_percentage = 0,
            l_t_t_percentage = 0, u_t_t_percentage = 0;
    for (int i = 0; i < nValues.size(); i++) {
        string inputPath = "upper_bound_" + nValues[i] + ".txt";
        vector<vector<int> > upperBoundTargets =
                Parser::parseUpperBoundFile(inputPath);
        for (int j = 0; j < hValues.size(); j++) {
            int upperBoundSum = 0, actualSum = 0, localSum = 0, tabuSum = 0;
            runTargets = this->run(
                    heuristicType, nValues[i], hValues[j],
                    true, true, false, false);
            for (int k = 0; k < runTargets.size(); k++) {
                upperBoundSum += upperBoundTargets[k][j];
                actualSum += runTargets[k][0];
                localSum += runTargets[k][1];
                tabuSum += runTargets[k][2];
            }
            float u_a_percentage =
                    (float) (actualSum - upperBoundSum) * 100 / upperBoundSum,
                    a_l_percentage =
                    (float) (localSum - actualSum) * 100 / actualSum,
                    u_l_percentage =
                    (float) (localSum - upperBoundSum) * 100 / upperBoundSum,
                    l_t_percentage =
                    (float) (tabuSum - localSum) * 100 / actualSum,
                    u_t_percentage =
                    (float) (tabuSum - upperBoundSum) * 100 / upperBoundSum;
            cout << "Benchmark for n = " << nValues[i]
                    << ", h = " << hValues[j]
                    << ": upper bound sum = " << upperBoundSum
                    << ", actual sum = " << actualSum
                    << ", local sum = " << localSum
                    << ", tabu sum = " << tabuSum
                    << ", upper to actual = " << u_a_percentage << "%"
                    << ", actual to local = " << a_l_percentage << "%"
                    << ", upper to local = " << u_l_percentage << "%"
                    << ", local to tabu = " << l_t_percentage << "%"
                    << ", upper to tabu = " << u_t_percentage << "%"
                    << endl;
            u_a_t_percentage += u_a_percentage;
            a_l_t_percentage += a_l_percentage;
            u_l_t_percentage += u_l_percentage;
            l_t_t_percentage += l_t_percentage;
            u_t_t_percentage += u_t_percentage;
        }
    }

    u_a_t_percentage /= 28.0;
    a_l_t_percentage /= 28.0;
    u_l_t_percentage /= 28.0;
    l_t_t_percentage /= 28.0;
    u_t_t_percentage /= 28.0;
    cout << "Final results: upper to actual = " << u_a_t_percentage << "%"
            << ", actual to local = " << a_l_t_percentage << "%"
            << ", upper to local = " << u_l_t_percentage << "%"
            << ", local to tabu = " << l_t_t_percentage << "%"
            << ", upper to tabu = " << u_t_t_percentage << "%"
            << endl;
}

vector<vector<int> > Controller::run(
        int heuristicType, string n, float h, bool localSearch, bool tabuSearch,
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
                localSearch, tabuSearch));
        if (printSchedule) {
            instance->printSchedule();
        }

        if (printSummary) {
            cout << "Target: " << targets[i][0] << endl;
            if (localSearch) {
                cout << "With local search: " << targets[i][1] << endl;
            }
            if (localSearch) {
                cout << "With tabu search: " << targets[i][2] << endl;
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
        bool localSearch, bool tabuSearch) {
    clock_t start, heur, local, tabu;

    vector<int> targets = vector<int>();
    Heuristic * heuristic = new Heuristic(instance, heuristicType);

    start = clock();
    heuristic->calculateSchedule();
    targets.push_back(instance->calculateTarget());
    heur = clock();
    cout << "Heuristic time for n = " << instance->n
            << ", h = " << instance->h << ": "
            << (heur - start) / (double) (CLOCKS_PER_SEC / 1000)
            << " ms" << std::endl;

    if (localSearch) {
        heuristic->localSearch();
        targets.push_back(instance->calculateTarget());
        local = clock();

        cout << "Local search time for n = " << instance->n
                << ", h = " << instance->h << ": "
                << (local - heur) / (double) (CLOCKS_PER_SEC / 1000)
                << " ms" << std::endl;
    }
    if (tabuSearch) {
        heuristic->tabuSearch();
        targets.push_back(instance->calculateTarget());
        tabu = clock();

        cout << "Tabu search time for n = " << instance->n
                << ", h = " << instance->h << ": "
                << (tabu - local) / (double) (CLOCKS_PER_SEC / 1000)
                << " ms" << std::endl;
    }
    return targets;
}

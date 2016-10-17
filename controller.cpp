#include "heuristic.h"
#include "controller.h"

Controller::Controller() {
}

void Controller::runAll() {
    int hT[3] = {1, 2, 3};
    vector<int> heuristicTypes(hT, hT+3);
    string nV[7] = {"10", "20", "50", "100", "200", "500", "1000"};        
    vector<string> nValues (nV, nV + 7);
    float hV[4] = {0.2, 0.4, 0.6, 0.8};
    vector<float> hValues (hV, hV + 4);
    
    for (int i = 0; i < nValues.size(); i++) {
        for (int j = 0; j < hValues.size(); j++) {
            for (int k = 0; k < heuristicTypes.size(); k++) {
                this->run(heuristicTypes[k], nValues[i], hValues[j], false);
            }
        }
    }
}

void Controller::run(int heuristicType, string n, float h, bool printSchedule) {
    string inputPath = "sch" + n + ".txt";
    this->loadInstances(inputPath);

    int s = this->instances.size();
    cout << "Running heuristic type " << heuristicType
            << " for " << s << " instances with n = "
            << n << " and h = " << h << endl;

    for (int i = 0; i < s; i++) {
        Instance * instance = this->instances[i];
        cout << "Instance " << i + 1 << ":" << endl;
        
        instance->h = h;
        instance->calculateDueTime();
        cout << "Due time: " << instance->d << endl;
        
        this->runInstance(instance, heuristicType);
        if (printSchedule) {
            instance->printSchedule();
        }
        cout << "Target: " << instance->calculateTarget() << endl;
        cout << endl;
    }
}

void Controller::loadInstances(string inputPath) {
    this->instances = Parser::parseInputFile(inputPath);
}

void Controller::runInstance(Instance * instance, int heuristicType) {
    Heuristic * heuristic = new Heuristic(instance, heuristicType);
    heuristic->calculateSchedule();
}
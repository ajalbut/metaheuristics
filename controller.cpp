#include "heuristic.h"
#include "controller.h"

Controller::Controller(string inputPath, string heuristicType, float h) {
    this->instances = Parser::parseInputFile(inputPath);
    this->heuristicType = heuristicType;
    this->h = h;
}

void Controller::run() {
    for (int i = 0; i< this->instances.size(); i++) {
        Instance * instance = this->instances[i];
        instance->h = this->h;
        instance->calculateDueTime();
        this->runInstance(instance);
    }
    this->print();
}

void Controller::runInstance(Instance * instance) {
    Heuristic * heuristic = new Heuristic(instance, this->heuristicType);
    heuristic->calculateSchedule();
}

void Controller::print() {
    for (int i = 0; i< this->instances.size(); i++) {
        Instance * instance = this->instances[i];
        cout << "Instance " << i << ":" << endl;
        cout << "Due date: " << instance->d << endl;
        instance->printScheduling();
        cout << "Target: " << instance->calculateTarget() << endl;
        cout << endl;
    }
}

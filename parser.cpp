#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include "parser.h"

vector<Instance*> Parser::parseInputFile(string filePath) {
    ifstream inputFile;
    char str[255];
    vector<string> instanceCountLine, machineCountLine, parameterLine;
    int instanceCount, machineCount;
    vector<Instance*> instances;

    inputFile.open(filePath.c_str());
    inputFile.getline(str, 255);
    instanceCountLine = Util::split(str, ' ');
    for (int i = 0; i < instanceCountLine.size(); i++) {
        if (!Util::isNumeric(instanceCountLine[i], 10)) {
            continue;
        }
        instanceCount = atoi(instanceCountLine[i].c_str());
        instances = vector<Instance*> (instanceCount);
        for (int j = 0; j < instanceCount; j++) {
            inputFile.getline(str, 255);
            machineCountLine = Util::split(str, ' ');
            for (int k = 0; k < machineCountLine.size(); k++) {
                if (!Util::isNumeric(machineCountLine[k], 10)) {
                    continue;
                }
                machineCount = atoi(machineCountLine[k].c_str());
                Instance * instance = new Instance(machineCount);
                for (int l = 0; l < machineCount; l++) {
                    inputFile.getline(str, 255);
                    parameterLine = Util::split(str, ' ');
                    int switcher = 0;
                    int proc, earl, tard;
                    for (int m = 0; m < parameterLine.size(); m++) {
                        if (!Util::isNumeric(parameterLine[m], 10)) {
                            continue;
                        }
                        int parameter = atoi(parameterLine[m].c_str());
                        switch (switcher) {
                            case 0:
                                proc = parameter;
                                break;
                            case 1:
                                earl = parameter;
                                break;
                            case 2:
                                tard = parameter;
                                break;
                            default:
                                throw runtime_error("Excess parameters");
                                break;
                        }
                        switcher++;
                    }
                    Job * job = new Job(l, proc, earl, tard);
                    instance->jobs[l] = job;
                }
                instances[j] = instance;
            }
        }
    }
    inputFile.close();

    return instances;
}

vector<vector<int> > Parser::parseUpperBoundFile(string filePath) {
    ifstream inputFile;
    char str[255];
    vector<string> instanceLine;
    vector<vector<int> > upperBounds;

    inputFile.open(filePath.c_str());
    while (inputFile.getline(str, 255)) {
        instanceLine = Util::split(str, ' ');
        vector<int> instanceUpperBounds;
        for (int i = 0; i < instanceLine.size(); i++) {
            if (!Util::isNumeric(instanceLine[i], 10)) {
                continue;
            }
            instanceUpperBounds.push_back(atoi(instanceLine[i].c_str()));
        }
        upperBounds.push_back(instanceUpperBounds);
    }
    inputFile.close();

    return upperBounds;
}

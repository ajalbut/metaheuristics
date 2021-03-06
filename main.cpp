#include <cstdlib>
#include "controller.h"

int main(int argc, char** argv) {
    int heuristicType;
    string n;
    float h;
    if (argc > 1) {
        heuristicType = atoi(argv[1]);
    } else {
        cout << "Please enter heuristic type to be used:" << endl;
        cout << "[0]: Comparative test mode" << endl;
        cout << "[1]: Biskup/Feldmann 1" << endl;
        cout << "[2]: Biskup/Feldmann 2" << endl;
        cout << "[3]: Constructive - min(proc/earl, proc/tard)" << endl;
        cout << "[-1]: Benchmark mode for [1] with local search" << endl;
        cout << "[-2]: Benchmark mode for [2] with local search" << endl;
        cout << "[-3]: Benchmark mode for [3] with local search" << endl;
        cout << "[-10]: Benchmark mode for [1] with tabu search" << endl;
        cout << "[-20]: Benchmark mode for [2] with tabu search" << endl;
        cout << "[-30]: Benchmark mode for [3] with tabu search" << endl;
        cin >> heuristicType;
    }

    if (argc > 2) {
        n = argv[2];
    } else if (heuristicType > 0) {
        cout << "Please enter n value:" << endl;
        cin >> n;
    }
    if (argc > 3) {
        h = atof(argv[3]);
    } else if (heuristicType > 0) {
        cout << "Please enter h value:" << endl;
        cin >> h;
    }

    Controller * controller = new Controller();
    if (heuristicType == 0) {
        controller->runAll();
    } else if (heuristicType < -9) {
        controller->runBenchmark(heuristicType, true);
    } else if (heuristicType < 0) {
        controller->runBenchmark(heuristicType, false);
    } else {
        controller->run(heuristicType, n, h, false, true, true, true);
    }

    return 0;
}


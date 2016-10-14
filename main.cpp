#include "controller.h"

int main(int argc, char** argv) {
    Controller * controller = new Controller("sch10.txt", "biskup1", 0.8);
    controller->run();
    return 0;
}


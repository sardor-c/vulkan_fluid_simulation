#include "fluid_sim_2d.h"

#include <iostream>
#include <stdexcept>
#include "utils.h"


int main() {
    FluidSimulation2D fluidSimulation2D;
    try {
        fluidSimulation2D.run();
    }
    catch (const std::exception& e) {
        std::cerr << utils::createErrorLog(e.what());
        return 1;
    }
    return 0;
}
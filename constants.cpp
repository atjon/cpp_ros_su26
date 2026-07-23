#include <iostream>
#include "constants.h"

int main() {

    std::cout << "Using constants::pi, via the included header file: " <<
    "where the constants are forward declared and are externally linked together" << 
    constants::pi << "\n";
    
    return 0;
}
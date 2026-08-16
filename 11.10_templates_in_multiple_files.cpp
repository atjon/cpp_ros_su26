#include "11.10_templates.h"
#include <string_view>

// 11.10 - Using function templates in multiple files (the MAIN .cpp)
//
// This target is built from TWO .cpp files plus one header:
//   11.10_templates_in_multiple_files.cpp  (this file, has main)
//   11.10_telemetry.cpp                    (a second translation unit)
//   11.10_templates.h                      (the template DEFINITIONS)
//
// See CMakeLists.txt -- both .cpp files are listed in the same add_executable,
// the same way linkage.cpp and linkageCopy.cpp are.

// normal forward declaration for a normal (non-template) function.
// This one CAN live in a .cpp, because the linker only needs to find the
// symbol -- it doesn't need to generate anything.
void runTelemetry();

int main() {

    std::cout << "main() -- instantiated in this .cpp:\n";

    // these instantiate larger<int> and larger<double> in THIS translation unit
    std::cout << "  larger(4, 9)       = " << larger(4, 9) << "\n";
    std::cout << "  larger(1.5, 0.25)  = " << larger(1.5, 0.25) << "\n";

    printReading("heading deg", 275);

    std::cout << "\n";

    // this function lives in the other .cpp and makes its OWN instantiations
    runTelemetry();

    std::cout << "\nBoth .cpp files instantiated larger<int> independently.\n"
                 "The linker merged the duplicates instead of erroring, because\n"
                 "templates are implicitly inline.\n";

    return 0;
}

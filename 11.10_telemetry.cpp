#include "11.10_templates.h" // full template definitions come along with it
#include <string_view>

// 11.10 - Using function templates in multiple files (the SECOND .cpp)
//
// This is a completely separate translation unit from the one holding main().
// It includes the same header, so the compiler can stamp out its OWN copies of
// larger<int> and printReading<double> right here.
//
// That means the object file for THIS .cpp and the object file for the main
// .cpp can both contain larger<int>. The linker dedupes them instead of
// complaining about a duplicate symbol -- exactly the behavior that makes
// header-defined templates legal.
//
// Try it: delete the template bodies out of the header and leave only
//   template <typename T> T larger(T a, T b);
// and this file will still COMPILE but the program will fail to LINK.

void runTelemetry() {
    std::cout << "runTelemetry() -- instantiated in a different .cpp:\n";

    printReading("battery volts", 11.85);   // instantiates printReading<double> HERE
    printReading("motor ticks", 3400);      // instantiates printReading<int> HERE

    std::cout << "  peak of (18, 42) = " << larger(18, 42) << "\n"; // larger<int> HERE
}

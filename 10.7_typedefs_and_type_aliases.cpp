#include <iostream>
#include <vector>

// 10.7 - Typedefs and type aliases
//
// A type alias is just a NEW NAME for an EXISTING type.
// It does NOT create a new type -- the compiler still sees the original.
// So you gain readability, but ZERO extra type safety.

// ------------------------- THE OLD WAY: typedef ------------------------------

// typedef reads BACKWARDS compared to a normal declaration:
// the new name goes LAST, which is exactly why it's easy to misread.
typedef double distance_meters_t; // "distance_meters_t is another name for double"

// with function pointers typedef gets genuinely ugly -- the new name
// ends up buried in the MIDDLE of the type
typedef int (*compare_fn_t)(int, int);

// ------------------------- THE MODERN WAY: using -----------------------------

// 'using' reads left-to-right like a normal assignment: newName = existingType.
// PREFER THIS. typedef only survives for backwards compatibility with C.
using DistanceMeters = double;      // name on the LEFT, where you expect it
using CompareFn = int (*)(int, int); // same function pointer, way more readable

// naming convention: learncpp suggests a capital letter and NO _t suffix,
// because the _t suffix is technically reserved by POSIX for its own types.

// the real payoff is shortening long template types you'd otherwise retype
using WaypointList = std::vector<double>;

int foo(int x, int y) { return x + y; }

int main() {

    // both of these are LITERALLY doubles. The alias is erased at compile time.
    DistanceMeters travelled { 12.5 };
    double raw { 4.5 };

    travelled = raw; // no cast, no warning, no complaint -- same type.
                     // an alias gives you NO protection against mixing up units.
                     // if you want real type safety, you need a struct/class.

    std::cout << "travelled = " << travelled << " m\n";

    distance_meters_t old_style { 3.0 }; // the typedef version behaves identically
    std::cout << "old_style  = " << old_style << " m\n";

    // aliases work for function pointers too
    CompareFn fn { foo };
    std::cout << "fn(3, 4) = " << fn(3, 4) << "\n";

    // and this is where they actually earn their keep:
    WaypointList path { 0.0, 1.5, 3.0, 4.5 }; // vs typing std::vector<double> every time

    std::cout << "path: ";
    for (double p : path)
        std::cout << p << " ";
    std::cout << "\n";

    // ---------------------- SCOPE OF AN ALIAS --------------------------------
    // an alias declared inside a block is LOCAL to that block, just like a variable
    {
        using Meters = int; // shadows nothing, only lives in these braces
        Meters m { 7 };
        std::cout << "block-scoped alias Meters m = " << m << "\n";
    }
    // Meters n {}; // ERROR if uncommented -- the alias is out of scope here

    // aliases meant to be shared across files go in a HEADER, so every
    // translation unit sees the identical definition (same rule as inline)

    return 0;
}

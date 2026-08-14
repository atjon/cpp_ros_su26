#include <iostream>

// 11.6 - Function templates
// 11.7 - Function template instantiation

// A template is NOT a function. It's a RECIPE the compiler uses to stamp out
// real functions on demand. Nothing is generated until you actually call it.

// ------------------------ 11.6 WRITING A TEMPLATE ----------------------------

// <typename T> introduces a TEMPLATE TYPE PARAMETER named T.
// 'typename' and 'class' mean exactly the same thing here -- prefer 'typename',
// since T doesn't have to be a class at all.
template <typename T>
T larger(T a, T b) {
    return (a > b) ? a : b;
}

// multiple parameters of the same type T must all MATCH -- see 11.8 for the fix

// ------------------------ 11.7 INSTANTIATION ---------------------------------
//
// When you call larger(3, 7), the compiler:
//   1. deduces T = int from the arguments
//   2. stamps out a real function larger<int>(int, int)  <-- an INSTANTIATION
//   3. compiles THAT function
//
// Consequences worth internalizing:
//   - a template with a bug in it produces NO error until it's instantiated
//   - each distinct T generates a separate function in the binary
//   - identical instantiations across .cpp files get de-duplicated by the linker
//     (templates are implicitly inline, so no ODR violation)

// This template compiles fine as written, but blows up for types that have
// no operator- (like std::string). The error appears at the CALL, not here.
template <typename T>
T difference(T a, T b) {
    return a - b;
}

int main() {

    // ---------------- deduced template arguments -----------------------------
    std::cout << "larger(3, 7)       = " << larger(3, 7) << "\n";       // T = int
    std::cout << "larger(2.5, 1.5)   = " << larger(2.5, 1.5) << "\n";   // T = double
    std::cout << "larger('a', 'z')   = " << larger('a', 'z') << "\n";   // T = char

    // ---------------- explicit template arguments ----------------------------
    // you can name T yourself instead of letting it be deduced
    std::cout << "larger<double>(3, 7) = " << larger<double>(3, 7) << "\n";
    // note the 3 and 7 got CONVERTED to double because T was pinned to double.
    // With deduction, larger(3, 7.0) would instead be an ERROR: T can't be
    // both int and double at once.

    // std::cout << larger(3, 7.0); // ERROR if uncommented: deduced conflicting
    //                              // types for T. Fixes: cast one argument,
    //                              // pin T explicitly, or use two type params
    //                              // (that's 11.8).

    // <> with nothing inside forces the TEMPLATE version specifically,
    // even if a non-template overload also exists
    std::cout << "larger<>(4, 9)     = " << larger<>(4, 9) << "\n";

    std::cout << "difference(10, 4)  = " << difference(10, 4) << "\n";
    // difference(std::string{"a"}, std::string{"b"}); // ERROR if uncommented:
    //   "no match for operator-". Notice the error points INTO the template
    //   body, which is why template errors are famously unreadable.

    // BEST PRACTICE: write the function normally for one concrete type first,
    // get it working, THEN turn the type into T. Debugging a template you
    // wrote template-first is much harder.

    return 0;
}

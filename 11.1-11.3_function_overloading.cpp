#include <iostream>

// 11.1 - Introduction to function overloading
// 11.2 - Function overload differentiation
// 11.3 - Function overload resolution and ambiguous matches

// Overloading = multiple functions with the SAME NAME in the same scope,
// separated by their PARAMETERS. The compiler picks which one to call based
// on the arguments at the call site. This is resolved at COMPILE time.

// ------------------------ 11.1 / 11.2 WHAT COUNTS ----------------------------

// these three are all "scale", differentiated by parameter TYPES
int scale(int value) {
    std::cout << "  [int version] ";
    return value * 2;
}

double scale(double value) {
    std::cout << "  [double version] ";
    return value * 2.0;
}

// differentiated by the NUMBER of parameters
int scale(int value, int factor) {
    std::cout << "  [int,int version] ";
    return value * factor;
}

// WHAT DOES *NOT* DIFFERENTIATE AN OVERLOAD:
//
// double scale(int value);   // ERROR: return type is NOT part of the signature.
//                            // The compiler resolves the call BEFORE it ever
//                            // looks at what you do with the result.
//
// int scale(int amount);     // ERROR: parameter NAMES don't matter either,
//                            // only their types.
//
// using Rpm = int;
// int scale(Rpm value);      // ERROR: a type alias is not a new type (see 10.7),
//                            // so this is just int again.

// ------------------------ 11.3 OVERLOAD RESOLUTION ---------------------------
// The compiler walks a sequence of steps, stopping at the first that works:
//   1. EXACT match
//   2. promotion   (char/short -> int, float -> double)
//   3. numeric conversion (int -> double, double -> int, ...)
//   4. user-defined conversion
// If two candidates tie at the SAME step, it's an AMBIGUOUS MATCH -- a compile
// error. Ambiguity is not resolved by declaration order or by "closeness".

void report(int) { std::cout << "  report(int)\n"; }
void report(double) { std::cout << "  report(double)\n"; }

// These two would make report('x') ambiguous, because char -> int and
// char -> unsigned int are BOTH plain conversions at the same rank:
//
// void report(unsigned int) { }

int main() {

    std::cout << "scale(5)      = " << scale(5) << "\n";        // exact -> int
    std::cout << "scale(5.5)    = " << scale(5.5) << "\n";      // exact -> double
    std::cout << "scale(5, 3)   = " << scale(5, 3) << "\n";     // 2 args -> int,int

    // a float ARGUMENT has no float overload, so it gets PROMOTED to double
    float f { 2.5f };
    std::cout << "scale(2.5f)   = " << scale(f) << "\n";        // promotion -> double

    // a char is promoted to int, so this hits the int version
    char c { 10 };
    std::cout << "scale(char 10)= " << scale(c) << "\n";        // promotion -> int

    std::cout << "\nresolution walkthrough:\n";
    report(42);    // exact match -> int
    report(4.2);   // exact match -> double
    report('a');   // no char overload; promotes to int
    report(4.2f);  // no float overload; promotes to double

    // AMBIGUOUS EXAMPLE (uncomment the unsigned overload above to see it break):
    // report('a'); // char -> int and char -> unsigned int are both conversions,
    //              // same rank, so the compiler refuses to guess.
    //
    // The fix is to be explicit at the call site:
    report(static_cast<int>('a'));

    // TAKEAWAY: overloads should do the SAME conceptual thing for different
    // types. Don't overload one name to do unrelated jobs -- that's just a
    // confusing API wearing a shared name.

    return 0;
}

#include <iostream>
#include <vector>
#include <algorithm>  // for std::find_if, std::sort, std::count_if
#include <functional> // for std::function
#include <string>
#include <string_view>

// 20.6 - Introduction to lambdas (anonymous functions)
// 20.7 - Lambda captures

// ------------------------ 20.6 LAMBDAS ---------------------------------------
//
// A lambda is a function defined INLINE, right where it's used, with no name.
// The full syntax:
//
//     [ captureClause ] ( parameters ) mutable -> returnType { statements }
//
// Everything except the [] and the {} is optional:
//     [](){}            is a valid (useless) lambda
//     []{ ... }         parameter list can be dropped entirely
//
// Why they exist: a function pointer (20.1) can only point at a real function
// defined somewhere else, and it can carry NO state. Lambdas fix both problems.

// A lambda is actually an anonymous CLASS with an overloaded operator() --
// a "functor". That's why it can hold state: the captures become its data
// members. Its type is unnamable, which is why you store them with auto.

// to take a lambda as a parameter, prefer a TEMPLATE (fastest -- no indirection)
template <typename Fn>
void applyToAll(std::vector<int>& values, Fn fn) {
    for (int& v : values)
        v = fn(v);
}

// or std::function when you need a concrete named type (slower -- may allocate,
// and calls through an indirection, but it can hold ANY callable)
void describe(std::string_view label, const std::function<int(int)>& fn) {
    std::cout << "  " << label << "(10) = " << fn(10) << "\n";
}

int main() {

    // ---------------- 20.6 basic lambdas -------------------------------------
    std::cout << "basic lambdas:\n";

    // store it in an auto variable. You can't write the type by hand --
    // every lambda has a unique, compiler-generated, unnamable type.
    auto square { [](int x) { return x * x; } };
    std::cout << "  square(7) = " << square(7) << "\n";

    // the return type is deduced. State it explicitly when deduction gets it wrong:
    auto half { [](int x) -> double { return x / 2.0; } };
    std::cout << "  half(7) = " << half(7) << "\n";

    // without the -> double above, this next one truncates -- a classic bug:
    auto badHalf { [](int x) { return x / 2; } };  // returns int!
    std::cout << "  badHalf(7) = " << badHalf(7) << " (truncated -- int division)\n";

    // an IMMEDIATELY INVOKED lambda -- defined and called on the spot.
    // Handy for complex initialization of a const variable.
    const int configured { [] {
        int base { 10 };
        return base * 4;
    }() };  // <- the trailing () calls it
    std::cout << "  immediately invoked: " << configured << "\n";

    // a lambda with NO captures converts to a plain function pointer (20.1)
    int (*asFnPtr)(int) { square };
    std::cout << "  as a function pointer: " << asFnPtr(5) << "\n";
    // (a lambda WITH captures cannot -- it has state, and a function pointer
    //  has nowhere to put it)

    // ---------------- where lambdas actually pay off -------------------------
    std::cout << "\nwith standard algorithms:\n";

    std::vector<int> readings { 42, 7, 91, 15, 63, 8, 27 };

    // find the first element over 50 -- the predicate is written right here,
    // instead of as a separate named function 40 lines away
    auto found { std::find_if(readings.begin(), readings.end(),
                              [](int v) { return v > 50; }) };
    if (found != readings.end())
        std::cout << "  first reading over 50: " << *found << "\n";

    std::cout << "  count of readings under 20: "
              << std::count_if(readings.begin(), readings.end(),
                               [](int v) { return v < 20; }) << "\n";

    // sort descending
    std::sort(readings.begin(), readings.end(),
              [](int a, int b) { return a > b; });
    std::cout << "  sorted descending: ";
    for (int v : readings) std::cout << v << " ";
    std::cout << "\n";

    // ---------------- 20.7 CAPTURES ------------------------------------------
    //
    // A lambda can only see variables from the enclosing scope if it CAPTURES
    // them. The capture clause [] is where you list them.
    //
    // This is the difference from a plain function: captures give the lambda
    // STATE that travels with it.

    std::cout << "\ncaptures:\n";

    int threshold { 30 };

    // auto broken { [](int v) { return v > threshold; } };
    //   ERROR if uncommented: threshold isn't captured, so it isn't visible.
    //   (Note this is NOT a scope issue -- the lambda's body can see the name,
    //    it just isn't allowed to use it without capturing.)

    // CAPTURE BY VALUE -- a COPY is made when the lambda is DEFINED
    auto overThreshold { [threshold](int v) { return v > threshold; } };
    std::cout << "  threshold=" << threshold << ", count over: "
              << std::count_if(readings.begin(), readings.end(), overThreshold) << "\n";

    // the copy is frozen at DEFINITION time, not call time:
    threshold = 80;
    std::cout << "  after threshold=80, the lambda still uses the OLD copy: "
              << std::count_if(readings.begin(), readings.end(), overThreshold)
              << " (captured 30)\n";

    // CAPTURE BY REFERENCE -- uses the actual variable, sees later changes
    auto overThresholdRef { [&threshold](int v) { return v > threshold; } };
    std::cout << "  by reference, threshold=80: "
              << std::count_if(readings.begin(), readings.end(), overThresholdRef)
              << "\n";
    threshold = 10;
    std::cout << "  by reference, threshold=10: "
              << std::count_if(readings.begin(), readings.end(), overThresholdRef)
              << " (it saw the change)\n";

    // ---------------- mutable ------------------------------------------------
    //
    // Captured-by-value variables are const inside the lambda by default.
    // 'mutable' makes the lambda's OWN COPY writable -- it does NOT affect
    // the original variable.

    int callCount { 0 };
    auto counter { [callCount]() mutable {
        ++callCount;                 // modifies the lambda's copy
        return callCount;
    } };

    std::cout << "\n  mutable lambda: " << counter() << ", " << counter()
              << ", " << counter() << "\n";
    std::cout << "  but the original callCount is still " << callCount << "\n";
    //           ^ this surprises everyone once. The copy persists between calls
    //             (it's a data member of the functor), but it's not the original.

    // to actually count calls in the caller's variable, capture by reference:
    int realCount { 0 };
    auto realCounter { [&realCount]() { ++realCount; } };
    realCounter(); realCounter(); realCounter();
    std::cout << "  by-reference counter: realCount = " << realCount << "\n";

    // ---------------- default captures ----------------------------------------
    //
    //   [=]  capture everything USED, by value
    //   [&]  capture everything USED, by reference
    //
    // Convenient, but they hide WHAT is being captured, and [&] makes dangling
    // captures easy to write by accident. Prefer naming them explicitly.

    int a { 1 };
    int b { 2 };
    auto sumByValue { [=] { return a + b; } };      // copies both
    auto sumByRef   { [&] { return a + b; } };      // references both
    a = 100;
    std::cout << "\n  [=] captured copies: " << sumByValue() << "\n";
    std::cout << "  [&] sees changes:    " << sumByRef() << "\n";

    // mixing: default by value, but one specific variable by reference
    auto mixed { [=, &b] { return a + b; } };
    b = 50;
    std::cout << "  [=, &b] mixed:       " << mixed() << "\n";

    // ---------------- THE DANGLING CAPTURE TRAP -------------------------------
    //
    // Capturing by reference is only safe while the referenced object is alive.
    // Returning a lambda that captured a LOCAL by reference is a dangling
    // reference, exactly like returning a reference to a local (12.12):
    //
    //   auto makeBroken() {
    //       int local { 42 };
    //       return [&local] { return local; };  // local dies at the return!
    //   }
    //
    // RULE: if the lambda outlives the current scope, capture BY VALUE.

    // ---------------- init captures (C++14) ------------------------------------
    // create a new variable that exists only in the lambda
    auto withInit { [total = 100](int v) { return total + v; } };
    std::cout << "\n  init capture: " << withInit(5) << "\n";

    // ---------------- generic lambdas (C++14) ----------------------------------
    // 'auto' parameters make the lambda's operator() a template
    auto printAny { [](const auto& value) { std::cout << "    " << value << "\n"; } };
    std::cout << "  generic lambda:\n";
    printAny(42);
    printAny(3.14);
    printAny(std::string { "works for any printable type" });

    // ---------------- passing lambdas around -----------------------------------
    std::cout << "\npassing lambdas as parameters:\n";

    std::vector<int> values { 1, 2, 3, 4, 5 };
    applyToAll(values, [](int v) { return v * 10; });  // via template
    std::cout << "  after applyToAll(*10): ";
    for (int v : values) std::cout << v << " ";
    std::cout << "\n";

    int offset { 7 };
    describe("square", [](int x) { return x * x; });          // via std::function
    describe("addOffset", [offset](int x) { return x + offset; }); // carries state

    return 0;
}

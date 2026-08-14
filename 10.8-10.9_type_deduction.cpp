#include <iostream>
#include <string>
#include <string_view>

// 10.8 - Type deduction for objects using the auto keyword
// 10.9 - Type deduction for functions

// -------------------- 10.9 TYPE DEDUCTION FOR FUNCTIONS ----------------------

// 'auto' as the RETURN type means the compiler figures it out from the returns.
// The catch: the compiler must SEE the body to know the type, so a forward
// declaration alone isn't enough -- these can't be split into a .h/.cpp pair
// the normal way.
auto addRates(int a, int b) {
    return a + b; // deduced as int
}

// if multiple returns disagree, it's a compile ERROR, not a silent conversion
auto safeDivide(double num, double den) {
    if (den == 0.0)
        return 0.0;  // double
    return num / den; // also double -- must MATCH, or it won't compile
}

// TRAILING RETURN TYPE: the return type moves after the ->
// This is NOT deduction -- you still state the type, just in a different place.
// Useful for lining up long function names, and required for some template cases.
auto motorLabel(int id) -> std::string {
    return "motor_" + std::to_string(id);
}

// C++20 abbreviated function template: 'auto' as a PARAMETER type quietly
// turns this into a template. Each distinct argument type makes a new version.
auto doubleIt(auto x) {
    return x * 2;
}

int main() {

    // -------------------- 10.8 AUTO FOR OBJECTS ------------------------------

    // auto deduces the type from the INITIALIZER, so an initializer is required
    auto speed { 3.5 };      // double
    auto ticks { 42 };       // int
    auto flag { true };      // bool
    // auto broken;          // ERROR if uncommented -- nothing to deduce from

    std::cout << "speed=" << speed << " ticks=" << ticks << " flag=" << flag << "\n";

    // GOTCHA #1: auto DROPS const and reference qualifiers
    const int maxRpm { 8000 };
    auto copyRpm { maxRpm }; // deduced as plain int, NOT const int
    copyRpm = 10; // perfectly legal -- the const didn't come along for the ride

    // put it back explicitly if you want it
    const auto lockedRpm { maxRpm }; // const int
    std::cout << "copyRpm=" << copyRpm << " lockedRpm=" << lockedRpm << "\n";

    // GOTCHA #2: a string literal deduces to const char*, NOT std::string.
    auto name { "lidar" }; // const char* -- probably not what you wanted

    using namespace std::literals; // needed for the s and sv suffixes
    auto realString { "lidar"s };   // std::string
    auto cheapView  { "lidar"sv };  // std::string_view

    std::cout << "name=" << name
              << " realString=" << realString
              << " cheapView=" << cheapView << "\n";

    // WHERE AUTO ACTUALLY HELPS: when the type is long, obvious, or unspeakable
    auto result { addRates(20, 22) };
    std::cout << "addRates(20,22) = " << result << "\n";
    std::cout << "safeDivide(7,2) = " << safeDivide(7.0, 2.0) << "\n";
    std::cout << "safeDivide(7,0) = " << safeDivide(7.0, 0.0) << "\n";
    std::cout << "motorLabel(3)   = " << motorLabel(3) << "\n";

    // one abbreviated template, two instantiations
    std::cout << "doubleIt(21)   = " << doubleIt(21) << "\n";   // int version
    std::cout << "doubleIt(1.25) = " << doubleIt(1.25) << "\n"; // double version

    // BEST PRACTICE: use auto for locals where the type is clear from the right
    // side, but prefer EXPLICIT return types on functions -- callers read the
    // signature far more often than they read the body.

    return 0;
}

#include <iostream>
#include <vector>
#include <string>

// 16.1 - Introduction to containers and arrays
// 16.2 - Introduction to std::vector and list constructors
// 16.3 - std::vector and the unsigned length and subscript problem

// ------------------------ 16.1 WHY CONTAINERS EXIST --------------------------
//
// Suppose you're logging distance-sensor readings. With plain variables:
//
//     int reading0 { };
//     int reading1 { };
//     int reading2 { };   // ... and 97 more?
//
// This falls apart immediately: you can't loop over them, you can't pass them
// as a group, and the count is frozen at compile time.
//
// A CONTAINER holds a collection of values (its ELEMENTS) and manages their
// storage for you. An ARRAY is the container type where elements sit
// CONTIGUOUSLY in memory and are accessed by INDEX -- so getting element #57
// is just as fast as getting element #0.
//
// C++ gives you three array-like containers:
//   std::vector  -- dynamic length, decided at RUNTIME       (chapter 16)
//   std::array   -- fixed length, decided at COMPILE TIME    (chapter 17)
//   C-style array-- the raw, dangerous, inherited-from-C one (17.7)
//
// std::vector should be your default.

// ------------------------ 16.2 CREATING A VECTOR -----------------------------

void printVector(const std::vector<int>& v) {
    std::cout << "    [ ";
    for (int e : v) std::cout << e << ' ';
    std::cout << "] length=" << v.size() << '\n';
}

int main() {

    std::cout << "creating vectors:\n";

    // a LIST CONSTRUCTOR -- braces filled with the actual elements
    std::vector<int> readings { 42, 17, 91, 8 };
    printVector(readings);

    // empty, to be filled later
    std::vector<int> empty { };
    printVector(empty);

    // THE CLASSIC TRAP: braces vs parentheses mean DIFFERENT things here.
    std::vector<int> braces { 5 };   // one element, whose value is 5
    std::vector<int> parens ( 5 );   // FIVE elements, all value-initialized to 0
    std::cout << "  vector<int> braces { 5 }: "; printVector(braces);
    std::cout << "  vector<int> parens ( 5 ): "; printVector(parens);
    //
    // The list constructor is STRONGLY PREFERRED whenever a braced list could
    // match it, so { 5 } means "a list containing 5". Use ( 5 ) only when you
    // deliberately want a LENGTH.

    // vectors work with any type
    std::vector<std::string> waypoints { "start", "corner", "goal" };
    std::cout << "  strings: ";
    for (const auto& w : waypoints) std::cout << w << ' ';
    std::cout << '\n';

    // CTAD (13.14) deduces the element type from the initializers
    std::vector deduced { 1.5, 2.5, 3.5 };   // -> std::vector<double>
    std::cout << "  CTAD deduced a vector<double> of length "
              << deduced.size() << '\n';

    // a const vector is allowed, but its LENGTH and ELEMENTS both become
    // fixed -- so it must be initialized at creation
    const std::vector<int> frozen { 1, 2, 3 };
    // frozen[0] = 99;   // ERROR if uncommented: elements are const too

    // ---------------- accessing elements -------------------------------------
    std::cout << "\naccessing elements:\n";

    // SUBSCRIPT operator [] -- fast, but does NO bounds checking
    std::cout << "  readings[0] = " << readings[0] << '\n';
    std::cout << "  readings[3] = " << readings[3] << '\n';

    // Indices are ZERO-BASED, so a length-4 vector has valid indices 0..3.
    // readings[4]  <-- UNDEFINED BEHAVIOR. Not an error, not a crash you can
    //                  rely on -- it silently reads whatever memory is there.

    // .at() DOES bounds check, and throws std::out_of_range instead
    std::cout << "  readings.at(1) = " << readings.at(1) << '\n';
    // readings.at(4);   // would throw -- a detectable failure, not silent UB

    // BEST PRACTICE: [] when you already know the index is valid (a loop
    // bounded by size()), .at() when the index came from somewhere untrusted.

    readings[0] = 100;   // [] returns a reference, so you can assign through it
    std::cout << "  after readings[0] = 100: "; printVector(readings);

    // ---------------- 16.3 THE UNSIGNED LENGTH PROBLEM ------------------------
    //
    // Here's a genuine C++ design wart. size() does NOT return an int -- it
    // returns std::vector<T>::size_type, which is an UNSIGNED integer
    // (in practice std::size_t). This causes two real bugs.

    std::cout << "\nthe unsigned length problem:\n";

    std::cout << "  readings.size() = " << readings.size()
              << " (type is unsigned)\n";

    // BUG 1: comparing a signed index against an unsigned length.
    // The signed value is CONVERTED to unsigned, so a negative becomes huge.
    int signedIndex { -1 };
    if (static_cast<std::size_t>(signedIndex) > readings.size())
        std::cout << "  -1 converted to unsigned is enormous: "
                  << static_cast<std::size_t>(signedIndex) << '\n';

    // BUG 2: counting DOWN with an unsigned loop variable never terminates.
    //
    //   for (std::size_t i { v.size() - 1 }; i >= 0; --i)   // INFINITE LOOP
    //
    // An unsigned value is ALWAYS >= 0, so the condition can never be false.
    // When i is 0 and you do --i, it WRAPS AROUND to a gigantic number.
    //
    // Worse, on an EMPTY vector, v.size() - 1 is 0u - 1, which wraps to the
    // largest possible size_t before the loop even starts.

    // ---------------- the C++20 fix: std::ssize ------------------------------
    //
    // std::ssize() returns a SIGNED length. Prefer it when you need to do
    // arithmetic on the length or iterate backwards.

    std::cout << "  std::ssize(readings) = " << std::ssize(readings)
              << " (signed -- safe to subtract from)\n";

    std::cout << "  backwards, safely:\n    ";
    for (auto i { std::ssize(readings) - 1 }; i >= 0; --i)
        std::cout << readings[static_cast<std::size_t>(i)] << ' ';
    std::cout << '\n';

    // and on an empty vector, this correctly does nothing instead of looping
    // four billion times:
    std::cout << "  backwards over the empty vector: ";
    for (auto i { std::ssize(empty) - 1 }; i >= 0; --i)
        std::cout << empty[static_cast<std::size_t>(i)] << ' ';
    std::cout << "(nothing, correctly)\n";

    // BEST PRACTICE: sidestep the whole mess with a range-based for loop
    // (16.8) whenever you don't actually need the index.

    return 0;
}

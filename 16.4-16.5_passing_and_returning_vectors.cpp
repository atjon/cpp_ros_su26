#include <iostream>
#include <vector>
#include <string>

// 16.4 - Passing std::vector
// 16.5 - Returning std::vector, and an introduction to move semantics

// ------------------------ 16.4 PASSING A VECTOR ------------------------------
//
// A std::vector owns a dynamically allocated block of memory. Copying one
// means allocating a new block and copying every element -- expensive, and
// proportional to the length.

// WRONG (usually): pass by value copies the whole thing
void byValue(std::vector<int> v) {
    v[0] = -1;   // modifies the COPY; the caller never sees it
}

// RIGHT: pass by const reference -- no copy, and can't be modified
void byConstRef(const std::vector<int>& v) {
    std::cout << "    length " << v.size() << ", first = " << v[0] << '\n';
    // v[0] = -1;   // ERROR if uncommented: v is const
}

// pass by non-const reference when you genuinely need to MODIFY the caller's
// vector
void scaleInPlace(std::vector<int>& v, int factor) {
    for (int& e : v)     // note int&, so we modify the actual elements
        e *= factor;
}

// ---------------- the element-type problem -----------------------------------
//
// std::vector<int> and std::vector<double> are COMPLETELY UNRELATED types.
// A function taking one will not accept the other. Three ways out:

// 1. a function template -- works for any element type
template <typename T>
void printAny(const std::vector<T>& v) {
    std::cout << "    [ ";
    for (const auto& e : v) std::cout << e << ' ';
    std::cout << "]\n";
}

// 2. an abbreviated function template (C++20) -- an `auto` PARAMETER makes
//    this a template implicitly. Note that `auto` is only allowed as the
//    parameter type itself; `const std::vector<auto>&` does NOT compile.
void printAbbrev(const auto& v) {
    std::cout << "    (abbreviated) length " << v.size() << '\n';
}

// 3. a generic template, if you don't even need to name std::vector
template <typename Container>
auto sumOf(const Container& c) {
    typename Container::value_type total { };   // value_type is the element type
    for (const auto& e : c) total += e;
    return total;
}

// ------------------------ 16.5 RETURNING A VECTOR ----------------------------
//
// Returning by value LOOKS expensive -- it's a whole vector -- but it isn't,
// for two reasons:
//
//   1. COPY ELISION: the compiler constructs the return value directly in the
//      caller's storage. No copy happens at all. Since C++17 this is mandatory
//      for a returned temporary.
//
//   2. MOVE SEMANTICS: when elision doesn't apply, the vector is MOVED, not
//      copied. A move just steals the internal pointer -- a few integer
//      assignments regardless of how many elements there are.
//
// So: RETURN BY VALUE. Do NOT return a reference or pointer to a local vector
// (12.12) -- it would dangle.

std::vector<int> generateReadings(int count) {
    std::vector<int> result { };
    for (int i { 0 }; i < count; ++i)
        result.push_back(i * i);
    return result;      // elided or moved -- cheap either way
}

// DON'T do this:
//   const std::vector<int>& broken() {
//       std::vector<int> local { 1, 2, 3 };
//       return local;                        // dangles the instant we return
//   }

int main() {

    std::vector<int> readings { 10, 20, 30, 40 };

    std::cout << "passing:\n";

    byValue(readings);
    std::cout << "  after byValue, readings[0] is still " << readings[0]
              << " (the function got a copy)\n";

    std::cout << "  byConstRef sees:\n";
    byConstRef(readings);

    scaleInPlace(readings, 3);
    std::cout << "  after scaleInPlace(x3): ";
    printAny(readings);

    // ---------------- templates over element types ----------------------------
    std::cout << "\ntemplates handle any element type:\n";

    std::vector<double> voltages { 3.3, 5.0, 12.0 };
    std::vector<std::string> labels { "left", "right" };

    printAny(readings);
    printAny(voltages);
    printAny(labels);

    printAbbrev(voltages);

    std::cout << "  sumOf(readings) = " << sumOf(readings) << '\n';
    std::cout << "  sumOf(voltages) = " << sumOf(voltages) << '\n';

    // ---------------- 16.5 returning ------------------------------------------
    std::cout << "\nreturning by value:\n";

    std::vector<int> squares { generateReadings(6) };
    std::cout << "  generateReadings(6) -> ";
    printAny(squares);
    std::cout << "  no copy was made -- the vector was constructed directly\n"
                 "  in squares' storage (copy elision)\n";

    // it composes naturally, which is the real payoff
    std::cout << "  sum of a freshly returned vector: "
              << sumOf(generateReadings(5)) << '\n';

    // ---------------- when a move DOES happen ---------------------------------
    //
    // Returning a NAMED local is a candidate for elision but not guaranteed;
    // if the compiler can't elide, it moves. Either way you don't pay for a
    // deep copy.
    //
    // The one thing that DOES force a copy: returning a named local as a
    // reference-to-const, or `return std::move(local)` -- the latter actually
    // DEFEATS elision. Never write std::move on a return of a local.

    std::cout << "\n  BEST PRACTICE:\n";
    std::cout << "    read only          -> const std::vector<T>&\n";
    std::cout << "    modify caller's    -> std::vector<T>&\n";
    std::cout << "    hand back a new one-> return by value\n";

    return 0;
}

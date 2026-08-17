#include <iostream>
#include <string>

// 13.13 - Class templates
// 13.14 - Class template argument deduction (CTAD) and deduction guides
// 13.15 - Alias templates

// ------------------------ 13.13 CLASS TEMPLATES ------------------------------
//
// Same idea as function templates (11.6), applied to types.
// Without templates you'd write PairInt, PairDouble, PairString... all
// identical except for one type. A class template is the recipe.

template <typename T>
struct Pair {
    T first { };
    T second { };
};

// member functions of a class template are themselves templates.
// Defined OUTSIDE the struct, the syntax gets verbose:
template <typename T>
struct Reading {
    T value { };
    std::string label { };

    // defined INSIDE -- much easier, and implicitly inline
    T doubled() const { return value * 2; }

    void print() const;  // declared here, defined below
};

// note you must repeat the template header AND qualify with Reading<T>
template <typename T>
void Reading<T>::print() const {
    std::cout << "  " << label << " = " << value << "\n";
}

// two type parameters, so the members can differ
template <typename T, typename U>
struct MixedPair {
    T first { };
    U second { };
};

// ------------------------ 13.15 ALIAS TEMPLATES ------------------------------
//
// A plain type alias (10.7) can't be templated:
//   using IntPair = Pair<int>;    // fine, but pins T to int
//
// An ALIAS TEMPLATE leaves the parameter open:
template <typename T>
using SensorPair = Pair<T>;      // still parameterized

// the real use: giving a name to a partially-filled template
template <typename T>
using LabeledWith = MixedPair<std::string, T>; // first is always a string

int main() {

    // ---------------- 13.13 instantiating ------------------------------------
    // pre-C++17 you had to spell out the type argument every time
    Pair<int> counts { 3, 7 };
    Pair<double> volts { 11.8, 12.1 };
    Pair<std::string> names { "left", "right" };

    std::cout << "counts: " << counts.first << ", " << counts.second << "\n";
    std::cout << "volts:  " << volts.first << ", " << volts.second << "\n";
    std::cout << "names:  " << names.first << ", " << names.second << "\n";

    // each instantiation is a genuinely SEPARATE type.
    // counts = volts;  // ERROR if uncommented: Pair<int> and Pair<double>
    //                  // are unrelated types, despite coming from one template

    Reading<double> battery { 11.85, "battery_v" };
    battery.print();
    std::cout << "  doubled: " << battery.doubled() << "\n";

    MixedPair<std::string, int> tagged { "motor_id", 4 };
    std::cout << "mixed:  " << tagged.first << " -> " << tagged.second << "\n";

    // ---------------- 13.14 CTAD (C++17) --------------------------------------
    //
    // Class Template Argument Deduction lets the compiler figure out the
    // template arguments from the INITIALIZER, exactly like function templates.

    Pair deduced { 4, 9 };            // deduced as Pair<int>
    Pair deducedD { 1.5, 2.5 };       // deduced as Pair<double>
    MixedPair mixed { std::string { "rpm" }, 1200 }; // MixedPair<std::string,int>

    std::cout << "\nCTAD:\n";
    std::cout << "  deduced:  " << deduced.first << ", " << deduced.second << "\n";
    std::cout << "  deducedD: " << deducedD.first << ", " << deducedD.second << "\n";
    std::cout << "  mixed:    " << mixed.first << " -> " << mixed.second << "\n";

    // CTAD ONLY works when there's an initializer to deduce from:
    // Pair empty;      // ERROR if uncommented: nothing to deduce from
    // Pair empty2 { }; // ERROR too -- an empty brace list deduces nothing
    Pair<int> empty { }; // explicit type argument is required here

    // GOTCHA: CTAD does not apply to the <> of a member's type. It works on
    // the OBJECT declaration only.

    // ---------------- deduction guides ----------------------------------------
    //
    // In C++17 an aggregate needed an explicit DEDUCTION GUIDE to teach the
    // compiler how to deduce. C++20 generates them automatically for
    // aggregates, so the guide below is no longer required -- but you'll still
    // meet them in library code and in pre-C++20 codebases. The syntax is:
    //
    //   template <typename T>
    //   Pair(T, T) -> Pair<T>;
    //
    // Read it as: "a Pair constructed from two T's is a Pair<T>".

    // ---------------- 13.15 alias templates ------------------------------------
    std::cout << "\nalias templates:\n";

    SensorPair<int> viaAlias { 10, 20 };   // exactly a Pair<int>
    std::cout << "  SensorPair<int>: " << viaAlias.first
              << ", " << viaAlias.second << "\n";

    // it really is the same type, not a distinct one -- aliases never create
    // new types, templated or not
    Pair<int> plain { viaAlias };
    std::cout << "  assigned straight into a Pair<int>: " << plain.first << "\n";

    LabeledWith<double> labeled { "temperature_c", 36.6 };
    std::cout << "  LabeledWith<double>: " << labeled.first
              << " -> " << labeled.second << "\n";

    // CTAD works through an alias template too (C++20)
    SensorPair aliasDeduced { 1, 2 };
    std::cout << "  CTAD through the alias: " << aliasDeduced.second << "\n";

    return 0;
}

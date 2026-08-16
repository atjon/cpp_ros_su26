#include <iostream>
#include <optional>
#include <string>

// 12.14 - Type deduction with pointers, references, and const
// 12.15 - std::optional

// ------------------------ 12.14 AUTO DROPS THINGS ----------------------------
//
// Recall from 10.8: auto DROPS const and reference qualifiers.
// Now that references and pointers exist, the exact rules matter.

int& getRef() {
    static int stored { 42 };
    return stored;
}

const int& getConstRef() {
    static const int stored { 7 };
    return stored;
}

int* getPtr() {
    static int stored { 99 };
    return &stored;
}

// ------------------------ 12.15 STD::OPTIONAL --------------------------------
//
// std::optional<T> holds either a T or NOTHING. It's the type-safe answer to
// "this function might not have a result", replacing sentinel values (-1) and
// nullable pointers.

std::optional<double> safeDivide(double num, double den) {
    if (den == 0.0)
        return {};        // empty optional -- "no value". std::nullopt also works.
    return num / den;     // implicitly wrapped into the optional
}

std::optional<int> findMotorId(std::string_view name) {
    if (name == "left")  return 1;
    if (name == "right") return 2;
    return std::nullopt;  // explicit "nothing"
}

int main() {

    // ---------------- 12.14 references are dropped ---------------------------
    auto a { getRef() };        // deduced as int -- the & is DROPPED, so this COPIES
    a = 100;                    // modifies only the copy
    std::cout << "after modifying the auto copy, getRef() = " << getRef() << "\n";

    auto& b { getRef() };       // ASK for the reference back -> int&
    b = 100;                    // this modifies the real object
    std::cout << "after modifying the auto& , getRef() = " << getRef() << "\n";

    // ---------------- const is dropped too -----------------------------------
    auto c { getConstRef() };        // int -- both const AND & dropped
    c = 5;                           // legal, it's just a copy
    const auto& d { getConstRef() }; // const int& -- explicitly kept
    // d = 5;                        // ERROR if uncommented
    std::cout << "c=" << c << " d=" << d << "\n";

    // TOP-LEVEL vs LOW-LEVEL const, the distinction that explains the rules:
    //   top-level const = the object itself is const   (const int x)      -> DROPPED
    //   low-level const = what it POINTS to is const   (const int* p)     -> KEPT
    const int value { 10 };
    const int* ptrToConst { &value };

    auto p1 { ptrToConst }; // deduced as const int* -- the LOW-level const stays,
                            // because dropping it would let you write through it
    // *p1 = 20;            // ERROR if uncommented: still points to const

    int* const constPtr { getPtr() };
    auto p2 { constPtr };   // deduced as int* -- the TOP-level const is dropped
    p2 = nullptr;           // fine, the copy isn't const

    std::cout << "p1 reads " << *p1 << ", p2 is now null: " << (p2 == nullptr) << "\n";

    // pointers don't need auto& to stay pointers -- the * is part of the type,
    // not a qualifier, so it's never dropped
    auto p3 { getPtr() };   // int*
    *p3 = 123;              // this DOES modify the original
    std::cout << "wrote through p3, *getPtr() = " << *getPtr() << "\n";

    // ---------------- 12.15 std::optional ------------------------------------
    std::cout << "\nstd::optional:\n";

    auto result { safeDivide(10.0, 4.0) };

    // an optional converts to bool: has a value -> true
    if (result)
        std::cout << "  10/4 = " << *result << "\n";  // * unwraps it
    else
        std::cout << "  10/4 undefined\n";

    auto bad { safeDivide(10.0, 0.0) };
    if (bad)
        std::cout << "  10/0 = " << *bad << "\n";
    else
        std::cout << "  10/0 -> no value (empty optional)\n";

    // has_value() is the explicit spelling of the bool check
    std::cout << "  bad.has_value() = " << bad.has_value() << "\n";

    // value_or() supplies a fallback in one step -- often the cleanest option
    std::cout << "  bad.value_or(-1.0) = " << bad.value_or(-1.0) << "\n";

    // value() throws std::bad_optional_access if empty, unlike * which is UB
    std::cout << "  result.value() = " << result.value() << "\n";
    // std::cout << bad.value();  // THROWS if uncommented
    // std::cout << *bad;         // UNDEFINED BEHAVIOR if uncommented -- no check!

    for (std::string_view name : { "left", "right", "rear" }) {
        auto id { findMotorId(name) };
        std::cout << "  " << name << " -> "
                  << (id ? std::to_string(*id) : std::string { "not found" }) << "\n";
    }

    // TRADEOFFS vs returning a pointer:
    //   + optional owns its value, so there's no lifetime/dangling question
    //   + "might be empty" is visible in the SIGNATURE, not in a comment
    //   - it stores the T by value, so it copies; for big objects that costs
    //   - can't represent "a reference that might be absent" (no optional<T&>)

    return 0;
}

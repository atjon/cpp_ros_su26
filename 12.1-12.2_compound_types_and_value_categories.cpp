#include <iostream>

// 12.1 - Introduction to compound data types
// 12.2 - Value categories (lvalues and rvalues)

// ------------------------ 12.1 COMPOUND TYPES --------------------------------
//
// Fundamental types (int, double, bool, char...) are built into the language.
// COMPOUND types are built OUT of other types:
//   - functions
//   - arrays
//   - pointers            (12.7+)
//   - references          (12.3+)
//   - enums               (13.2+)
//   - structs / classes   (13.7+, 14.1+)
//
// The rest of chapter 12 is references and pointers. Everything after that in
// the tutorial builds on this chapter, so it's worth being solid here.

// ------------------------ 12.2 VALUE CATEGORIES ------------------------------
//
// EVERY expression in C++ has TWO properties:
//   1. a TYPE          (int, double, ...)
//   2. a VALUE CATEGORY (lvalue or rvalue)
//
// The value category answers: "can I take the address of this / does it
// persist beyond this expression?"
//
//   LVALUE  - an expression that identifies a real object with an identity
//             and a storage location. Historically "left of the =".
//             Two flavors: modifiable and non-modifiable (const).
//
//   RVALUE  - an expression that is just a VALUE with no persistent identity.
//             Literals, temporaries, and function return-by-value results.
//             It dies at the end of the full expression it appears in.
//
// The simplest practical test: if it can legally go on the LEFT of an =,
// it's an lvalue.

int getValue() { return 7; } // returns an RVALUE (a temporary)

int& getRef() {              // returns an LVALUE (see 12.12)
    static int stored { 99 };
    return stored;
}

int main() {

    int x { 5 };
    const int cx { 10 };

    // ---------------- lvalues ------------------------------------------------
    // x    is a modifiable lvalue -- identity, an address, assignable
    // cx   is a NON-modifiable lvalue -- identity and an address, but const
    // (written as comments because a bare "x;" statement is a warning:
    //  the expression is evaluated and its result thrown away)

    x = 6;      // legal: x is a modifiable lvalue
    // cx = 11; // ERROR if uncommented: cx is a non-modifiable lvalue

    std::cout << "x = " << x << ", cx = " << cx << "\n";

    // ---------------- rvalues ------------------------------------------------
    // 5;          // an rvalue literal -- no identity, no address
    // x + 1;      // an rvalue -- the result is a temporary
    // getValue(); // an rvalue -- return by value produces a temporary

    // 5 = x;         // ERROR if uncommented: can't assign to an rvalue
    // getValue() = 3; // ERROR if uncommented: same reason

    // but a function returning a REFERENCE gives back an lvalue,
    // so this genuinely works:
    getRef() = 123;
    std::cout << "after getRef() = 123, getRef() is " << getRef() << "\n";

    // ---------------- the conversion ------------------------------------------
    // An lvalue used where an rvalue is expected undergoes an implicit
    // LVALUE-TO-RVALUE CONVERSION -- it gives up its value.
    int y { x }; // x is an lvalue, but here we need its VALUE, so it converts
    std::cout << "y copied from x = " << y << "\n";

    // this is why x = x + 1 works: the right x converts to an rvalue (its
    // value), the left x stays an lvalue (its location).
    x = x + 1;
    std::cout << "x after x = x + 1 is " << x << "\n";

    // WHY ANY OF THIS MATTERS:
    //   - 12.3: an lvalue reference can only bind to an LVALUE
    //   - 12.4: a const lvalue reference can also bind to an RVALUE
    //   - 22.2: an rvalue reference (&&) binds to RVALUES, which is the whole
    //           foundation of move semantics
    //
    // Value categories look like pedantic trivia right up until chapter 22,
    // where they turn out to be the entire mechanism.

    return 0;
}

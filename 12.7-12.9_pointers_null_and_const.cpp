#include <iostream>

// 12.7 - Introduction to pointers
// 12.8 - Null pointers
// 12.9 - Pointers and const

// ------------------------ 12.7 POINTERS --------------------------------------
//
// A pointer is an object that HOLDS AN ADDRESS.
// Two operators do all the work:
//   &  address-of  -- given an object, get its address
//   *  dereference -- given an address, get the object living there
//
// (& is overloaded three ways in C++: address-of, part of a reference type,
//  and bitwise AND. Read it from context.)

void describe(int* ptr) {
    // ALWAYS check before dereferencing a pointer you didn't create
    if (ptr)                       // shorthand for ptr != nullptr
        std::cout << "  points to " << *ptr << "\n";
    else
        std::cout << "  null pointer, nothing to read\n";
}

int main() {

    int rpm { 1200 };

    // ---------------- 12.7 the basics ----------------------------------------
    std::cout << "value of rpm:   " << rpm << "\n";
    std::cout << "address of rpm: " << &rpm << "\n";

    int* ptr { &rpm };  // ptr HOLDS the address of rpm
                        // style note: the * binds to the TYPE, so write int* ptr.
                        // Careful though -- "int* a, b;" makes b a plain int!

    std::cout << "ptr holds:      " << ptr << "\n";
    std::cout << "*ptr (deref):   " << *ptr << "\n";

    *ptr = 1800;        // writing THROUGH the pointer changes rpm itself
    std::cout << "after *ptr = 1800, rpm = " << rpm << "\n";

    // POINTERS VS REFERENCES -- the differences that actually matter:
    //   - a pointer can be reseated; a reference can't
    //   - a pointer can be null; a reference can't
    //   - a pointer needs explicit * to read through; a reference is implicit
    //   - a pointer is its own object with its own address; a reference isn't
    int other { 5 };
    ptr = &other;       // RESEATING -- perfectly legal, unlike with a reference
    std::cout << "reseated ptr, *ptr = " << *ptr << "\n";

    // a pointer is an object too, so it has its own address
    std::cout << "address of ptr itself: " << &ptr << "\n";

    // ---------------- 12.8 null pointers -------------------------------------
    //
    // A pointer that isn't pointing at anything should hold NULLPTR.
    // Use nullptr -- NOT 0 and NOT the C macro NULL. nullptr has its own type
    // (std::nullptr_t), so it can't be mistaken for an integer in an overload.

    int* empty { };          // value-initialized -> nullptr
    int* explicitNull { nullptr };

    std::cout << "\nempty is null?        " << (empty == nullptr) << "\n";
    std::cout << "explicitNull is null? " << (explicitNull == nullptr) << "\n";

    // a pointer converts to bool: null -> false, non-null -> true
    describe(&rpm);
    describe(nullptr);

    // *empty; // UNDEFINED BEHAVIOR if uncommented -- dereferencing null.
    //         // Usually a crash, but "usually" is doing a lot of work there.
    //         // UB means the compiler is allowed to do ANYTHING.

    // DANGLING POINTERS are the nastier problem: a pointer to an object that
    // no longer exists. It isn't null, so the null check passes, and reading
    // it is still UB. Nothing in the language detects this for you.
    int* dangling { };
    {
        int temp { 42 };
        dangling = &temp;
    } // temp dies here -- dangling now points at reclaimed memory
    // std::cout << *dangling; // UB if uncommented, may well print 42 anyway
    std::cout << "dangling is not null (" << (dangling != nullptr)
              << ") but is NOT safe to read\n";

    // BEST PRACTICE: set a pointer to nullptr the moment it stops being valid.
    dangling = nullptr;

    // ---------------- 12.9 pointers and const --------------------------------
    //
    // TWO independent things can be const: the POINTEE and the POINTER.
    // Read the declaration RIGHT TO LEFT and it stops being confusing.

    int a { 1 };
    int b { 2 };
    const int c { 3 };

    // "pointer to const int" -- can't write through it, CAN be reseated
    const int* ptrToConst { &a };
    // *ptrToConst = 10;    // ERROR if uncommented: pointee is const
    ptrToConst = &b;        // fine: the pointer itself isn't const
    ptrToConst = &c;        // and this is the point -- it can hold a const's address

    // "const pointer to int" -- CAN write through it, can't be reseated
    int* const constPtr { &a };
    *constPtr = 10;         // fine: the pointee isn't const
    // constPtr = &b;       // ERROR if uncommented: the pointer is const
    std::cout << "\nafter *constPtr = 10, a = " << a << "\n";

    // "const pointer to const int" -- neither can change
    const int* const bothConst { &c };
    // *bothConst = 4;      // ERROR if uncommented
    // bothConst = &a;      // ERROR if uncommented
    std::cout << "bothConst reads " << *bothConst << " and that's all it can do\n";

    // A non-const pointer can NEVER hold the address of a const object --
    // that would be a hole straight through the const system:
    // int* leak { &c };    // ERROR if uncommented

    return 0;
}

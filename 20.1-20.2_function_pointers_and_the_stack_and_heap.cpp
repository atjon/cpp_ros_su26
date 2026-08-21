#include <iostream>
#include <string_view>

// 20.1 - Function Pointers
// 20.2 - The stack and the heap

// ------------------------ 20.1 FUNCTION POINTERS -----------------------------
//
// Functions live in memory too, so they have addresses -- and a pointer can
// hold one. That lets you pass BEHAVIOR as an argument, not just data.

int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }

// The syntax is genuinely awful. Read it inside-out:
//   int (*fn)(int, int)
//        ^^^ fn is a pointer...
//   ...to a function taking (int, int) and returning int.
//
// The parentheses around *fn are REQUIRED. Without them:
//   int *fn(int, int)   is a FUNCTION returning int*, a completely different thing.

// A type alias (10.7) makes this readable, and is what you should actually write:
using MathOp = int (*)(int, int);

// A function that takes a function -- this is a CALLBACK.
int applyTwice(MathOp op, int a, int b) {
    return op(op(a, b), b);
}

void describe(std::string_view name, MathOp op, int a, int b) {
    std::cout << "  " << name << "(" << a << ", " << b << ") = " << op(a, b) << "\n";
}

// a function pointer can have a DEFAULT ARGUMENT like any other parameter
int compute(int a, int b, MathOp op = add) {
    return op(a, b);
}

int main() {

    // ---------------- 20.1 using function pointers ---------------------------
    std::cout << "function pointers:\n";

    // the function's NAME implicitly converts to a pointer to it.
    // &add works too and means exactly the same thing.
    MathOp op { add };
    std::cout << "  op = add,      op(10, 3) = " << op(10, 3) << "\n";

    op = subtract;   // REPOINT it -- this is the whole point of the feature
    std::cout << "  op = subtract, op(10, 3) = " << op(10, 3) << "\n";

    op = multiply;
    std::cout << "  op = multiply, op(10, 3) = " << op(10, 3) << "\n";

    // calling through the pointer: op(...) works, and so does the older
    // explicit form (*op)(...). They're identical; prefer the short one.
    std::cout << "  explicit deref (*op)(10, 3) = " << (*op)(10, 3) << "\n";

    // the raw type without the alias, so you've seen it at least once:
    int (*uglyButValid)(int, int) { add };
    std::cout << "  raw syntax: " << uglyButValid(2, 2) << "\n";

    // ---------------- callbacks ----------------------------------------------
    std::cout << "\ncallbacks -- same function, different behavior:\n";
    describe("add", add, 8, 2);
    describe("subtract", subtract, 8, 2);
    describe("multiply", multiply, 8, 2);

    std::cout << "\napplyTwice(add, 5, 3)      = " << applyTwice(add, 5, 3) << "\n";
    std::cout << "applyTwice(multiply, 5, 3) = " << applyTwice(multiply, 5, 3) << "\n";

    std::cout << "\ncompute with a default op: " << compute(6, 4) << "\n";
    std::cout << "compute with multiply:     " << compute(6, 4, multiply) << "\n";

    // a null function pointer is possible, so CHECK before calling
    MathOp maybe { nullptr };
    std::cout << "\nnull function pointer is safe to test: "
              << (maybe ? "callable" : "null -- do not call") << "\n";

    // NOTE: in modern C++ you'd usually reach for a LAMBDA (20.6) or
    // std::function instead. A function pointer can't carry any state with it,
    // which is exactly the limitation lambdas with captures (20.7) remove.

    // ---------------- 20.2 THE STACK AND THE HEAP ----------------------------
    //
    // Your program's memory is divided into segments. The two you actively
    // manage are the STACK and the HEAP.
    //
    // THE STACK (the call stack):
    //   - holds function parameters, local variables, and return addresses
    //   - allocation is just moving a pointer, so it's extremely FAST
    //   - memory is reclaimed AUTOMATICALLY when a function returns
    //   - LIFO: each function call pushes a "stack frame", returning pops it
    //   - it is SMALL (commonly 1-8 MB) and its size is fixed at startup
    //   - overflowing it (deep recursion, huge local arrays) CRASHES the program
    //
    // THE HEAP (free store):
    //   - where 'new' allocates from
    //   - much LARGER -- effectively limited by available RAM
    //   - SLOWER: the allocator has to find and track a suitable block
    //   - memory is NOT reclaimed automatically. Forget to free it and you
    //     have a LEAK; free it twice or use it after freeing and you have UB
    //   - the object outlives the scope it was created in, which is the reason
    //     to use it at all

    std::cout << "\nstack vs heap:\n";

    int onStack { 42 };                 // automatic storage -- freed at scope exit
    std::cout << "  stack address: " << &onStack << "\n";

    int* onHeap { new int { 42 } };     // dynamic storage -- YOU own this now
    std::cout << "  heap address:  " << onHeap << "\n";
    std::cout << "  heap value:    " << *onHeap << "\n";

    delete onHeap;                      // MUST free it, or it leaks
    onHeap = nullptr;                   // and null it, so nobody uses it after (12.8)

    // notice how far apart the two addresses are -- different memory segments.

    // WHY YOU RARELY WRITE new/delete IN MODERN C++:
    // matching every new with exactly one delete, on every path including
    // early returns and exceptions, is genuinely hard to get right. RAII (15.4)
    // and smart pointers (22.5) do it for you. Chapter 22 replaces this entire
    // pattern with std::unique_ptr, and the manual version above becomes
    // something you only need to recognize in older code.

    // this is a stack allocation, and it's why huge local arrays are risky:
    // int enormous[10'000'000] { };   // likely STACK OVERFLOW if uncommented
    //                                 // the same array on the heap would be fine

    return 0;
}

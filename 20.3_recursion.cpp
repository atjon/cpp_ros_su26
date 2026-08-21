#include <iostream>
#include <string> // for std::to_string

// 20.3 - Recursion

// A recursive function calls itself. Every one needs a TERMINATION CONDITION
// (base case) that stops the chain -- without one it recurses until the stack
// overflows (20.2) and the program dies.

// ------------------------ THE CLASSIC ----------------------------------------

int factorial(int n) {
    if (n <= 1)              // BASE CASE -- always write this FIRST
        return 1;
    return n * factorial(n - 1);  // RECURSIVE CASE -- must move toward the base
}

// ------------------------ WATCHING THE STACK UNWIND --------------------------

int countdown(int n, int depth = 0) {
    // indent by depth so the call stack is visible in the output
    for (int i { 0 }; i < depth; ++i) std::cout << "  ";
    std::cout << "-> countdown(" << n << ") called\n";

    if (n <= 0) {
        for (int i { 0 }; i < depth; ++i) std::cout << "  ";
        std::cout << "   base case reached, unwinding\n";
        return 0;
    }

    int result { countdown(n - 1, depth + 1) + n };

    for (int i { 0 }; i < depth; ++i) std::cout << "  ";
    std::cout << "<- countdown(" << n << ") returns " << result << "\n";
    return result;
}

// ------------------------ WHERE NAIVE RECURSION GOES WRONG -------------------

// Fibonacci is the standard example of recursion being a TERRIBLE fit.
// fib(n) calls itself twice, so the call count grows exponentially --
// fib(30) makes over 2.6 million calls, and most recompute identical values.
int fibSlow(int n) {
    if (n <= 1) return n;
    return fibSlow(n - 1) + fibSlow(n - 2);
}

// MEMOIZATION: cache each answer the first time it's computed. Same recursive
// shape, but now each value is calculated exactly once.
int fibMemo(int n) {
    // a static local (7.x) persists between calls, so the cache survives
    static int cache[64] { };
    static bool cached[64] { };

    if (n <= 1) return n;
    if (cached[n]) return cache[n];   // already known -- stop recursing

    cache[n] = fibMemo(n - 1) + fibMemo(n - 2);
    cached[n] = true;
    return cache[n];
}

// ITERATIVE version -- no recursion at all, no stack growth, fastest of the three
int fibIterative(int n) {
    if (n <= 1) return n;
    int prev { 0 };
    int curr { 1 };
    for (int i { 2 }; i <= n; ++i) {
        int next { prev + curr };
        prev = curr;
        curr = next;
    }
    return curr;
}

// ------------------------ WHERE RECURSION IS THE RIGHT TOOL ------------------

// Recursion shines when the DATA is recursive. Reversing a number, walking a
// tree, or parsing nested structures are all naturally recursive shapes.
int sumDigits(int n) {
    if (n < 10) return n;               // base case: single digit
    return n % 10 + sumDigits(n / 10);  // last digit + the rest
}

// binary search is another naturally recursive shape
int binarySearch(const int arr[], int target, int low, int high) {
    if (low > high) return -1;          // base case: not found

    int mid { low + (high - low) / 2 }; // note: not (low+high)/2, which can overflow

    if (arr[mid] == target) return mid;
    if (arr[mid] < target)  return binarySearch(arr, target, mid + 1, high);
    return binarySearch(arr, target, low, mid - 1);
}

// a constexpr recursive function runs at COMPILE time (chapter F)
constexpr int powerOfTwo(int exp) {
    return (exp <= 0) ? 1 : 2 * powerOfTwo(exp - 1);
}

int main() {

    std::cout << "factorial:\n";
    for (int i { 1 }; i <= 6; ++i)
        std::cout << "  " << i << "! = " << factorial(i) << "\n";

    std::cout << "\nwatching the call stack (countdown from 4):\n";
    std::cout << "sum = " << countdown(4) << "\n";
    // notice EVERY call is pushed before ANY of them return. All 5 frames
    // exist simultaneously at the deepest point -- that's the memory cost
    // recursion pays that a loop doesn't.

    std::cout << "\nfibonacci, three ways:\n";
    std::cout << "  fibSlow(20)      = " << fibSlow(20) << "  (~13000 calls)\n";
    std::cout << "  fibMemo(20)      = " << fibMemo(20) << "  (~20 calls)\n";
    std::cout << "  fibIterative(20) = " << fibIterative(20) << "  (a plain loop)\n";
    std::cout << "  fibMemo(45)      = " << fibMemo(45)
              << "  (fibSlow(45) would take minutes)\n";

    std::cout << "\nnaturally recursive problems:\n";
    std::cout << "  sumDigits(93412) = " << sumDigits(93412) << "\n";

    const int sorted[] { 2, 5, 8, 12, 16, 23, 38, 56, 72, 91 };
    constexpr int size { sizeof(sorted) / sizeof(sorted[0]) };
    for (int target : { 23, 2, 91, 40 }) {
        int index { binarySearch(sorted, target, 0, size - 1) };
        std::cout << "  binarySearch(" << target << ") -> "
                  << (index >= 0 ? "index " + std::to_string(index) : "not found")
                  << "\n";
    }

    constexpr int p { powerOfTwo(10) };  // computed at COMPILE time
    std::cout << "\n  powerOfTwo(10) = " << p << " (compile-time recursion)\n";

    // BEST PRACTICE: prefer ITERATION unless the problem is naturally recursive.
    // Recursion is more readable for trees and nested structures, but each call
    // costs a stack frame, and deep recursion overflows a stack that's only a
    // few megabytes. A loop has neither problem.

    return 0;
}

#include <iostream>
#include <type_traits> // for std::common_type_t

// 11.8 - Function templates with multiple template types
// 11.9 - Non-type template parameters

// ------------------------ 11.8 MULTIPLE TYPE PARAMETERS ----------------------

// THE PROBLEM: with a single T, mixing types fails to deduce.
//   template <typename T> T larger(T a, T b);
//   larger(3, 7.5);  // ERROR: T can't be int AND double

// FIX 1: give each parameter its own type
template <typename T, typename U>
auto larger(T a, U b) {
    // 'auto' return matters here: if we wrote T, then larger(3, 7.5) would
    // truncate the double back down to int on the way out.
    return (a > b) ? a : b;
}

// FIX 2 (explicit about the result): std::common_type_t names the type both
// arguments convert to, so the return type is stated rather than deduced.
template <typename T, typename U>
std::common_type_t<T, U> largerExplicit(T a, U b) {
    return (a > b) ? a : b;
}

// C++20 abbreviated form -- each 'auto' parameter is secretly its own type
// parameter. Same thing as FIX 1, much less typing.
auto smaller(auto a, auto b) {
    return (a < b) ? a : b;
}

// ------------------------ 11.9 NON-TYPE TEMPLATE PARAMETERS ------------------

// A non-type template parameter (NTTP) is a VALUE baked in at compile time,
// not a type. It must be a constant expression -- known before the program runs.

template <int Size>
void describeBuffer() {
    std::cout << "  buffer of " << Size << " slots\n";
    // Size is usable as a constant expression here, so this is legal:
    int slots[Size] {}; // a real fixed-size array, sized at compile time
    std::cout << "  sizeof(slots) = " << sizeof(slots) << " bytes\n";
}

// NTTPs let you do work at COMPILE time that would otherwise cost runtime
template <int N>
constexpr int factorial() {
    if constexpr (N <= 1)
        return 1;
    else
        return N * factorial<N - 1>(); // recursion, resolved by the compiler
}

// C++20 lets you write 'auto' for the NTTP's type too
template <auto Value>
void showConstant() {
    std::cout << "  compile-time constant: " << Value << "\n";
}

int main() {

    std::cout << "multiple template types:\n";
    std::cout << "  larger(3, 7.5)          = " << larger(3, 7.5) << "\n";
    std::cout << "  larger(9.5, 2)          = " << larger(9.5, 2) << "\n";
    std::cout << "  largerExplicit(3, 7.5)  = " << largerExplicit(3, 7.5) << "\n";
    std::cout << "  smaller(3, 7.5)         = " << smaller(3, 7.5) << "\n";

    std::cout << "\nnon-type template parameters:\n";
    describeBuffer<4>();
    describeBuffer<16>();
    // describeBuffer<n>();  // ERROR if n is a runtime variable -- the value
    //                       // MUST be known at compile time

    // each distinct value makes a SEPARATE instantiation. describeBuffer<4>
    // and describeBuffer<16> are two different functions in the binary.

    constexpr int f5 { factorial<5>() };
    std::cout << "  factorial<5>() = " << f5 << "  (computed at compile time)\n";

    showConstant<42>();
    showConstant<'x'>();
    showConstant<3.5>(); // C++20: floating-point NTTPs are allowed now

    return 0;
}

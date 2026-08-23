#include <iostream>
#include <iterator>  // for std::size, std::begin, std::end

// 17.7 - Introduction to C-style arrays
// 17.8 - C-style array decay
// 17.9 - Pointer arithmetic and subscripting

// ------------------------ 17.7 C-STYLE ARRAYS --------------------------------
//
// The array type C++ inherited from C. It's built into the LANGUAGE rather
// than the library, so it needs no #include -- and gets no help either.
//
// You should prefer std::array or std::vector in new code. You still need to
// understand these because they appear constantly: string literals, argv,
// and every C API you'll ever call.

// The length must be a COMPILE-TIME CONSTANT. These are all legal:
int globalReadings[4] { };                     // zero-initialized
constexpr int kSize { 5 };
double voltages[kSize] { 3.3, 5.0 };           // rest are zeroed

// A function taking an array parameter... doesn't. See decay, below.
void takesArray(const int arr[], std::size_t length) {
    std::cout << "    inside: length passed in = " << length << '\n';
    // NOTE: this line makes the compiler emit -Wsizeof-array-argument.
    // That warning is the lesson: it's telling you the array already decayed.
    std::cout << "    sizeof(arr) = " << sizeof(arr)
              << " <- the size of a POINTER, not the array\n";
}

int main() {

    std::cout << "C-style arrays:\n";

    // length omitted -- deduced from the initializer list
    int readings[] { 42, 17, 91, 8 };

    std::cout << "  readings[2] = " << readings[2] << '\n';
    // subscripting works the same as std::array/vector, and has the same
    // rule: valid indices are 0 .. length-1, and going outside is UB.
    // Unlike std::array there is no .at(), so there is NO checked access.

    // getting the LENGTH: an array knows its own size only while it's still
    // a real array (see decay below).
    std::cout << "  sizeof(readings) = " << sizeof(readings) << " bytes\n";
    std::cout << "  length via sizeof trick: "
              << sizeof(readings) / sizeof(readings[0]) << '\n';
    std::cout << "  length via std::size (C++17, preferred): "
              << std::size(readings) << '\n';

    // std::size is better because it's a COMPILE ERROR on a decayed pointer,
    // whereas the sizeof trick silently produces a garbage answer.

    // ---------------- 17.8 ARRAY DECAY ----------------------------------------
    //
    // THE defining weirdness of C-style arrays: in almost any expression, an
    // array IMPLICITLY CONVERTS to a pointer to its first element. That
    // conversion is called DECAY, and it's why array parameters don't work.
    //
    // The array type `int[4]` becomes the pointer type `int*`. The LENGTH
    // information is thrown away and cannot be recovered.

    std::cout << "\ndecay:\n";

    int* ptr { readings };    // no & needed -- the array decays on its own
    std::cout << "  ptr points at readings[0] = " << *ptr << '\n';
    std::cout << "  sizeof(readings) = " << sizeof(readings)
              << " but sizeof(ptr) = " << sizeof(ptr) << '\n';

    // This is why the function above can't know the length -- by the time the
    // array arrives, it's just a pointer. All three of these declare the
    // SAME parameter, an `int*`:
    //
    //     void f(int arr[10]);   // the 10 is IGNORED by the compiler
    //     void f(int arr[]);
    //     void f(int* arr);
    //
    // So you must pass the length separately. Every C array API does this.

    std::cout << "  calling takesArray(readings, std::size(readings)):\n";
    takesArray(readings, std::size(readings));

    // Decay is also why you can't copy or assign C-style arrays:
    //   int other[4] { };
    //   other = readings;    // ERROR: both sides decay to pointers
    //
    // and why passing one by value is impossible -- you always get a pointer.
    // std::array and std::vector both fix all of this by being real objects.

    // ---------------- 17.9 POINTER ARITHMETIC ---------------------------------
    //
    // Adding an integer to a pointer moves it by that many ELEMENTS, not
    // bytes. The compiler scales by sizeof(the pointed-to type) for you.

    std::cout << "\npointer arithmetic:\n";

    std::cout << "  *(ptr + 0) = " << *(ptr + 0) << '\n';
    std::cout << "  *(ptr + 2) = " << *(ptr + 2) << '\n';

    std::cout << "  ptr     is at " << static_cast<const void*>(ptr) << '\n';
    std::cout << "  ptr + 1 is at " << static_cast<const void*>(ptr + 1)
              << " (moved by sizeof(int) = " << sizeof(int) << " bytes)\n";

    // THE BIG REVEAL: subscripting IS pointer arithmetic.
    //     arr[n]  is defined as  *((arr) + (n))
    // That's the entire definition. Which explains a few oddities:

    std::cout << "  readings[2] == *(readings + 2): "
              << (readings[2] == *(readings + 2)) << '\n';

    // and, absurdly but legally, since a+n == n+a:
    // (the compiler warns "self-comparison always true" here -- which is
    //  itself the proof that it considers the two forms identical)
    std::cout << "  2[readings] == readings[2]: "
              << (2[readings] == readings[2]) << " (yes, really)\n";

    // It also explains why there's no bounds checking: `arr[999]` is just
    // "add 999 elements' worth of bytes and dereference." The compiler has no
    // length to check against.

    // ---------------- traversing with pointers --------------------------------
    std::cout << "\n  traversal by pointer:\n    ";
    for (int* p { std::begin(readings) }; p != std::end(readings); ++p)
        std::cout << *p << ' ';
    std::cout << '\n';

    // std::end points ONE PAST the last element. That address is legal to
    // form and compare against, but dereferencing it is UB. This half-open
    // [begin, end) convention is used by the entire standard library.

    std::cout << "  distance from begin to end = "
              << std::end(readings) - std::begin(readings) << " elements\n";
    // subtracting two pointers into the same array gives the element COUNT
    // between them (as a signed std::ptrdiff_t), not a byte count.

    // ---------------- and the modern equivalent -------------------------------
    std::cout << "\n  a range-based for works on C-style arrays too:\n    ";
    for (int r : readings)      // only because the array hasn't decayed yet --
        std::cout << r << ' ';  // this would NOT compile inside takesArray()
    std::cout << '\n';

    std::cout << "\n  BEST PRACTICE: use std::array (fixed length) or\n"
                 "  std::vector (dynamic). They don't decay, they know their\n"
                 "  own length, they can be copied and returned, and they\n"
                 "  offer checked access via .at().\n";

    return 0;
}

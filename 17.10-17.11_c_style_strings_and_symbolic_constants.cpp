#include <iostream>
#include <cstring>     // for std::strlen, std::strcpy, std::strcmp
#include <iterator>    // for std::size
#include <string>
#include <string_view>

// 17.10 - C-style strings
// 17.11 - C-style string symbolic constants

// ------------------------ 17.10 WHAT A C-STYLE STRING IS ---------------------
//
// A C-style string is just a C-style array of char, with one extra rule:
// the last character must be a NULL TERMINATOR, '\0' (a char with value 0).
//
//     char name[] { "bot" };
//
// is really an array of FOUR chars: 'b', 'o', 't', '\0'.
//
// Everything that's painful about C-style strings follows from that one
// design choice: the length isn't stored anywhere, so finding it means
// SCANNING for the terminator -- an O(n) walk every single time.

void printBytes(const char* label, const char* str, std::size_t count) {
    std::cout << "  " << label << ": ";
    for (std::size_t i { 0 }; i < count; ++i) {
        if (str[i] == '\0') std::cout << "\\0 ";
        else                std::cout << str[i] << "  ";
    }
    std::cout << '\n';
}

int main() {

    std::cout << "the null terminator:\n";

    char name[] { "bot" };

    std::cout << "  char name[] { \"bot\" } has "
              << std::size(name) << " elements for 3 letters\n";
    printBytes("bytes", name, std::size(name));

    std::cout << "  std::strlen(name) = " << std::strlen(name)
              << "  <- counts characters, scanning for '\\0'\n";
    std::cout << "  std::size(name)   = " << std::size(name)
              << "  <- array elements, INCLUDING the terminator\n";
    // These two numbers are different, and mixing them up is the source of
    // most buffer overruns in C code.

    // ---------------- the ways this goes wrong --------------------------------

    std::cout << "\nwhat makes them dangerous:\n";

    // 1. FIXED CAPACITY. The array can't grow. Writing past the end is UB,
    //    and there is no check -- this is the classic buffer overflow.
    char buffer[8] { };
    std::strcpy(buffer, "short");        // fits (5 + terminator = 6 of 8)
    std::cout << "  buffer after strcpy: \"" << buffer << "\"\n";
    //
    //    std::strcpy(buffer, "this is far too long");   // silently corrupts
    //                                                    // whatever follows
    std::cout << "    strcpy does NOT check the destination size.\n";

    // 2. NO ASSIGNMENT. Arrays don't assign (17.8), so this is illegal:
    //      buffer = "other";            // ERROR
    //    You must copy character by character, via strcpy and friends.

    // 3. COMPARISON DOESN'T WORK THE WAY IT LOOKS.
    const char* a { "left" };
    const char* b { "left" };
    std::cout << "  a == b compares POINTERS, not text: " << (a == b)
              << " (whatever the compiler happened to do)\n";
    std::cout << "  std::strcmp(a, b) == 0 is the real test: "
              << (std::strcmp(a, b) == 0) << '\n';
    // strcmp returns 0 for equal, <0 or >0 for ordering. The "== 0" reads
    // backwards, which is another reliable source of bugs.

    // 4. THE DANGLING TRAP with std::cin.
    //      char small[4] { };
    //      std::cin >> small;      // typing more than 3 chars overruns it
    //    (std::string handles this correctly and automatically.)

    // ---------------- 17.11 SYMBOLIC CONSTANTS --------------------------------
    //
    // A STRING LITERAL like "bot" is itself a C-style string constant. Its
    // type is `const char[4]`, and it lives in read-only memory for the
    // ENTIRE program (it has static duration).
    //
    // That last part matters: unlike a local array, a string literal does NOT
    // die at the end of its scope.

    std::cout << "\nstring literals:\n";

    // Two ways to make a named constant, and they are NOT the same:

    const char arrayForm[] { "front_left" };  // a COPY, one array per object
    const char* ptrForm { "front_left" };     // points AT the literal, no copy

    std::cout << "  const char arrayForm[]: " << arrayForm
              << " (sizeof " << sizeof(arrayForm) << ")\n";
    std::cout << "  const char* ptrForm:    " << ptrForm
              << " (sizeof " << sizeof(ptrForm) << " -- just the pointer)\n";

    // Because literals have static duration, returning a pointer to one from
    // a function is SAFE -- unusually for pointers:
    //
    //     const char* statusName(int code) {
    //         if (code == 0) return "ok";      // fine! the literal outlives
    //         return "fault";                  // the function
    //     }

    // But a pointer to a LOCAL array is not:
    //
    //     const char* broken() {
    //         char local[] { "oops" };
    //         return local;                    // DANGLES -- local array dies
    //     }
    //
    // The difference is subtle and invisible at the call site. This is exactly
    // the kind of trap std::string and std::string_view remove.

    // ---------------- printing quirk ------------------------------------------
    //
    // std::cout treats char* SPECIALLY: it prints the string, not the address.
    // To see the address you must cast.

    std::cout << "\n  cout prints char* as text: " << ptrForm << '\n';
    std::cout << "  its actual address:        "
              << static_cast<const void*>(ptrForm) << '\n';

    // and a lone char pointer with no terminator anywhere runs off into
    // memory printing garbage until it randomly finds a zero byte.

    // ---------------- what to use instead --------------------------------------

    std::cout << "\nthe modern replacements:\n";

    std::string owned { "front_left" };        // owns and manages its memory
    owned += "_motor";                          // grows on demand
    std::cout << "  std::string:      " << owned
              << " (length " << owned.length() << ", O(1))\n";

    std::string_view viewed { "front_left" };  // a non-owning VIEW, no copy
    std::cout << "  std::string_view: " << viewed
              << " (length " << viewed.length() << ", no allocation)\n";

    // both compare with plain == , the way you'd expect
    std::cout << "  owned == \"front_left_motor\": "
              << (owned == "front_left_motor") << " -- no strcmp needed\n";

    // and both convert TO a C-style string when you must call a C API:
    std::cout << "  owned.c_str() for C APIs: " << owned.c_str() << '\n';
    // NOTE: string_view has NO c_str(), because it isn't guaranteed to be
    // null-terminated -- it's just a pointer plus a length.

    std::cout << "\n  BEST PRACTICE: std::string to OWN text, std::string_view\n"
                 "  to pass read-only text around. Use C-style strings only\n"
                 "  when a C API forces you to.\n";

    return 0;
}

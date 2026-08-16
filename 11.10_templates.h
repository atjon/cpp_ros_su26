#ifndef TEMPLATES_11_10_H
#define TEMPLATES_11_10_H

#include <iostream>
#include <string_view>

// 11.10 - Using function templates in multiple files (the HEADER)
//
// THE KEY RULE: a template's FULL DEFINITION must be visible in every
// translation unit that instantiates it.
//
// Why: the compiler can't stamp out larger<int> unless it can see the body.
// Each .cpp is compiled separately, so a declaration alone in a header leaves
// the other .cpp with a recipe it can't cook -> "undefined reference" at LINK
// time, which is a confusing error because the code looked fine.
//
// So unlike normal functions (declaration in .h, definition in .cpp),
// templates put the ENTIRE DEFINITION in the header.

// This does NOT violate the ODR. Templates are implicitly inline, so the
// linker is allowed to see identical instantiations in multiple object files
// and quietly collapse them into one.

template <typename T>
T larger(T a, T b) {
    return (a > b) ? a : b;
}

template <typename T>
void printReading(std::string_view label, T value) {
    std::cout << "  " << label << ": " << value << "\n";
}

#endif

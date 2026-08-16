#include <iostream>
#include <string>

// 12.10 - Pass by address
// 12.11 - Pass by address (part 2)
// 12.12 - Return by reference and return by address
// 12.13 - In and out parameters

// ------------------------ 12.10 PASS BY ADDRESS ------------------------------

// Pass by address = pass a POINTER. The function gets a copy of the address,
// so it can modify what's pointed AT, but not the caller's pointer itself.
void boost(int* rpm) {
    if (!rpm) return;   // pass by address can receive nullptr, so CHECK
    *rpm += 500;
}

// This is a common bug: the pointer parameter is a COPY, so reseating it
// inside the function has no effect on the caller.
void tryToReseat(int* ptr) {
    static int elsewhere { 999 };
    ptr = &elsewhere;   // only changes the LOCAL copy of the pointer
    (void)ptr;          // silence the unused-value warning
}

// If you genuinely need to change the caller's pointer, pass the POINTER
// BY REFERENCE -- note the & after the *
void actuallyReseat(int*& ptr) {
    static int elsewhere { 999 };
    ptr = &elsewhere;   // now the caller's pointer really is reseated
}

// ------------------------ 12.11 OPTIONAL ARGUMENTS ---------------------------

// The classic reason to pass by address: nullptr means "no argument given".
// std::optional (12.15) is usually the better modern answer, but you'll see
// this pattern constantly in existing code and in C APIs.
void report(int reading, const std::string* label = nullptr) {
    if (label)
        std::cout << "  " << *label << " = " << reading << "\n";
    else
        std::cout << "  (unlabeled) = " << reading << "\n";
}

// ------------------------ 12.12 RETURN BY REFERENCE --------------------------

// Return by value COPIES the return value. For big objects that's wasteful.
// Return by reference returns the object ITSELF -- but the object must
// OUTLIVE the function, or you hand back a dangling reference.

// SAFE: the static lives for the whole program
const std::string& safeName() {
    static const std::string name { "front_lidar" };
    return name;
}

// SAFE: the referent was passed in by the caller, so the caller owns it
int& pickLarger(int& a, int& b) {
    return (a > b) ? a : b;
}

// DANGEROUS -- do not do this:
// const std::string& brokenName() {
//     std::string local { "oops" };
//     return local;      // local dies at the closing brace
// }                      // caller gets a reference to nothing. UB.
//
// Equally broken: returning a reference to a BY-VALUE parameter, since
// the parameter is also destroyed when the function returns.

// ------------------------ 12.13 IN AND OUT PARAMETERS ------------------------
//
// IN parameter:     the function reads it        -> by value, or by const&
// OUT parameter:    the function writes it       -> by reference (or address)
// IN/OUT parameter: the function reads AND writes -> by reference

// an OUT parameter. The big downside is at the CALL SITE: nothing about
// "calibrate(sensor, offset)" tells you that offset gets modified.
void calibrate(int reading, int& offsetOut) {
    offsetOut = reading - 100;
}

// an IN/OUT parameter -- read and modified
void clampRpm(int& rpm) {
    if (rpm > 5000) rpm = 5000;
    if (rpm < 0)    rpm = 0;
}

int main() {

    // ---------------- 12.10 pass by address ----------------------------------
    int rpm { 1000 };
    boost(&rpm);                       // must explicitly take the address
    std::cout << "after boost: " << rpm << "\n";
    boost(nullptr);                    // legal -- the function handles it

    int value { 1 };
    int* p { &value };
    tryToReseat(p);
    std::cout << "after tryToReseat, *p = " << *p << " (unchanged -- copy)\n";
    actuallyReseat(p);
    std::cout << "after actuallyReseat, *p = " << *p << " (reseated -- int*&)\n";

    // ---------------- 12.11 optional arguments -------------------------------
    std::cout << "\noptional label:\n";
    std::string label { "battery_mv" };
    report(11850, &label);
    report(11850);                     // no label -> defaults to nullptr

    // ---------------- 12.12 return by reference ------------------------------
    std::cout << "\nreturn by reference:\n";
    std::cout << "  safeName() = " << safeName() << " (no copy made)\n";

    // careful: assigning a returned reference to a NON-reference makes a COPY
    std::string copy { safeName() };        // this copies
    const std::string& alias { safeName() }; // this doesn't
    std::cout << "  copy=" << copy << " alias=" << alias << "\n";

    // because pickLarger returns an lvalue reference, the call is an LVALUE
    // and can be assigned to -- same idea as 12.2
    int a { 3 };
    int b { 9 };
    pickLarger(a, b) = 100;
    std::cout << "  after pickLarger(a,b) = 100: a=" << a << " b=" << b << "\n";

    // ---------------- 12.13 in and out parameters ----------------------------
    std::cout << "\nin/out parameters:\n";
    int offset { };
    calibrate(137, offset);   // nothing here hints that offset gets written
    std::cout << "  offset written by calibrate: " << offset << "\n";

    int speed { 7200 };
    clampRpm(speed);          // in/out: read and modified
    std::cout << "  clamped 7200 -> " << speed << "\n";

    // BEST PRACTICE: prefer RETURNING a value over using an out parameter.
    // Out parameters are hard to read at the call site and can't be used in
    // expressions. Reach for them mainly when returning multiple things and
    // a struct would be overkill.

    return 0;
}

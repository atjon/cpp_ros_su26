#include <iostream>
#include <string_view>

// 11.4 - Deleting functions
// 11.5 - Default arguments

// ------------------------ 11.4 DELETING FUNCTIONS ----------------------------

// Sometimes an overload resolves in a way that COMPILES but is nonsense.
// printRpm('a') would happily promote 'a' to int and print 97.
// '= delete' makes a candidate exist for overload resolution but makes
// CHOOSING it a compile error. That's the point: it still participates,
// so it can win the match and then be rejected loudly.

void printRpm(int rpm) {
    std::cout << "  rpm = " << rpm << "\n";
}

// "you may not call this with a char" -- catches printRpm('a') at compile time
void printRpm(char) = delete;

// "you may not call this with a bool" -- catches printRpm(true)
void printRpm(bool) = delete;

// A nuclear option: delete ALL overloads except the ones you explicitly wrote,
// by making a deleted function template catch everything else.
void setSpeed(double speed) {
    std::cout << "  speed = " << speed << "\n";
}
template <typename T>
void setSpeed(T) = delete; // anything that isn't already an exact double: rejected

// ------------------------ 11.5 DEFAULT ARGUMENTS -----------------------------

// A default argument is used when the caller omits that argument.
// RULES:
//  - defaults must be the RIGHTMOST parameters (you can't skip a middle one)
//  - the default is declared exactly ONCE, in the forward declaration if there
//    is one, NOT repeated in the definition
//  - the value is filled in at the CALL SITE at compile time

void logEvent(std::string_view msg, int level = 1, bool timestamped = true);

// notice: NO defaults repeated here, only in the declaration above.
void logEvent(std::string_view msg, int level, bool timestamped) {
    std::cout << "  [" << level << "] " << msg
              << (timestamped ? "  (stamped)" : "") << "\n";
}

// void bad(int a = 5, int b);  // ERROR: a default can't be followed by a non-default

// DEFAULTS + OVERLOADS = easy ambiguity. These two look distinct but
// crash into each other the moment someone calls arm(1):
//
// void arm(int id);
// void arm(int id, int mode = 0);   // arm(1) matches BOTH -> ambiguous

int main() {

    printRpm(1200); // fine, exact int match

    // printRpm('a');  // ERROR if uncommented: "use of deleted function"
    //                 // WITHOUT the = delete, this would silently print 97
    // printRpm(true); // ERROR if uncommented: would have printed 1

    // you can still opt in deliberately with a cast -- explicit is the point
    printRpm(static_cast<int>('a'));

    std::cout << "\nsetSpeed:\n";
    setSpeed(2.5);  // exact double, allowed
    // setSpeed(2);    // ERROR if uncommented: the deleted template catches int
    // setSpeed(2.5f); // ERROR if uncommented: catches float too
    setSpeed(static_cast<double>(2)); // explicit conversion is allowed

    std::cout << "\ndefault arguments:\n";
    logEvent("motor started");            // uses level=1, timestamped=true
    logEvent("battery low", 3);           // uses timestamped=true
    logEvent("shutdown", 5, false);       // nothing defaulted

    // NOTE: because defaults are baked in at the CALL SITE, changing a default
    // in a header requires RECOMPILING every file that calls it -- not just
    // relinking. Worth knowing before you change one in a shared library.

    return 0;
}

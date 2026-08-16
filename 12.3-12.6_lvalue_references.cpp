#include <iostream>
#include <string>

// 12.3 - Lvalue references
// 12.4 - Lvalue references to const
// 12.5 - Pass by lvalue reference
// 12.6 - Pass by const lvalue reference

// ------------------------ 12.3 LVALUE REFERENCES -----------------------------
//
// A reference is an ALIAS for an existing object. Not a copy, not a pointer --
// another name for the same storage.
//
// RULES that make references different from pointers:
//   - MUST be initialized when declared (no null references)
//   - can NEVER be reseated to refer to something else afterwards
//   - has no separate identity: &ref gives the address of the REFERENT

// ------------------------ 12.5 PASS BY LVALUE REFERENCE ----------------------

// Pass by value COPIES. For an int that's free; for a big object it isn't.
// Pass by reference doesn't copy, and lets the function MODIFY the caller's object.
void spinUp(int& rpm) {   // note the & -- rpm is an alias for the caller's variable
    rpm += 500;           // this modifies the ORIGINAL
}

void spinUpCopy(int rpm) { // no & -- this gets a copy
    rpm += 500;            // modifies only the local copy; caller sees nothing
}

// ------------------------ 12.6 PASS BY CONST LVALUE REFERENCE ----------------

// const& = "don't copy this, and I promise not to change it".
// This is the default choice for passing anything expensive to copy.
void printName(const std::string& name) {
    std::cout << "  name: " << name << " (" << name.size() << " chars)\n";
    // name = "x"; // ERROR if uncommented: it's const
}

// non-const references can ONLY bind to modifiable lvalues.
// const references are far more flexible -- they bind to almost anything.
void showValue(const int& v) {
    std::cout << "  value: " << v << "\n";
}

int main() {

    // ---------------- 12.3 basic references ----------------------------------
    int rpm { 1000 };
    int& rpmRef { rpm }; // rpmRef IS rpm, under a different name

    rpmRef = 2000;
    std::cout << "rpm = " << rpm << " (changed through the reference)\n";

    rpm = 3000;
    std::cout << "rpmRef = " << rpmRef << " (sees the change both ways)\n";

    // they share one address -- there is no separate reference object
    std::cout << "&rpm == &rpmRef ? " << (&rpm == &rpmRef) << "\n";

    // int& bad;          // ERROR if uncommented: must be initialized
    // int& bad2 { 5 };   // ERROR if uncommented: 5 is an rvalue

    // RESEATING IS IMPOSSIBLE. This looks like it repoints the reference,
    // but it actually ASSIGNS the value of other into rpm:
    int other { 42 };
    rpmRef = other;      // rpm becomes 42; rpmRef still refers to rpm
    std::cout << "after rpmRef = other: rpm=" << rpm << " other=" << other << "\n";
    other = 7;
    std::cout << "after other = 7:      rpm=" << rpm << " other=" << other << "\n";
    //                                  ^ rpm did NOT follow other. Not reseated.

    // ---------------- 12.4 const references ----------------------------------
    const int maxRpm { 8000 };
    const int& maxRef { maxRpm }; // a const object needs a const reference
    // int& badRef { maxRpm };    // ERROR if uncommented: would discard const

    // you can bind a CONST reference to a non-const object -- a read-only view
    int volts { 12 };
    const int& voltsView { volts };
    volts = 11;                     // fine through the original
    // voltsView = 11;              // ERROR if uncommented: not through the view
    std::cout << "voltsView = " << voltsView << "\n";

    // THE SPECIAL RULE: a const reference can bind to an RVALUE, and doing so
    // EXTENDS that temporary's lifetime to match the reference.
    const int& literalRef { 500 }; // the temporary 500 now lives as long as literalRef
    std::cout << "literalRef = " << literalRef << " (bound to a literal!)\n";

    // ---------------- 12.5 pass by reference ---------------------------------
    int motor { 1000 };
    spinUpCopy(motor);
    std::cout << "\nafter spinUpCopy: " << motor << " (unchanged -- got a copy)\n";
    spinUp(motor);
    std::cout << "after spinUp:     " << motor << " (changed -- got a reference)\n";

    // spinUp(5); // ERROR if uncommented: a non-const int& can't bind to an rvalue.
    //            // This is actually a FEATURE: modifying a temporary is pointless,
    //            // so the compiler blocks it.

    // ---------------- 12.6 const reference flexibility -----------------------
    std::cout << "\nconst& binds to everything:\n";
    showValue(motor);      // modifiable lvalue -- ok
    showValue(maxRpm);     // const lvalue      -- ok
    showValue(99);         // rvalue literal    -- ok
    showValue(motor + 1);  // rvalue temporary  -- ok

    std::string label { "lidar_front" };
    printName(label);   // no copy of the string is made
    printName("imu");   // a temporary std::string is created and bound

    // BEST PRACTICE (the rule of thumb worth memorizing):
    //   - cheap to copy (int, double, pointer, std::string_view) -> pass BY VALUE
    //   - expensive to copy (std::string, vector, big structs)   -> pass BY CONST&
    //   - need to modify the caller's object                     -> pass BY &

    return 0;
}

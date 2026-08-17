#include <iostream>
#include <string>

// 13.7 - Introduction to structs, members, and member selection
// 13.8 - Struct aggregate initialization
// 13.9 - Default member initialization

// ------------------------ 13.7 STRUCTS ---------------------------------------
//
// A struct bundles related variables into one type. Each variable inside is a
// DATA MEMBER. Without structs you end up passing 5 loose parameters that all
// describe one thing -- structs make "one thing" an actual type.

struct Waypoint {
    double x;      // data members. Note: NOT initialized by default!
    double y;
    double heading;
}; // semicolon, always

// ------------------------ 13.9 DEFAULT MEMBER INITIALIZERS -------------------
//
// You can give members a default value right in the definition. Any member
// left out of an initializer list then falls back to this instead of garbage.

struct MotorConfig {
    int id { };              // value-initialized -> 0
    double maxRpm { 5000.0 }; // explicit default
    bool reversed { false };
    std::string label { "unnamed" };
};

// BEST PRACTICE: give every member a default initializer. It makes the type
// impossible to leave uninitialized by accident, which is one of the most
// common sources of garbage-value bugs.

int main() {

    // ---------------- 13.7 member selection ----------------------------------
    Waypoint start; // NO initializer -> members hold GARBAGE.
                    // Waypoint has no default member initializers, so this is
                    // default-initialization, which for doubles means
                    // "whatever was in that memory". Reading it is UB.

    // the . operator selects a member
    start.x = 0.0;
    start.y = 0.0;
    start.heading = 90.0;

    std::cout << "start: (" << start.x << ", " << start.y
              << ") heading " << start.heading << "\n";

    // ---------------- 13.8 aggregate initialization --------------------------
    //
    // A struct is an AGGREGATE (no private members, no user-declared
    // constructors, no inheritance), so it can be initialized with a brace list.
    // Members are filled IN DECLARATION ORDER -- position matters, names don't.

    Waypoint goal { 10.0, 5.0, 180.0 }; // x=10, y=5, heading=180
    std::cout << "goal:  (" << goal.x << ", " << goal.y
              << ") heading " << goal.heading << "\n";

    // fewer initializers than members: the REST are VALUE-initialized (zeroed).
    // This is why brace-init is safer than leaving it off entirely.
    Waypoint partial { 3.0 };  // x=3.0, y=0.0, heading=0.0
    std::cout << "partial: (" << partial.x << ", " << partial.y
              << ") heading " << partial.heading << "\n";

    Waypoint zeroed { };       // ALL members value-initialized to 0
    std::cout << "zeroed: (" << zeroed.x << ", " << zeroed.y
              << ") heading " << zeroed.heading << "\n";

    // Waypoint tooMany { 1.0, 2.0, 3.0, 4.0 }; // ERROR if uncommented:
    //                                          // too many initializers

    // brace init also catches NARROWING, same as with fundamental types
    // Waypoint narrow { 1, 2, 3.5f };  // ints -> double is fine (widening)
    //                                  // but double -> int would be an error

    // ---------------- DESIGNATED INITIALIZERS (C++20) ------------------------
    //
    // Name the members explicitly. Far more readable, and it won't silently
    // break if someone reorders the struct's members later.
    // Restriction: they must appear in DECLARATION ORDER, and you can't skip
    // around out of sequence.

    Waypoint named { .x = 7.0, .y = 2.0, .heading = 45.0 };
    std::cout << "\nnamed: (" << named.x << ", " << named.y
              << ") heading " << named.heading << "\n";

    // you can omit members -- omitted ones get their default/value-init
    Waypoint partialNamed { .y = 9.0 }; // x and heading -> 0.0
    std::cout << "partialNamed: (" << partialNamed.x << ", " << partialNamed.y
              << ") heading " << partialNamed.heading << "\n";

    // Waypoint wrongOrder { .y = 1.0, .x = 2.0 }; // ERROR if uncommented:
    //                                             // must follow declaration order

    // ---------------- 13.9 defaults in action --------------------------------
    std::cout << "\ndefault member initializers:\n";

    MotorConfig defaults { }; // every member falls back to its default
    std::cout << "  id=" << defaults.id
              << " maxRpm=" << defaults.maxRpm
              << " reversed=" << defaults.reversed
              << " label=" << defaults.label << "\n";

    // explicit values OVERRIDE the defaults; anything omitted keeps its default
    MotorConfig custom { .id = 3, .maxRpm = 9000.0, .label = "drive_left" };
    std::cout << "  id=" << custom.id
              << " maxRpm=" << custom.maxRpm
              << " reversed=" << custom.reversed  // still the default, false
              << " label=" << custom.label << "\n";

    // NOTE: MotorConfig defaults;  (no braces) still uses the default member
    // initializers, so it is safe -- unlike Waypoint, which has none.
    MotorConfig noBraces;
    std::cout << "  no braces, still safe: maxRpm=" << noBraces.maxRpm << "\n";

    // ---------------- assignment between structs -----------------------------
    Waypoint copy { goal }; // memberwise copy
    copy.x = 999.0;
    std::cout << "\ncopy.x=" << copy.x << " but goal.x=" << goal.x
              << " (independent objects)\n";

    return 0;
}

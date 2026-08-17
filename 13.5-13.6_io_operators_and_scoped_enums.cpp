#include <iostream>
#include <string_view>

// 13.5 - Introduction to overloading the I/O operators
// 13.6 - Scoped enumerations (enum classes)

// ------------------------ 13.6 SCOPED ENUMERATIONS ---------------------------
//
// 'enum class' fixes the two big problems with unscoped enums:
//   1. the enumerators do NOT leak into the surrounding scope
//   2. they do NOT implicitly convert to integers
//
// ("class" here is a confusing keyword choice -- a scoped enum is not a class.)
//
// BEST PRACTICE: prefer enum class. Reach for an unscoped enum only when you
// genuinely want the implicit integer conversion (e.g. indexing into an array).

enum class MotorSide {
    left,
    right,
    // no prefix needed on the names -- they're scoped inside MotorSide,
    // so MotorSide::left can't collide with anything
};

enum class WheelSide {
    left,    // no collision with MotorSide::left -- different scope
    right,
};

enum class Severity {
    info,
    warning,
    critical,
};

// ------------------------ 13.5 OVERLOADING operator<< ------------------------
//
// Rather than calling toString() by hand everywhere (13.4), teach std::cout
// how to print the type directly.
//
// The signature to memorize:
//   std::ostream& operator<<(std::ostream& out, const YourType& value)
//
// Why each piece:
//   - takes the stream BY REFERENCE, since streams can't be copied
//   - RETURNS the stream by reference, which is what makes chaining work:
//     (std::cout << a) << b -- the first call hands the stream to the second
//   - takes the value by const& (by value is fine for something enum-sized)

constexpr std::string_view toString(MotorSide side) {
    switch (side) {
        case MotorSide::left:  return "left";
        case MotorSide::right: return "right";
        default:               return "???";
    }
}

std::ostream& operator<<(std::ostream& out, MotorSide side) {
    return out << toString(side); // return the stream so << can chain
}

std::ostream& operator<<(std::ostream& out, Severity level) {
    switch (level) {
        case Severity::info:     return out << "INFO";
        case Severity::warning:  return out << "WARN";
        case Severity::critical: return out << "CRIT";
        default:                 return out << "????";
    }
}

int main() {

    // ---------------- 13.6 scoped enums --------------------------------------
    MotorSide side { MotorSide::left };

    // MotorSide side2 { left };  // ERROR if uncommented: 'left' isn't in scope.
    //                            // The qualification is MANDATORY, and that's
    //                            // the feature, not a nuisance.

    // std::cout << side;         // without the operator<< below this would be
    //                            // an ERROR -- no implicit conversion to int,
    //                            // so the stream has no idea what to do.

    // to get the number out you must be explicit
    std::cout << "static_cast<int>(MotorSide::left)  = "
              << static_cast<int>(MotorSide::left) << "\n";

    // C++23 adds std::to_underlying for exactly this; before that, static_cast
    // to the underlying type is the idiom.

    // the NO-IMPLICIT-CONVERSION rule catches real mistakes:
    // if (side == WheelSide::left) {}    // ERROR if uncommented: different types.
    //                                    // With unscoped enums both sides would
    //                                    // decay to int and this would COMPILE,
    //                                    // silently comparing 0 == 0 -> true.

    // comparing within the same enum is fine
    std::cout << "side == MotorSide::left ? " << (side == MotorSide::left) << "\n";

    // ---------------- 13.5 the payoff ----------------------------------------
    std::cout << "\nwith operator<< overloaded:\n";
    std::cout << "  side is " << side << "\n";
    std::cout << "  other side is " << MotorSide::right << "\n";

    // chaining works because each call returns the stream
    std::cout << "  " << Severity::info << " / "
              << Severity::warning << " / "
              << Severity::critical << "\n";

    // now enums compose naturally into normal output code
    std::cout << "  [" << Severity::critical << "] motor "
              << MotorSide::right << " stalled\n";

    // ---------------- using enum (C++20) -------------------------------------
    // if the qualification really is too verbose in a tight scope, C++20 lets
    // you pull the enumerators in locally -- WITHOUT giving up type safety
    {
        using enum Severity;   // only inside these braces
        Severity s { warning };  // unqualified is now allowed here
        std::cout << "\n  using enum, unqualified: " << s << "\n";
    }
    // out here, 'warning' is out of scope again

    return 0;
}

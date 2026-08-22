#include <iostream>
#include <vector>
#include <string_view>

// 16.9 - Array indexing and length using enumerators

// ------------------------ THE PROBLEM ----------------------------------------
//
// Magic-number indices are unreadable and easy to get wrong:
//
//     std::cout << motorSpeeds[2];    // ...which motor is 2?
//
// Enumerators (13.2) give the indices NAMES, so the code says what it means.

// ------------------------ UNSCOPED ENUMS AS INDICES --------------------------
//
// An UNSCOPED enum is used here on purpose. Its enumerators implicitly convert
// to int, so they can be used as subscripts directly -- which is exactly the
// behaviour we normally avoid, but is the whole point in this one case.
//
// Putting it in a namespace keeps the names from leaking everywhere while
// preserving the implicit conversion.

namespace Motor {
    enum Type {
        frontLeft,
        frontRight,
        rearLeft,
        rearRight,

        // THE COUNT TRICK: because enumerators auto-increment from 0, an extra
        // enumerator placed LAST automatically equals the number of real ones.
        // Add a motor above this line and the count updates itself.
        count_
    };
}

// a scoped enum (enum class) is safer but does NOT implicitly convert, so it
// needs a cast at every use -- which is why unscoped wins for indexing
namespace Sensor {
    enum class Type {
        lidar,
        imu,
        gps,
        count_
    };

    // a helper to do the conversion once, instead of at every subscript
    constexpr auto index(Type t) {
        return static_cast<std::size_t>(t);
    }
}

std::string_view motorName(Motor::Type m) {
    switch (m) {
        case Motor::frontLeft:  return "front left";
        case Motor::frontRight: return "front right";
        case Motor::rearLeft:   return "rear left";
        case Motor::rearRight:  return "rear right";
        default:                return "???";
    }
}

int main() {

    // ---------------- named indices ------------------------------------------
    //
    // The vector's LENGTH also comes from the enum -- so the container and the
    // enumerators can never drift out of sync.
    std::vector<int> motorSpeeds(Motor::count_);   // note ( ), a LENGTH, not a
                                                   // one-element list (16.2)

    std::cout << "created a vector of length " << motorSpeeds.size()
              << " from Motor::count_\n\n";

    // now the indices read like English
    motorSpeeds[Motor::frontLeft]  = 120;
    motorSpeeds[Motor::frontRight] = 118;
    motorSpeeds[Motor::rearLeft]   = 95;
    motorSpeeds[Motor::rearRight]  = 97;

    std::cout << "by name:\n";
    std::cout << "  front left  = " << motorSpeeds[Motor::frontLeft]  << '\n';
    std::cout << "  rear right  = " << motorSpeeds[Motor::rearRight] << '\n';

    // compare against the version this replaces:
    //     motorSpeeds[2] = 95;      // which one was 2 again?

    // ---------------- looping over the enumerators ----------------------------
    //
    // count_ gives the loop its bound, so adding an enumerator automatically
    // extends the loop too.

    std::cout << "\nall motors:\n";
    for (int i { 0 }; i < Motor::count_; ++i)
        std::cout << "  " << motorName(static_cast<Motor::Type>(i))
                  << ": " << motorSpeeds[static_cast<std::size_t>(i)] << '\n';

    // Note that enumerators are NOT iterable directly -- there's no
    // `for (auto m : Motor::Type)`. Counting an int from 0 to count_ and
    // casting is the standard workaround.

    // ---------------- the scoped-enum version ---------------------------------
    //
    // enum class doesn't convert implicitly, so `sensors[Sensor::Type::imu]`
    // is a compile error. The index() helper above makes it usable:

    std::vector<double> sensors(Sensor::index(Sensor::Type::count_));

    sensors[Sensor::index(Sensor::Type::lidar)] = 4.2;
    sensors[Sensor::index(Sensor::Type::imu)]   = 0.03;
    sensors[Sensor::index(Sensor::Type::gps)]   = 12.7;

    std::cout << "\nscoped enum, via an index() helper:\n";
    std::cout << "  lidar = " << sensors[Sensor::index(Sensor::Type::lidar)] << '\n';
    std::cout << "  imu   = " << sensors[Sensor::index(Sensor::Type::imu)] << '\n';

    // C++23 adds std::to_underlying, which does exactly what index() does.

    // ---------------- why count_ has a trailing underscore --------------------
    //
    // It marks it as "not a real motor" -- it's a bookkeeping value. If you
    // ever switch over the enum, remember to exclude it, or the compiler will
    // warn about an unhandled case that should never actually occur.

    std::cout << "\nBEST PRACTICE:\n"
                 "  unscoped enum in a namespace, plus a count_ enumerator last.\n"
                 "  Use count_ for BOTH the vector's length and the loop's bound,\n"
                 "  and the two can never disagree.\n";

    return 0;
}

#include <iostream>
#include <string_view>
#include <optional>
#include <cstdint> // for std::uint8_t -- don't rely on another header pulling it in

// 13.1 - Introduction to program-defined (user-defined) types
// 13.2 - Unscoped enumerations
// 13.3 - Unscoped enumerator integral conversions
// 13.4 - Converting an enumeration to and from a string

// ------------------------ 13.1 PROGRAM-DEFINED TYPES -------------------------
//
// The fundamental types are fixed. When you need a type the language doesn't
// have, you DEFINE one: an enum, a struct, or a class.
//
// Two rules that matter immediately:
//   - a type definition ends with a SEMICOLON. Forgetting it produces a
//     cascade of nonsense errors on the following lines.
//   - types are EXEMPT from the one-definition rule ACROSS files, but must be
//     defined exactly once PER file. So type definitions go in a HEADER,
//     protected by a header guard, and get included wherever needed.

// ------------------------ 13.2 UNSCOPED ENUMERATIONS -------------------------
//
// An enum is a program-defined type whose values are limited to a set of
// named symbolic constants (ENUMERATORS).
// "Unscoped" = declared with plain 'enum'. Its enumerators leak into the
// SURROUNDING scope, which is the main problem with them.

enum SensorState {
    // these enumerators are in the GLOBAL scope, not inside SensorState
    state_offline,     // implicitly 0
    state_warming,     // implicitly 1
    state_ready,       // implicitly 2
    state_fault,       // implicitly 3
}; // <-- don't forget this semicolon

// naming convention: because the enumerators leak, prefixing them keeps the
// global namespace from filling up with bare words like "ready".

// you can assign explicit values; unspecified ones continue from the last
enum ErrorCode {
    err_none      = 0,
    err_timeout   = 10,
    err_overheat,      // 11 -- continues from the previous value
    err_disconnect = 20,
};

// AVOID: values you don't control can collide, and negative/duplicate values
// are legal but rarely what you want.

// enum Duplicate { state_ready }; // ERROR if uncommented: state_ready already
//                                 // exists in the global scope. This name
//                                 // collision is exactly why scoped enums
//                                 // (13.6) were added.

// you can specify the underlying integral type to control the size
enum Compact : std::uint8_t {
    compact_a,
    compact_b,
};

// ------------------------ 13.4 ENUM <-> STRING -------------------------------

// An enum prints as its INTEGER value by default, which is rarely useful.
// The standard fix is a conversion function. constexpr + string_view means
// this costs nothing at runtime.
constexpr std::string_view toString(SensorState state) {
    switch (state) {
        case state_offline: return "offline";
        case state_warming: return "warming";
        case state_ready:   return "ready";
        case state_fault:   return "fault";
        default:            return "???";
    }
}

// going the other way: a string might not match anything, so std::optional
// (12.15) is the honest return type
constexpr std::optional<SensorState> fromString(std::string_view text) {
    if (text == "offline") return state_offline;
    if (text == "warming") return state_warming;
    if (text == "ready")   return state_ready;
    if (text == "fault")   return state_fault;
    return std::nullopt;
}

int main() {

    // ---------------- 13.2 using an enum -------------------------------------
    SensorState lidar { state_warming };
    std::cout << "lidar state (raw): " << lidar << "\n";
    //                                    ^ prints 1, not "warming"

    lidar = state_ready;
    std::cout << "lidar state (raw): " << lidar << "\n";

    // notice the enumerators are used WITHOUT any qualification, because
    // they leaked into the global scope. SensorState::state_ready also works,
    // and is the better habit -- it reads like a scoped enum.
    SensorState imu { SensorState::state_fault };
    std::cout << "imu state (raw):   " << imu << "\n";

    // ---------------- 13.3 integral conversions ------------------------------
    //
    // Unscoped enumerators IMPLICITLY convert to integers. This is convenient
    // and also the source of most enum bugs.

    int asInt { lidar };  // implicit -- no cast needed, no warning
    std::cout << "\nlidar implicitly converted to int: " << asInt << "\n";

    // this compiles and is almost certainly a bug:
    int nonsense { state_ready + state_fault }; // 2 + 3 = 5, meaningless
    std::cout << "state_ready + state_fault = " << nonsense << " (meaningless!)\n";

    // the conversion does NOT go the other way implicitly
    // SensorState bad { 2 };  // ERROR if uncommented: int doesn't convert to enum
    SensorState fromInt { static_cast<SensorState>(2) }; // explicit is required
    std::cout << "static_cast<SensorState>(2) = " << fromInt << "\n";

    // WARNING: nothing checks that the integer is a VALID enumerator value.
    SensorState invalid { static_cast<SensorState>(77) }; // no error, no check
    std::cout << "static_cast<SensorState>(77) = " << invalid
              << " -- not a real state!\n";

    // ---------------- 13.4 printing properly ---------------------------------
    std::cout << "\nwith toString():\n";
    std::cout << "  lidar = " << toString(lidar) << "\n";
    std::cout << "  imu   = " << toString(imu) << "\n";
    std::cout << "  bogus = " << toString(invalid) << "\n";

    // because toString is constexpr, this is resolved entirely at compile time
    constexpr std::string_view compiled { toString(state_offline) };
    std::cout << "  compile-time: " << compiled << "\n";

    std::cout << "\nwith fromString():\n";
    for (std::string_view text : { "ready", "fault", "banana" }) {
        auto parsed { fromString(text) };
        std::cout << "  \"" << text << "\" -> "
                  << (parsed ? toString(*parsed) : "unrecognized") << "\n";
    }

    std::cout << "\nsizeof(Compact) = " << sizeof(Compact)
              << " byte (underlying type pinned to uint8_t)\n";

    return 0;
}

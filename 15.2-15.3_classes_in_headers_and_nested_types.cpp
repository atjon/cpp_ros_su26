#include "15.2_battery.h" // the class DEFINITION comes from here
#include <iostream>
#include <string_view>

// 15.2 - Classes and header files
// 15.3 - Nested types (member types)
//
// This target is built from TWO .cpp files plus a header:
//   15.2-15.3_classes_in_headers_and_nested_types.cpp  (this file, has main)
//   15.2_battery.cpp                                    (member definitions)
//   15.2_battery.h                                      (the class definition)
// See CMakeLists.txt.

// ------------------------ 15.3 NESTED TYPES ----------------------------------
//
// A class can contain TYPES as members, not just data and functions.
// A nested type is scoped to the class, so its name won't collide with
// anything outside, and the association is obvious at the point of use:
// Sensor::State reads better than SensorState.
//
// Nested types obey access specifiers just like everything else -- a private
// nested type is an implementation detail outsiders can't even name.

class Sensor {
public:
    // a nested ENUM. Referred to from outside as Sensor::State::ready.
    // This is a very common pattern -- the enum belongs to the class
    // conceptually, so it lives there literally.
    enum class State {
        offline,
        warming,
        ready,
        fault,
    };

    // a nested STRUCT
    struct Sample {
        double value { };
        int sequence { };
    };

    // a nested TYPE ALIAS (10.7) -- lets you change the underlying type in
    // one place without touching any caller that used Sensor::Id
    using Id = int;

private:
    Id m_id { };
    State m_state { State::offline };
    Sample m_last { };

    // a PRIVATE nested type -- outside code cannot even name Sensor::Internals
    struct Internals {
        int calibrationOffset { };
    };
    Internals m_internals { };

public:
    explicit Sensor(Id id) : m_id { id } { }

    Id id() const { return m_id; }
    State state() const { return m_state; }
    const Sample& lastSample() const { return m_last; }

    void record(double value, int sequence) {
        m_last = Sample { value, sequence };
        m_state = State::ready;
    }

    void fail() { m_state = State::fault; }
};

// defining a nested type's member function OUTSIDE the class needs the full
// nested qualification: Sensor::Sample::something()

constexpr std::string_view toString(Sensor::State state) {
    switch (state) {
        case Sensor::State::offline: return "offline";
        case Sensor::State::warming: return "warming";
        case Sensor::State::ready:   return "ready";
        case Sensor::State::fault:   return "fault";
        default:                     return "???";
    }
}

int main() {

    // ---------------- 15.2 the split class -----------------------------------
    //
    // main() only ever saw the HEADER. The bodies of wattHours(), drain(), and
    // print() live in a separate .cpp that was compiled independently and
    // linked in. This file has no idea how they're implemented -- which is the
    // entire point of the split.

    std::cout << "class split across header and source:\n";

    Battery main { 12.6, 8.0, "main_pack" };
    main.print();
    std::cout << "  wattHours() = " << main.wattHours() << "\n";

    main.drain(2.0);
    std::cout << "after drain(2.0):\n";
    main.print();

    main.drain(5.0);   // pushes it under 11V
    std::cout << "after drain(5.0):\n";
    main.print();      // now flagged [LOW] by the inline helper in the header

    Battery spare { };  // uses the = default constructor
    std::cout << "default-constructed spare:\n";
    spare.print();

    // ---------------- 15.3 nested types --------------------------------------
    std::cout << "\nnested types:\n";

    Sensor lidar { 1 };
    std::cout << "  sensor " << lidar.id() << " is "
              << toString(lidar.state()) << "\n";

    lidar.record(3.75, 100);
    std::cout << "  after record: " << toString(lidar.state())
              << ", value " << lidar.lastSample().value
              << " (seq " << lidar.lastSample().sequence << ")\n";

    // the nested type is named through the class
    Sensor::State s { Sensor::State::warming };
    std::cout << "  a standalone Sensor::State = " << toString(s) << "\n";

    // the nested struct can be used on its own too
    Sensor::Sample manual { 9.9, 42 };
    std::cout << "  a standalone Sensor::Sample = " << manual.value
              << " (seq " << manual.sequence << ")\n";

    // the nested alias behaves like any other type name
    Sensor::Id nextId { 2 };
    Sensor imu { nextId };
    std::cout << "  second sensor id = " << imu.id() << "\n";

    // Sensor::Internals leak { }; // ERROR if uncommented: it's private.
    //                             // Outside code can't name it at all.

    lidar.fail();
    std::cout << "  after fail(): " << toString(lidar.state()) << "\n";

    return 0;
}

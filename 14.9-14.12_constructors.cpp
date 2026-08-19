#include <iostream>
#include <string>

// 14.9  - Introduction to constructors
// 14.10 - Constructor member initializer lists
// 14.11 - Default constructors and default arguments
// 14.12 - Delegating constructors

// ------------------------ 14.9 WHY CONSTRUCTORS ------------------------------
//
// A class with private members is NOT an aggregate, so brace initialization
// can't reach in and set the members. Something has to initialize them, and
// that something is a CONSTRUCTOR.
//
// A constructor is a special member function that:
//   - has the SAME NAME as the class
//   - has NO return type (not even void)
//   - runs automatically when an object is created
//
// Its real job is to establish the class's INVARIANTS before anyone can
// touch the object.

class Motor {
private:
    int m_id { };
    double m_rpm { };
    std::string m_label { };

public:
    // ---------------- 14.10 MEMBER INITIALIZER LISTS -------------------------
    //
    // The part after the : and before the { } is the MEMBER INITIALIZER LIST.
    // Members are INITIALIZED there, before the constructor body ever runs.
    Motor(int id, double rpm, const std::string& label)
        : m_id { id }         // <-- these INITIALIZE
        , m_rpm { rpm }
        , m_label { label }
    {
        // the body runs AFTER all members are already initialized.
        // Assigning here would be a second, wasted operation:
        //     m_id = id;   // initialize-then-assign, not initialize
        //
        // BEST PRACTICE: initialize in the list, use the body only for
        // validation and side effects.
        if (m_rpm < 0.0)
            m_rpm = 0.0;     // this IS a legitimate body use -- enforcing an invariant
    }

    // CRITICAL GOTCHA: members are initialized in the order they are DECLARED
    // in the class, NOT the order you write them in the initializer list.
    // Writing them out of order gets you a compiler warning, and if one member
    // is initialized from another, you can silently read an uninitialized value.

    int id() const { return m_id; }
    double rpm() const { return m_rpm; }
    const std::string& label() const { return m_label; }

    void print() const {
        std::cout << "  motor " << m_id << " (" << m_label
                  << ") at " << m_rpm << " rpm\n";
    }
};

// ------------------------ 14.11 DEFAULT CONSTRUCTORS -------------------------
//
// A DEFAULT CONSTRUCTOR takes no parameters. It's what runs for `Thing t;`
// or `Thing t{};`.
//
// If you declare NO constructors at all, the compiler generates an implicit
// default constructor for you. The moment you declare ANY constructor, that
// free gift disappears -- which is why `Motor m;` wouldn't compile above.

class Gripper {
private:
    double m_openMm { 0.0 };
    bool m_powered { false };

public:
    Gripper() {  // explicit default constructor
        std::cout << "  [Gripper default ctor]\n";
    }

    Gripper(double openMm) : m_openMm { openMm } {
        std::cout << "  [Gripper(double) ctor]\n";
    }

    double openMm() const { return m_openMm; }
    bool powered() const { return m_powered; }
};

// Rather than writing two constructors, DEFAULT ARGUMENTS (11.5) often
// collapse them into one. A constructor whose parameters ALL have defaults
// counts as a default constructor.
class Servo {
private:
    int m_channel { };
    double m_angle { };

public:
    Servo(int channel = 0, double angle = 90.0)
        : m_channel { channel }, m_angle { angle } { }

    void print() const {
        std::cout << "  servo ch" << m_channel << " at " << m_angle << " deg\n";
    }
};

// If you want the compiler's default constructor back AFTER declaring another
// constructor, ask for it explicitly with = default. Prefer this over writing
// an empty `Thing() {}` -- = default keeps the type trivially constructible,
// which matters for some optimizations and type traits.
class Encoder {
private:
    int m_ticks { 0 };

public:
    Encoder() = default;                       // the compiler's version, restored
    explicit Encoder(int ticks) : m_ticks { ticks } { }

    int ticks() const { return m_ticks; }
};

// ------------------------ 14.12 DELEGATING CONSTRUCTORS ----------------------
//
// When two constructors share setup, DON'T copy the logic, and DON'T call one
// constructor from inside another's body (that creates a temporary and throws
// it away -- a classic bug). Instead, DELEGATE from the initializer list.

class Waypoint {
private:
    double m_x { };
    double m_y { };
    std::string m_name { };

public:
    // the "full" constructor does the real work
    Waypoint(double x, double y, const std::string& name)
        : m_x { x }, m_y { y }, m_name { name }
    {
        std::cout << "  [full ctor ran]\n";
    }

    // this one DELEGATES to the full one. Note the delegation target is the
    // ONLY thing in the initializer list -- you can't delegate AND initialize
    // members in the same list.
    Waypoint(double x, double y)
        : Waypoint { x, y, "unnamed" }   // calls the constructor above
    {
        std::cout << "  [2-arg ctor body ran after delegating]\n";
    }

    Waypoint() : Waypoint { 0.0, 0.0 } { } // delegates to the 2-arg one,
                                           // which delegates again. Chains are fine.

    void print() const {
        std::cout << "  " << m_name << " at (" << m_x << ", " << m_y << ")\n";
    }
};
// WARNING: a constructor that delegates to itself creates infinite recursion.
// The compiler will not catch it; the program will stack overflow.

int main() {

    // ---------------- 14.9 / 14.10 -------------------------------------------
    std::cout << "constructors:\n";
    Motor left { 1, 3000.0, "drive_left" };
    left.print();

    Motor clamped { 2, -500.0, "drive_right" }; // body enforces the invariant
    clamped.print();

    // Motor bad;  // ERROR if uncommented: no default constructor exists,
    //             // because declaring Motor(int,double,string) suppressed the
    //             // implicit one

    // ---------------- 14.11 default constructors -----------------------------
    std::cout << "\ndefault constructors:\n";
    Gripper g1;      // default ctor
    Gripper g2 { };  // ALSO the default ctor -- prefer this brace form
    Gripper g3 { 25.0 };

    std::cout << "  g1 open " << g1.openMm() << ", g3 open " << g3.openMm() << "\n";

    // WATCH OUT for the "most vexing parse":
    //   Gripper g4();   // this declares a FUNCTION named g4 returning a Gripper,
    //                   // NOT an object. Using { } instead of ( ) avoids it
    //                   // entirely, which is a good reason to prefer braces.

    std::cout << "\ndefault arguments instead of two constructors:\n";
    Servo a { };            // both defaulted
    Servo b { 3 };          // angle defaulted
    Servo c { 3, 45.0 };    // neither defaulted
    a.print();
    b.print();
    c.print();

    Encoder e1 { };         // = default version
    Encoder e2 { 500 };
    std::cout << "\nencoders: " << e1.ticks() << " and " << e2.ticks() << "\n";

    // ---------------- 14.12 delegation ---------------------------------------
    std::cout << "\ndelegating constructors:\n";

    std::cout << "Waypoint w1 { 3.0, 4.0, \"dock\" }:\n";
    Waypoint w1 { 3.0, 4.0, "dock" };
    w1.print();

    std::cout << "Waypoint w2 { 1.0, 2.0 }:\n";
    Waypoint w2 { 1.0, 2.0 };   // watch the full ctor run FIRST, then this body
    w2.print();

    std::cout << "Waypoint w3 { }:\n";
    Waypoint w3 { };            // chains through both
    w3.print();

    return 0;
}

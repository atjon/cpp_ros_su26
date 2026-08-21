#include <iostream>
#include <string>

// 15.6 - Static member variables
// 15.7 - Static member functions

// ------------------------ 15.6 STATIC MEMBER VARIABLES -----------------------
//
// A normal data member exists once PER OBJECT.
// A STATIC data member exists ONCE FOR THE WHOLE CLASS -- every object shares
// the same one, and it exists even if no objects are ever created.
//
// Think of it as a global variable that lives inside the class's scope and
// obeys the class's access specifiers.

class Motor {
private:
    int m_id { };
    double m_rpm { };

    // shared by every Motor that ever exists
    static int s_count;          // DECLARED here...
    static double s_maxRpmSeen;

public:
    // A static member that is constexpr (or const integral) CAN be initialized
    // right here in the class -- no separate definition needed.
    static constexpr double s_absoluteMaxRpm { 10000.0 };

    // 'inline static' (C++17) also allows in-class initialization for
    // non-const statics. This is now the easiest way to do it, and avoids the
    // out-of-class definition entirely.
    inline static int s_faultCount { 0 };

    explicit Motor(int id) : m_id { id } {
        ++s_count;   // every constructor bumps the SHARED counter
    }

    ~Motor() { --s_count; }

    void setRpm(double rpm) {
        m_rpm = rpm;
        if (rpm > s_maxRpmSeen)
            s_maxRpmSeen = rpm;   // writing to shared state
    }

    void fault() { ++s_faultCount; }

    int id() const { return m_id; }
    double rpm() const { return m_rpm; }

    // ---------------- 15.7 STATIC MEMBER FUNCTIONS ---------------------------
    //
    // A static member function belongs to the CLASS, not to any object.
    // Consequences:
    //   - it can be called WITHOUT an object: Motor::count()
    //   - it has NO 'this' pointer (15.1)
    //   - therefore it can ONLY touch static members, never m_id or m_rpm
    //   - it CANNOT be const, because const constrains 'this', which doesn't exist

    static int count() { return s_count; }
    static double maxRpmSeen() { return s_maxRpmSeen; }

    static void resetStats() {
        s_maxRpmSeen = 0.0;
        s_faultCount = 0;
        // m_rpm = 0.0;  // ERROR if uncommented: no object, no m_rpm to touch
    }

    // static int broken() const { return s_count; } // ERROR: static + const
};

// ...DEFINED here, outside the class. This is the part people forget, and the
// error is a link-time "undefined reference to Motor::s_count" rather than a
// compile error, which makes it confusing.
//
// Note: NO 'static' keyword on the definition, and the private access
// specifier does NOT block you from defining it out here.
int Motor::s_count { 0 };
double Motor::s_maxRpmSeen { 0.0 };

// ------------------------ A COMMON REAL USE ----------------------------------
//
// Static members give you class-scoped constants and shared registries without
// polluting the global namespace.

class Config {
public:
    static constexpr int maxRetries { 3 };
    static constexpr double timeoutSeconds { 1.5 };
    inline static std::string activeProfile { "default" };

    // a static function used as a named alternative constructor -- a very
    // common pattern, since it can have a descriptive name unlike a constructor
    static Config makeDefault() { return Config { }; }
};

int main() {

    // ---------------- statics exist before any object ------------------------
    std::cout << "before any Motor exists:\n";
    std::cout << "  Motor::count() = " << Motor::count() << "\n";
    //             ^ called through the CLASS, not an object

    std::cout << "  Motor::s_absoluteMaxRpm = " << Motor::s_absoluteMaxRpm << "\n";

    // ---------------- 15.6 shared state --------------------------------------
    {
        Motor left { 1 };
        std::cout << "\nafter creating 1 motor: count = " << Motor::count() << "\n";

        Motor right { 2 };
        Motor rear { 3 };
        std::cout << "after creating 3 motors: count = " << Motor::count() << "\n";

        left.setRpm(3000);
        right.setRpm(7500);
        rear.setRpm(1200);

        // every motor wrote to the SAME s_maxRpmSeen
        std::cout << "max rpm seen across all motors: " << Motor::maxRpmSeen() << "\n";

        // a static can also be reached through an object, though it reads
        // misleadingly -- it looks per-object but isn't. Prefer Motor::count().
        std::cout << "left.count() = " << left.count()
                  << " (same shared value, confusing syntax)\n";

        left.fault();
        right.fault();
        std::cout << "fault count = " << Motor::s_faultCount << "\n";

    } // all three destroyed here, each decrementing the counter

    std::cout << "\nafter scope ends: count = " << Motor::count() << "\n";
    std::cout << "but max rpm seen persists: " << Motor::maxRpmSeen() << "\n";
    //             ^ static state OUTLIVES the objects -- that's the whole point,
    //               and also the main hazard. Shared mutable state is exactly as
    //               dangerous here as a global variable.

    Motor::resetStats();
    std::cout << "after resetStats(): max rpm = " << Motor::maxRpmSeen()
              << ", faults = " << Motor::s_faultCount << "\n";

    // ---------------- class-scoped constants ---------------------------------
    std::cout << "\nclass-scoped constants:\n";
    std::cout << "  Config::maxRetries      = " << Config::maxRetries << "\n";
    std::cout << "  Config::timeoutSeconds  = " << Config::timeoutSeconds << "\n";
    std::cout << "  Config::activeProfile   = " << Config::activeProfile << "\n";

    Config::activeProfile = "outdoor"; // shared by everything in the program
    std::cout << "  changed to              = " << Config::activeProfile << "\n";

    // because maxRetries is constexpr, it works in constant expressions
    constexpr int retries { Config::maxRetries };
    int slots[retries] { };   // legal: a compile-time constant array size
    std::cout << "  array sized by the constant: " << sizeof(slots) / sizeof(int)
              << " slots\n";

    Config c { Config::makeDefault() }; // the static factory function
    (void)c;

    return 0;
}

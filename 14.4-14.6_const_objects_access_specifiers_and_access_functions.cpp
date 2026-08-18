#include <iostream>
#include <string>

// 14.4 - Const class objects and const member functions
// 14.5 - Public and private members and access specifiers
// 14.6 - Access functions

class SensorReading {

// ------------------------ 14.5 ACCESS SPECIFIERS -----------------------------
//
// Three of them:
//   public:    accessible by anyone
//   private:   accessible only by other members of this class (the default
//              for a class), and by friends (15.8)
//   protected: like private, but also accessible by DERIVED classes
//              (matters once inheritance shows up, chapter 24)
//
// You can use each specifier as many times as you like, in any order.
// Common style: public interface first (that's what readers care about),
// private data last.

private:
    double m_celsius { };
    std::string m_label { "unnamed" };
    bool m_valid { false };

public:
    // ---------------- 14.4 CONST MEMBER FUNCTIONS ----------------------------
    //
    // The 'const' AFTER the parameter list promises this function will not
    // modify the object. That promise is what makes it callable on a const
    // object.
    //
    // RULE: if a member function does not modify the object, mark it const.
    // Skipping this is the #1 reason people hit "passing const X as this
    // argument discards qualifiers" errors later.

    double celsius() const { return m_celsius; }        // const: read-only
    double fahrenheit() const {                          // const: computes, no writes
        return m_celsius * 9.0 / 5.0 + 32.0;
    }
    const std::string& label() const { return m_label; } // const: returns a read-only view
    bool valid() const { return m_valid; }

    // NOT const -- these modify the object, so they can't be called on a const one
    void setCelsius(double c) {
        m_celsius = c;
        m_valid = true;
    }
    void setLabel(const std::string& label) { m_label = label; }
    void invalidate() { m_valid = false; }

    void print() const {
        std::cout << "  " << m_label << ": " << m_celsius << "C / "
                  << fahrenheit() << "F"
                  << (m_valid ? "" : "  (INVALID)") << "\n";
        // a const member function may only call OTHER const member functions.
        // calling setCelsius() here would be a compile error.
    }
};

// ------------------------ 14.6 ACCESS FUNCTIONS ------------------------------
//
// An access function is a short public member function whose job is to expose
// a private member.
//   GETTER -- returns the value
//   SETTER -- changes the value
//
// Naming conventions vary; learncpp suggests either celsius()/setCelsius() or
// getCelsius()/setCelsius(). Pick one and be consistent.
//
// IMPORTANT: getters and setters for EVERY member is an anti-pattern -- that's
// a public struct wearing a disguise, with more typing and no added safety.
// Provide access only where callers genuinely need it.

class Tank {
private:
    double m_liters { };
    double m_capacity { 10.0 };

public:
    Tank() = default; // needed because we declared another constructor below

    explicit Tank(double capacity) : m_capacity { capacity } { }

    double liters() const { return m_liters; }
    double capacity() const { return m_capacity; }

    // THIS is the payoff of a setter over a public member: the class can
    // enforce an INVARIANT ("level is never negative, never over capacity")
    // that a raw public member could never guarantee.
    void setLiters(double liters) {
        if (liters < 0.0)         m_liters = 0.0;
        else if (liters > m_capacity) m_liters = m_capacity;
        else                      m_liters = liters;
    }

    double percentFull() const { return m_liters / m_capacity * 100.0; }
};

int main() {

    // ---------------- 14.5 / 14.6 access -------------------------------------
    SensorReading temp;
    temp.setLabel("coolant");
    temp.setCelsius(93.5);
    temp.print();

    // temp.m_celsius = 200.0; // ERROR if uncommented: private
    std::cout << "  via getter: " << temp.celsius() << "C\n";

    // ---------------- 14.4 const objects -------------------------------------
    const SensorReading frozen { }; // a const object: nothing about it can change

    frozen.print();                 // OK -- print() is const
    std::cout << "  frozen.celsius() = " << frozen.celsius() << "\n";

    // frozen.setCelsius(10.0); // ERROR if uncommented: setCelsius isn't const,
    //                          // so it can't be called on a const object

    // this matters constantly in practice, because passing by const& (12.6)
    // makes the parameter a CONST object inside the function:
    auto describe = [](const SensorReading& r) {
        r.print();        // only works because print() is const
        // r.invalidate(); // would be an ERROR -- not a const member function
    };
    std::cout << "\npassed as const&:\n";
    describe(temp);

    // ---------------- 14.6 invariants ----------------------------------------
    std::cout << "\nsetter enforcing an invariant:\n";
    Tank fuel { 20.0 };

    fuel.setLiters(15.0);
    std::cout << "  set 15 -> " << fuel.liters()
              << "L (" << fuel.percentFull() << "%)\n";

    fuel.setLiters(-5.0);            // clamped to 0
    std::cout << "  set -5 -> " << fuel.liters() << "L (clamped)\n";

    fuel.setLiters(1000.0);          // clamped to capacity
    std::cout << "  set 1000 -> " << fuel.liters() << "L (clamped to capacity)\n";

    // If m_liters were public, every one of those bad values would have stuck,
    // and the bug would surface somewhere far away from the line that caused it.

    return 0;
}

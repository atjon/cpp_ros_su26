#include <iostream>
#include <string>

// 14.7 - Member functions returning references to data members
// 14.8 - The benefits of data hiding (encapsulation)

// ------------------------ 14.7 RETURNING REFERENCES TO MEMBERS ---------------
//
// A getter returning by VALUE copies. For a std::string that's a real cost on
// every call. Returning a CONST REFERENCE avoids the copy.
//
// This is safe in a way that returning a reference to a local (12.12) is not:
// the member lives as long as the object does, so the reference stays valid
// for at least as long as the object.

class Robot {
private:
    std::string m_name { "unnamed" };
    int m_battery { 100 };

public:
    Robot() = default;
    explicit Robot(const std::string& name) : m_name { name } { }

    // returns a COPY -- correct but wasteful for a string
    std::string nameByValue() const { return m_name; }

    // returns a CONST REFERENCE -- no copy.
    // The const matters twice over: the function is const (14.4), AND the
    // returned reference is const so callers can't write through it.
    const std::string& name() const { return m_name; }

    // returning a NON-const reference hands out write access to a private
    // member, which throws away the encapsulation you set up. Almost always
    // the wrong call:
    // std::string& nameWritable() { return m_name; }

    int battery() const { return m_battery; }  // int is cheap -- return by VALUE.
                                               // Returning const int& here would
                                               // be pointless indirection.
    void setBattery(int pct) {
        m_battery = (pct < 0) ? 0 : (pct > 100 ? 100 : pct);
    }
};

// THE DANGLING TRAP: if the object is a TEMPORARY, the reference it returned
// dies with it at the end of the full expression.
Robot makeRobot() {
    return Robot { "temporary" };
}

// ------------------------ 14.8 DATA HIDING -----------------------------------
//
// Encapsulation = keep data private, expose a public INTERFACE.
// The four arguments for it, concretely:
//
//   1. INVARIANTS  -- the class can guarantee its data is always valid,
//                     because it controls every path that writes to it.
//   2. DEBUGGING   -- if a private member holds a bad value, the culprit is
//                     inside this class. With a public member it could be
//                     anywhere in the program.
//   3. CHANGEABLE  -- you can rewrite the internals without touching callers,
//                     as long as the interface stays the same.
//   4. SIMPLER API -- callers learn a handful of functions instead of a pile
//                     of members and the rules for combining them.

// Demonstrating #3: this class stores degrees internally...
class Heading {
private:
    double m_degrees { };

public:
    explicit Heading(double degrees) : m_degrees { normalize(degrees) } { }

    double degrees() const { return m_degrees; }
    double radians() const { return m_degrees * 3.14159265358979 / 180.0; }

    void setDegrees(double d) { m_degrees = normalize(d); } // invariant: 0-360

private:
    // a PRIVATE helper -- part of the implementation, not the interface.
    // Callers never see it, so it can change or disappear freely.
    static double normalize(double d) {
        while (d < 0.0)      d += 360.0;
        while (d >= 360.0)   d -= 360.0;
        return d;
    }
};
// ...if you later switched m_degrees to m_radians internally, every caller
// using degrees()/radians()/setDegrees() would keep working untouched.

int main() {

    // ---------------- 14.7 reference getters ---------------------------------
    Robot bot { "scout" };

    std::cout << "by value:     " << bot.nameByValue() << " (a copy was made)\n";
    std::cout << "by const ref: " << bot.name() << " (no copy)\n";

    // binding the result to a reference keeps it copy-free
    const std::string& alias { bot.name() };
    std::cout << "alias:        " << alias << "\n";

    // binding it to a std::string COPIES -- that's usually fine, just know it
    std::string copy { bot.name() };
    std::cout << "copy:         " << copy << "\n";

    // ---------------- the dangling trap --------------------------------------
    //
    // const std::string& bad { makeRobot().name() };
    // std::cout << bad;   // UNDEFINED BEHAVIOR: the temporary Robot is
    //                     // destroyed at the end of that first line, so 'bad'
    //                     // refers to a dead object's member.
    //                     // Lifetime extension does NOT apply here -- it only
    //                     // extends the temporary itself, not a reference to
    //                     // something INSIDE it.
    //
    // SAFE version: copy it out immediately, before the temporary dies.
    std::string safe { makeRobot().name() };
    std::cout << "\nsafely copied from a temporary: " << safe << "\n";

    // RULE OF THUMB: use the return value of a reference-returning getter
    // immediately, or copy it. Don't store the reference unless you're certain
    // the object outlives it.

    // ---------------- 14.8 encapsulation in action ---------------------------
    std::cout << "\nheading invariant (always 0-360):\n";

    Heading h { 45.0 };
    std::cout << "  45    -> " << h.degrees() << "\n";

    h.setDegrees(370.0);
    std::cout << "  370   -> " << h.degrees() << " (wrapped)\n";

    h.setDegrees(-90.0);
    std::cout << "  -90   -> " << h.degrees() << " (wrapped)\n";

    std::cout << "  as radians: " << h.radians() << "\n";

    // there is NO WAY for outside code to put an out-of-range value in there.
    // That's the guarantee a public member could never give you.

    return 0;
}

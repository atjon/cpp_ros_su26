#include <iostream>

// 14.1 - Introduction to object-oriented programming
// 14.2 - Introduction to classes
// 14.3 - Member functions

// ------------------------ 14.1 OOP IN ONE PARAGRAPH --------------------------
//
// Procedural programming: data over here, functions that act on it over there.
// Object-oriented programming: bundle the data WITH the behavior that operates
// on it, so an object knows how to manage itself.
//
// The practical payoff isn't philosophy -- it's that you can change a type's
// internals without every caller breaking, because callers go through the
// type's interface instead of poking at its members.

// ------------------------ 14.3 MEMBER FUNCTIONS ------------------------------
//
// A struct can have FUNCTIONS as members, not just data. They're called on an
// object with the same . and -> syntax as data members.

struct Battery {
    // data members
    double volts { 12.0 };
    double capacityAh { 5.0 };

    // MEMBER FUNCTION -- note it uses volts/capacityAh with no qualification.
    // Inside a member function, unqualified member names implicitly refer to
    // the object you were called on. (The mechanism is the hidden 'this'
    // pointer -- that's 15.1.)
    double wattHours() const {
        return volts * capacityAh;
    }

    void print() const {
        std::cout << "  " << volts << "V " << capacityAh << "Ah -> "
                  << wattHours() << "Wh\n";
        //           ^ one member function calling another. No qualification,
        //             and DECLARATION ORDER DOESN'T MATTER inside a class --
        //             wattHours() could have been defined below this.
    }

    void drain(double amount) {
        volts -= amount;   // member functions can MODIFY the object
    }
};

// ------------------------ 14.2 CLASSES ---------------------------------------
//
// 'class' and 'struct' are the SAME feature with ONE difference:
//   struct -> members are PUBLIC by default
//   class  -> members are PRIVATE by default
//
// That's genuinely the whole language-level difference. The convention that
// grew around it:
//   use struct for simple passive data bundles with no invariants
//   use class  when the type has invariants to protect (that's 14.5)

class Motor {
    // no access specifier yet, so these are PRIVATE -- inaccessible outside
    int m_id { };
    double m_rpm { };

public: // everything after this is accessible from outside
    void configure(int id, double rpm) {
        m_id = id;
        m_rpm = rpm;
    }

    void print() const {
        std::cout << "  motor " << m_id << " at " << m_rpm << " rpm\n";
    }
};

// naming convention: the m_ prefix marks a data MEMBER, so inside a long
// member function you can tell members from locals and parameters at a glance.

int main() {

    // ---------------- 14.3 calling member functions --------------------------
    Battery main { 12.6, 8.0 };  // still aggregate init -- member FUNCTIONS
                                 // don't stop a struct from being an aggregate
                                 // (constructors do -- that's 14.9)

    std::cout << "battery:\n";
    main.print();
    std::cout << "  wattHours() = " << main.wattHours() << "\n";

    main.drain(1.2);
    std::cout << "after drain(1.2):\n";
    main.print();

    // each OBJECT has its own copy of the data members, but they all SHARE one
    // copy of each member function. The function is told which object to work
    // on at the call site.
    Battery spare { 11.0, 2.5 };
    std::cout << "\ntwo independent objects:\n";
    main.print();
    spare.print();

    // through a pointer, use -> exactly like with data members
    Battery* ptr { &spare };
    std::cout << "\nvia pointer: ";
    ptr->print();

    // ---------------- 14.2 class access --------------------------------------
    std::cout << "\nclass with private members:\n";
    Motor left;              // safe: the members have default initializers
    left.configure(1, 3000); // must go through the public interface
    left.print();

    // left.m_rpm = 9999;    // ERROR if uncommented: m_rpm is private.
    //                       // THIS IS THE POINT -- the class controls how its
    //                       // data changes, so it can enforce rules later
    //                       // without hunting down every caller.

    // Motor bad { 1, 3000 }; // ERROR if uncommented: a class with private
    //                        // members is NOT an aggregate, so brace-init
    //                        // doesn't work. It needs a constructor (14.9).

    return 0;
}

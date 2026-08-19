#include <iostream>
#include <string>

// 15.1 - The hidden "this" pointer and member function chaining

// ------------------------ THE HIDDEN THIS POINTER ----------------------------
//
// In 14.3 we used member names inside member functions without saying WHICH
// object they belonged to. Here's the mechanism.
//
// Every non-static member function has a hidden parameter: 'this', a pointer
// to the object it was called on. When you write:
//
//     motor.setRpm(3000);
//
// the compiler effectively turns it into:
//
//     Motor::setRpm(&motor, 3000);
//
// and inside the function, an unqualified m_rpm is really this->m_rpm.
//
// 'this' is a CONST POINTER (Motor* const), so it can't be reseated.
// In a CONST member function it's a pointer to const (const Motor* const),
// which is exactly why a const member function can't write to members.

class Motor {
private:
    int m_id { };
    double m_rpm { };
    std::string m_label { "unnamed" };

public:
    explicit Motor(int id) : m_id { id } { }

    void showThis() const {
        std::cout << "  this = " << this << "\n";
        std::cout << "  this->m_id = " << this->m_id
                  << " (same as plain m_id = " << m_id << ")\n";
    }

    // WHERE 'this' IS ACTUALLY NECESSARY:
    // when a parameter SHADOWS a member name, this-> disambiguates them.
    // (The m_ prefix convention exists partly to avoid needing this.)
    void setLabel(const std::string& m_label) {
        this->m_label = m_label; // this->m_label is the MEMBER
                                 //      m_label  is the PARAMETER
    }

    // ---------------- MEMBER FUNCTION CHAINING -------------------------------
    //
    // Return *this (the OBJECT, by reference) and calls can be strung together.
    // Each call hands the same object back, so the next call operates on it.
    // This is the same trick that makes std::cout << a << b work (13.5).
    //
    // Return type is Motor& -- a reference. Returning Motor by value would
    // copy the object and each call would modify a different temporary.

    Motor& setRpm(double rpm) {
        m_rpm = rpm;
        return *this;   // dereference the pointer to get the OBJECT
    }

    Motor& setId(int id) {
        m_id = id;
        return *this;
    }

    Motor& scaleRpm(double factor) {
        m_rpm *= factor;
        return *this;
    }

    // a const member function can chain too, returning const Motor&
    const Motor& print() const {
        std::cout << "  motor " << m_id << " (" << m_label
                  << ") at " << m_rpm << " rpm\n";
        return *this;
    }
};

int main() {

    Motor left { 1 };
    Motor right { 2 };

    // ---------------- proving 'this' is per-object ---------------------------
    std::cout << "left:\n";
    left.showThis();
    std::cout << "&left = " << &left << "  <- the same address\n";

    std::cout << "\nright:\n";
    right.showThis();
    std::cout << "&right = " << &right << "  <- a different object, different this\n";

    // both calls run the SAME function. The only thing that differs is the
    // 'this' pointer passed in behind your back.

    // ---------------- shadowing ----------------------------------------------
    left.setLabel("drive_left");

    // ---------------- chaining -----------------------------------------------
    std::cout << "\nwithout chaining:\n";
    left.setRpm(3000);
    left.scaleRpm(1.5);
    left.print();

    std::cout << "\nwith chaining (same operations, one expression):\n";
    right.setLabel("drive_right");
    right.setId(7).setRpm(2000).scaleRpm(2.0).print();
    //     ^ setId returns Motor&, so .setRpm() is called on that reference,
    //       which returns Motor& again, and so on down the line.

    // print() returns const Motor&, so anything chained AFTER it must also be
    // a const member function:
    // right.print().setRpm(10);  // ERROR if uncommented: setRpm isn't const
    right.print().print();        // fine -- print() is const

    // WHEN TO USE CHAINING: it reads well for setters and for builder-style
    // configuration. Don't force it -- a function that returns *this purely so
    // it can be chained, when nobody chains it, is just noise.

    return 0;
}

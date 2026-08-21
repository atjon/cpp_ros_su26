#include <iostream>
#include <string>
#include <utility> // for std::move

// 15.8  - Friend non-member functions
// 15.9  - Friend classes and friend member functions
// 15.10 - Ref qualifiers

// ------------------------ 15.8 FRIEND NON-MEMBER FUNCTIONS -------------------
//
// A FRIEND is a function or class that a class explicitly grants access to its
// private members.
//
// Key points:
//   - friendship is GRANTED BY the class, not taken. Outside code can't
//     declare itself a friend, so encapsulation isn't actually broken --
//     the class still decides who gets in.
//   - a friend is NOT a member. It has no 'this' pointer, isn't called with
//     the . syntax, and access specifiers don't apply to the declaration's
//     placement (a friend declared in a private section is still a friend).
//   - friendship is NOT reciprocal and NOT inherited.

class Temperature {
private:
    double m_celsius { };

public:
    explicit Temperature(double celsius) : m_celsius { celsius } { }

    double celsius() const { return m_celsius; }

    // A friend function DEFINED inside the class. It's still a NON-MEMBER --
    // it just gets defined here for convenience, and is implicitly inline.
    // This is the standard way to write operator<< (13.5) for a class:
    // the left operand is the stream, not the object, so it CAN'T be a member.
    friend std::ostream& operator<<(std::ostream& out, const Temperature& t) {
        return out << t.m_celsius << "C"; // reaches into the private member
    }

    // friend declaration only -- the definition is below
    friend double difference(const Temperature& a, const Temperature& b);

    // friending a whole class (15.9)
    friend class Thermostat;
};

// the definition of the friend. Note: NO 'friend' keyword out here, and no
// Temperature:: qualification, because it is not a member.
double difference(const Temperature& a, const Temperature& b) {
    return a.m_celsius - b.m_celsius; // both privates are accessible
}

// ------------------------ 15.9 FRIEND CLASSES --------------------------------
//
// A friend class can access ALL private members of the class that befriended it.
// This is a blunt instrument -- prefer friending a single member function when
// you can, so you're not handing over the whole class.

class Thermostat {
private:
    double m_targetC { 20.0 };

public:
    explicit Thermostat(double targetC) : m_targetC { targetC } { }

    bool shouldHeat(const Temperature& current) const {
        return current.m_celsius < m_targetC;  // private access, granted above
    }

    void report(const Temperature& current) const {
        std::cout << "  target " << m_targetC << "C, current "
                  << current.m_celsius << "C -> "
                  << (shouldHeat(current) ? "HEAT" : "idle") << "\n";
    }
};

// ------------------------ 15.10 REF QUALIFIERS -------------------------------
//
// THE PROBLEM: a getter that returns a const reference to a member (14.7) is
// efficient for a normal object but DANGEROUS on a temporary -- the temporary
// dies at the end of the expression, leaving a dangling reference.
//
// REF QUALIFIERS let you overload a member function on the VALUE CATEGORY
// (12.2) of the object it's called on:
//     & -- this overload is chosen when the object is an LVALUE
//    && -- chosen when the object is an RVALUE (a temporary)
//
// So you can return a cheap reference for lvalues and a safe copy for rvalues.

class Label {
private:
    std::string m_text { };

public:
    explicit Label(const std::string& text) : m_text { text } { }

    // called on an LVALUE -- the object outlives the call, so a reference is safe
    const std::string& text() const & {
        std::cout << "    [lvalue overload: returning a reference]\n";
        return m_text;
    }

    // called on an RVALUE -- the object is about to die, so return a COPY
    std::string text() const && {
        std::cout << "    [rvalue overload: returning a copy]\n";
        return m_text;
    }

    // NOTE: once you ref-qualify ONE overload of a function, you must
    // ref-qualify ALL of them. Mixing qualified and unqualified is an error.
};

Label makeLabel() {
    return Label { "temporary_label" };
}

int main() {

    // ---------------- 15.8 friend functions ----------------------------------
    std::cout << "friend functions:\n";

    Temperature coolant { 93.5 };
    Temperature ambient { 22.0 };

    // works because operator<< is a friend and can read m_celsius
    std::cout << "  coolant = " << coolant << "\n";
    std::cout << "  ambient = " << ambient << "\n";

    std::cout << "  difference = " << difference(coolant, ambient) << " degrees\n";
    //             ^ called like a normal free function, NOT coolant.difference()

    // ---------------- 15.9 friend classes ------------------------------------
    std::cout << "\nfriend class:\n";

    Thermostat stat { 21.0 };
    stat.report(ambient);
    stat.report(Temperature { 18.0 });

    // note the friendship is ONE-WAY: Thermostat can see into Temperature,
    // but Temperature cannot see Thermostat's private m_targetC.

    // ---------------- 15.10 ref qualifiers -----------------------------------
    std::cout << "\nref qualifiers:\n";

    Label named { "persistent_label" };

    std::cout << "  called on an lvalue:\n";
    const std::string& safe { named.text() };  // lvalue overload -> reference
    std::cout << "    got: " << safe << "\n";

    std::cout << "  called on an rvalue:\n";
    std::string fromTemp { makeLabel().text() }; // rvalue overload -> copy
    std::cout << "    got: " << fromTemp << "\n";

    // WITHOUT the ref qualifiers, this line:
    //     const std::string& bad { makeLabel().text() };
    // would bind a reference to a member of a temporary that dies immediately
    // -- exactly the dangling bug from 14.7. The && overload returns a copy
    // instead, so the same code is safe.

    // std::move turns an lvalue into an rvalue, so it picks the && overload
    // even on a named object (that mechanism is 22.4):
    std::cout << "  lvalue forced to rvalue with std::move:\n";
    std::string moved { std::move(named).text() };
    std::cout << "    got: " << moved << "\n";

    return 0;
}

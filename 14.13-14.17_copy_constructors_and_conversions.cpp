#include <iostream>
#include <string_view>

// 14.13 - Temporary class objects
// 14.14 - Introduction to the copy constructor
// 14.15 - Class initialization and copy elision
// 14.16 - Converting constructors and the explicit keyword
// 14.17 - Constexpr aggregates and classes

class Volts {
private:
    double m_value { };

public:
    // ---------------- 14.14 THE COPY CONSTRUCTOR -----------------------------
    //
    // A copy constructor creates a new object as a COPY of an existing one.
    // Its signature is always: ClassName(const ClassName& other)
    //
    // Taking the parameter BY CONST REFERENCE is mandatory -- by value would
    // need a copy to make the copy, recursing forever. The compiler rejects it.

    Volts(double value) : m_value { value } {
        std::cout << "    [converting ctor: " << m_value << "]\n";
    }

    Volts(const Volts& other) : m_value { other.m_value } {
        std::cout << "    [COPY ctor: " << m_value << "]\n";
        // note it reads other.m_value -- a member function can access the
        // private members of ANY object of its own class, not just its own.
    }

    // If you don't write a copy constructor, the compiler generates one that
    // copies each member. That IMPLICIT copy constructor is correct for most
    // classes; you only need your own when the class owns a resource
    // (raw pointer, file handle) -- see chapter 22 for that whole story.
    //
    // To suppress copying entirely:  Volts(const Volts&) = delete;   (11.4)
    // To get the compiler's back:    Volts(const Volts&) = default;

    double value() const { return m_value; }
    void print(std::string_view label) const {
        std::cout << "  " << label << " = " << m_value << "V\n";
    }
};

void takeByValue(Volts v) {     // passing by value invokes the COPY constructor
    v.print("inside takeByValue");
}

void takeByRef(const Volts& v) { // by reference -- NO copy
    v.print("inside takeByRef");
}

Volts makeVolts() {
    return Volts { 12.6 };       // watch for elision here
}

// ------------------------ 14.16 CONVERTING CONSTRUCTORS ----------------------
//
// A constructor callable with ONE argument doubles as an implicit conversion
// from that argument's type to the class type. Sometimes handy, often a trap.

class Meters {
private:
    double m_value { };
public:
    Meters(double value) : m_value { value } { } // CONVERTING constructor
    double value() const { return m_value; }
};

class Feet {
private:
    double m_value { };
public:
    // 'explicit' blocks the implicit conversion. The constructor still works,
    // it just has to be requested by name.
    explicit Feet(double value) : m_value { value } { }
    double value() const { return m_value; }
};

void travel(Meters d) {
    std::cout << "  travelling " << d.value() << " meters\n";
}

void travelExplicit(Feet d) {
    std::cout << "  travelling " << d.value() << " feet\n";
}

// ------------------------ 14.17 CONSTEXPR CLASSES ----------------------------
//
// A class can be usable in constant expressions. Requirements:
//   - the constructor is constexpr
//   - the member functions you call are constexpr
//
// An AGGREGATE (no private members, no user constructors) is constexpr-capable
// for free -- just declare the object constexpr.

struct Point {          // aggregate: constexpr with no extra work
    double x { };
    double y { };
};

class Rect {
private:
    double m_w { };
    double m_h { };
public:
    constexpr Rect(double w, double h) : m_w { w }, m_h { h } { }

    constexpr double area() const { return m_w * m_h; }
    constexpr double perimeter() const { return 2 * (m_w + m_h); }
};

int main() {

    // ---------------- 14.13 TEMPORARY OBJECTS --------------------------------
    //
    // A temporary (anonymous) object has no name and dies at the end of the
    // full expression. Useful when you need a value exactly once.

    std::cout << "temporary objects:\n";
    takeByRef(Volts { 9.0 });   // constructed, used, destroyed on this line
                                // no variable is created at all

    // if you WOULD have written this, prefer the temporary above:
    //   Volts once { 9.0 };
    //   takeByRef(once);

    // ---------------- 14.14 the copy constructor -----------------------------
    std::cout << "\ncopy constructor:\n";
    Volts battery { 12.0 };

    std::cout << "  Volts copy { battery }:\n";
    Volts copy { battery };     // COPY ctor runs
    copy.print("copy");

    std::cout << "  takeByValue(battery):\n";
    takeByValue(battery);       // COPY ctor runs -- pass by value copies

    std::cout << "  takeByRef(battery):\n";
    takeByRef(battery);         // NO copy ctor -- this is why const& matters

    // ---------------- 14.15 COPY ELISION -------------------------------------
    //
    // The compiler is allowed to skip copy/move constructors entirely, even
    // when they have side effects (like our print statements). This is COPY
    // ELISION, and since C++17 it is MANDATORY in some cases.
    //
    // So you may see FEWER "[COPY ctor]" lines than you expect. That isn't a
    // bug -- it's the optimizer constructing the object directly in its final
    // location instead of building it and copying it.

    std::cout << "\ncopy elision -- note how few ctors actually run:\n";
    Volts made { makeVolts() };  // no copy: constructed directly into 'made'
    made.print("made");

    // This is why "return by value is slow" is outdated advice. Return by
    // value and let the compiler elide.

    // ---------------- 14.16 implicit vs explicit ------------------------------
    std::cout << "\nconverting constructors:\n";

    travel(Meters { 100.0 });  // explicit and clear
    travel(100.0);             // IMPLICIT conversion: double -> Meters.
                               // Convenient, but the call site no longer says
                               // what unit 100.0 is in.

    travelExplicit(Feet { 328.0 }); // must name the type -- that's the point
    // travelExplicit(328.0);       // ERROR if uncommented: Feet is explicit,
    //                              // so no implicit conversion is allowed

    // BEST PRACTICE: make single-argument constructors 'explicit' by default.
    // Relax it only when the conversion is genuinely lossless and obvious
    // (std::string from const char* is the classic exception).
    //
    // Note: only ONE user-defined conversion is applied implicitly in a chain,
    // so implicit conversions don't cascade indefinitely.

    // ---------------- 14.17 constexpr classes ---------------------------------
    std::cout << "\nconstexpr classes:\n";

    constexpr Point origin { 0.0, 0.0 };        // aggregate, constexpr for free
    constexpr Point corner { 3.0, 4.0 };
    std::cout << "  corner = (" << corner.x << ", " << corner.y << ")\n";

    constexpr Rect box { 3.0, 4.0 };            // constexpr ctor runs at COMPILE time
    constexpr double area { box.area() };       // so does this
    std::cout << "  box area (computed at compile time) = " << area << "\n";
    std::cout << "  box perimeter = " << box.perimeter() << "\n";

    // the same class still works perfectly well at runtime
    double w { 5.0 };
    Rect runtime { w, 2.0 };
    std::cout << "  runtime rect area = " << runtime.area() << "\n";

    (void)origin; // unused, just here to show the constexpr aggregate compiles

    return 0;
}

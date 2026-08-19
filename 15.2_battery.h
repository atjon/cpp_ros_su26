#ifndef BATTERY_15_2_H
#define BATTERY_15_2_H

#include <string>

// 15.2 - Classes and header files (the HEADER)
//
// THE PATTERN: the class DEFINITION goes in the header, the member function
// DEFINITIONS go in the matching .cpp.
//
// Why split at all?
//   - the header stays short and readable -- it IS the documentation of the
//     type's interface
//   - changing a member function's BODY only recompiles the .cpp, not every
//     file that includes the header
//
// Why is putting the class definition in a header not an ODR violation?
// Types are exempt from the one-definition-rule across translation units,
// as long as every definition is IDENTICAL. That's what the header guarantees.

class Battery {
private:
    double m_volts { 12.0 };
    double m_capacityAh { 5.0 };
    std::string m_label { "unnamed" };

public:
    Battery() = default;
    Battery(double volts, double capacityAh, const std::string& label);

    // ---------------- DEFINED IN THE HEADER ----------------------------------
    //
    // A member function defined INSIDE the class body is implicitly INLINE,
    // so it can appear in every translation unit without a duplicate-symbol
    // error. That's why these are legal here.
    //
    // Keep short one-liners here (getters) -- the compiler can inline them,
    // and moving them to the .cpp would cost more in noise than it saves.

    double volts() const { return m_volts; }
    double capacityAh() const { return m_capacityAh; }
    const std::string& label() const { return m_label; }

    // ---------------- DECLARED HERE, DEFINED IN THE .CPP ---------------------
    // Anything with real logic goes in the .cpp.

    double wattHours() const;
    void drain(double amountVolts);
    void print() const;
};

// A non-member function defined in a header MUST be marked inline explicitly
// (unlike member functions defined inside the class, which get it for free).
// Without 'inline' here, every .cpp that includes this header would emit its
// own copy of the symbol and the linker would reject the duplicates.
inline bool isLowVoltage(const Battery& b) {
    return b.volts() < 11.0;
}

#endif

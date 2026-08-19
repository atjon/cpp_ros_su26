#include "15.2_battery.h"
#include <iostream>

// 15.2 - Classes and header files (the IMPLEMENTATION)
//
// Every definition here must be qualified with Battery:: so the compiler knows
// these belong to the class rather than being free functions.
//
// Note what does NOT get repeated in this file:
//   - default arguments (they live in the header, same rule as 11.5)
//   - the 'explicit' keyword
//   - access specifiers
// Repeating any of those is a compile error.
//
// What DOES get repeated: 'const' on a const member function. That's part of
// the signature, so leaving it off here would declare a DIFFERENT function
// and produce a confusing "no declaration matches" error.

Battery::Battery(double volts, double capacityAh, const std::string& label)
    : m_volts { volts }
    , m_capacityAh { capacityAh }
    , m_label { label }
{
}

double Battery::wattHours() const {   // the trailing const is REQUIRED here
    return m_volts * m_capacityAh;
}

void Battery::drain(double amountVolts) {
    m_volts -= amountVolts;
    if (m_volts < 0.0)
        m_volts = 0.0;
}

void Battery::print() const {
    std::cout << "  " << m_label << ": " << m_volts << "V "
              << m_capacityAh << "Ah -> " << wattHours() << "Wh"
              << (isLowVoltage(*this) ? "  [LOW]" : "") << "\n";
    //            ^ *this passes the current object by reference (15.1)
}

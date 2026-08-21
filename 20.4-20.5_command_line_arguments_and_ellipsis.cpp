#include <iostream>
#include <string>
#include <string_view>
#include <cstdarg>   // for the ellipsis machinery: va_list, va_start, va_arg, va_end
#include <stdexcept> // for std::invalid_argument / std::out_of_range
#include <initializer_list>

// 20.4 - Command line arguments
// 20.5 - Ellipsis (and why to avoid them)

// ------------------------ 20.5 ELLIPSIS --------------------------------------
//
// The ellipsis (...) lets a function take a VARIABLE number of arguments.
// It must be the LAST parameter, and there must be at least one named
// parameter before it.
//
// THIS IS A LANDMINE. Read this section to understand legacy code (printf is
// built on it), then never write one yourself. The reasons are below.

double averageOf(int count, ...) {
    va_list args;             // holds the state of the argument walk
    va_start(args, count);    // start AFTER the last named parameter

    double sum { 0.0 };
    for (int i { 0 }; i < count; ++i)
        sum += va_arg(args, int);  // you must state the type YOURSELF.
                                   // Guess wrong and you get garbage, silently.

    va_end(args);             // mandatory cleanup
    return count > 0 ? sum / count : 0.0;
}

// WHY ELLIPSIS IS DANGEROUS:
//
//  1. NO TYPE CHECKING AT ALL. The compiler cannot verify what you passed.
//     Passing a double and reading it as an int is undefined behavior, and
//     nothing warns you.
//
//  2. THE FUNCTION CAN'T KNOW HOW MANY ARGUMENTS IT GOT. You have to tell it
//     out-of-band -- via a count parameter (above), a sentinel value, or a
//     format string (printf's approach). All three can be gotten wrong by the
//     caller, and all three fail at RUNTIME, not compile time.
//
//  3. Arguments undergo default promotions (char/short -> int, float ->
//     double), so va_arg(args, char) is ALWAYS wrong.
//
// THE MODERN REPLACEMENTS, in rough order of preference:
//   - std::vector / std::array / std::initializer_list for homogeneous data
//   - variadic TEMPLATES (chapter 26) for heterogeneous data, fully type-checked
//   - std::format (C++20) instead of printf

// a type-safe alternative using an initializer_list -- what you should write:
double averageSafe(std::initializer_list<int> values) {
    if (values.size() == 0) return 0.0;
    double sum { 0.0 };
    for (int v : values)   // the size is KNOWN, and the type is CHECKED
        sum += v;
    return sum / static_cast<double>(values.size());
}

// ------------------------ 20.4 COMMAND LINE ARGUMENTS ------------------------
//
// main() has a second, longer form:
//
//     int main(int argc, char* argv[])
//
//   argc -- "argument count", how many arguments were passed
//   argv -- "argument values", an array of C-style strings
//
// argv[0] is (conventionally) the program's own name or path, so the real
// user arguments start at argv[1] and argc is always at least 1.
//
// Arguments arrive as TEXT. Converting "42" to the number 42 is your job.

int main(int argc, char* argv[]) {

    // ---------------- 20.4 reading the arguments -----------------------------
    std::cout << "command line arguments:\n";
    std::cout << "  argc = " << argc << "\n";

    for (int i { 0 }; i < argc; ++i)
        std::cout << "  argv[" << i << "] = " << argv[i] << "\n";

    if (argc <= 1) {
        std::cout << "\n  (no user arguments passed -- try running it as:\n"
                     "   ./build/20.4-20.5_command_line_arguments_and_ellipsis 42 hello 7)\n";
    }

    // CONVERTING an argument to a number. Arguments are always strings, so
    // this step is unavoidable -- and it can fail, so handle that.
    if (argc > 1) {
        std::cout << "\n  converting argv[1] to a number:\n";
        try {
            int value { std::stoi(argv[1]) };
            std::cout << "    std::stoi(\"" << argv[1] << "\") = " << value << "\n";
            std::cout << "    doubled = " << value * 2 << "\n";
        } catch (const std::invalid_argument&) {
            std::cout << "    \"" << argv[1] << "\" is not a number\n";
        } catch (const std::out_of_range&) {
            std::cout << "    \"" << argv[1] << "\" doesn't fit in an int\n";
        }
    }

    // treating argv entries as string_view is convenient and copy-free
    if (argc > 2) {
        std::string_view second { argv[2] };
        std::cout << "  argv[2] as string_view: \"" << second
                  << "\" (" << second.length() << " chars)\n";
    }

    // ---------------- 20.5 the ellipsis --------------------------------------
    std::cout << "\nellipsis (legacy, avoid writing new code like this):\n";

    std::cout << "  averageOf(3, 10, 20, 30)  = " << averageOf(3, 10, 20, 30) << "\n";
    std::cout << "  averageOf(5, 1, 2, 3, 4, 5) = "
              << averageOf(5, 1, 2, 3, 4, 5) << "\n";

    // HOW IT BREAKS -- both of these COMPILE without a single warning:
    //
    //   averageOf(6, 10, 20, 30);   // count LIES: reads 3 arguments that were
    //                               // never passed. Garbage, or a crash.
    //
    //   averageOf(3, 1.5, 2.5, 3.5); // doubles read as ints. Total nonsense.
    //
    // Neither mistake is detectable by the compiler. That's the case against it.

    std::cout << "\ntype-safe replacement:\n";
    std::cout << "  averageSafe({10, 20, 30})  = " << averageSafe({ 10, 20, 30 }) << "\n";
    std::cout << "  averageSafe({1, 2, 3, 4, 5}) = "
              << averageSafe({ 1, 2, 3, 4, 5 }) << "\n";
    // averageSafe({1.5, 2.5});  // ERROR if uncommented -- narrowing is CAUGHT.
    //                           // The ellipsis version would have accepted it
    //                           // and produced garbage.

    return 0;
}

#include <iostream>
#include <vector>
#include <string>

// 16.6 - Arrays and loops
// 16.7 - Arrays, loops, and sign challenge solutions
// 16.8 - Range-based for loops (for-each)

// ------------------------ 16.6 THE POINT OF ARRAYS ---------------------------
//
// Arrays and loops are made for each other. The loop variable IS the index,
// so a single body handles every element -- and the code is identical whether
// there are 4 elements or 4 million.
//
// This pattern (visit every element in order) is called a TRAVERSAL.

int main() {

    std::vector readings { 42, 17, 91, 8, 63, 25 };

    // ---------------- the basic traversal ------------------------------------
    std::cout << "index-based traversal:\n    ";

    // size() is unsigned (16.3), so the loop variable is too. Inside a loop
    // that only counts UP and starts at 0, unsigned is harmless.
    for (std::size_t i { 0 }; i < readings.size(); ++i)
        std::cout << readings[i] << ' ';
    std::cout << '\n';

    // classic array algorithms, all the same shape:
    int max { readings[0] };
    int total { 0 };
    std::size_t maxIndex { 0 };

    for (std::size_t i { 0 }; i < readings.size(); ++i) {
        total += readings[i];
        if (readings[i] > max) {
            max = readings[i];
            maxIndex = i;
        }
    }

    std::cout << "  sum = " << total
              << ", average = " << total / static_cast<int>(readings.size())
              << ", max = " << max << " at index " << maxIndex << '\n';

    // ---------------- 16.7 THE SIGN CHALLENGE --------------------------------
    //
    // The trouble starts when the index has to be signed -- because you're
    // counting down, or doing arithmetic that can go negative.
    //
    // BROKEN:
    //     for (std::size_t i { readings.size() - 1 }; i >= 0; --i)
    //
    // i is unsigned, so `i >= 0` is ALWAYS true. When i is 0, `--i` wraps
    // around to a huge value and the loop keeps going into invalid memory.

    std::cout << "\ncounting down, three ways:\n";

    // SOLUTION 1 (C++20, preferred): std::ssize gives a SIGNED length.
    std::cout << "  std::ssize:  ";
    for (auto i { std::ssize(readings) - 1 }; i >= 0; --i)
        std::cout << readings[static_cast<std::size_t>(i)] << ' ';
    std::cout << '\n';

    // SOLUTION 2: keep the index unsigned, but shift the loop by one so the
    // condition is `i > 0` and you subscript with i-1. Ugly but correct.
    std::cout << "  offset by 1: ";
    for (std::size_t i { readings.size() }; i > 0; --i)
        std::cout << readings[i - 1] << ' ';
    std::cout << '\n';

    // SOLUTION 3: use a signed type and convert only at the subscript.
    // A tiny helper keeps the casts out of the loop body.
    std::cout << "  signed index: ";
    for (int i { static_cast<int>(readings.size()) - 1 }; i >= 0; --i)
        std::cout << readings[static_cast<std::size_t>(i)] << ' ';
    std::cout << '\n';

    // The reason casts are needed at all: subscripting with a signed value
    // triggers a signed->unsigned conversion warning under -Wsign-conversion.
    // The static_cast says "yes, I know, and I've checked it's non-negative."

    // ---------------- 16.8 RANGE-BASED FOR (for-each) -------------------------
    //
    // Most traversals don't actually need the index. The range-based for loop
    // hands you each ELEMENT directly:
    //
    //     for (declaration : container) { ... }
    //
    // No index, so no off-by-one, no sign problem, and no way to run off the
    // end. This is why it should be your DEFAULT loop over a container.

    std::cout << "\nrange-based for:\n    ";
    for (int reading : readings)     // reading is a COPY of each element
        std::cout << reading << ' ';
    std::cout << '\n';

    // ---------------- const auto& is the right default ------------------------
    //
    // `for (T e : v)` COPIES every element. For int that's free; for
    // std::string or a big class it's a real cost, repeated once per element.

    std::vector<std::string> labels { "front_left", "front_right", "rear" };

    std::cout << "  copying (wasteful for strings):\n    ";
    for (std::string label : labels)        // one string copy per iteration
        std::cout << label << ' ';
    std::cout << '\n';

    std::cout << "  const auto& (no copies):\n    ";
    for (const auto& label : labels)        // binds directly, copies nothing
        std::cout << label << ' ';
    std::cout << '\n';

    // BEST PRACTICE: use `const auto&` unless you specifically want a copy
    // or need to modify. It's never slower and often much faster.

    // to MODIFY the elements, take a non-const reference:
    std::cout << "  modifying through auto&:\n";
    for (auto& reading : readings)
        reading += 100;
    std::cout << "    ";
    for (const auto& r : readings) std::cout << r << ' ';
    std::cout << '\n';

    // ---------------- what it can't do ----------------------------------------
    //
    // A range-based for gives you no index and no control over the order.
    // Use an indexed loop when you need to:
    //   - know WHERE an element is (like maxIndex above)
    //   - iterate backwards, or by twos
    //   - compare an element to its neighbour
    //   - stop partway and report the position

    std::cout << "\n  finding a position needs the index:\n";
    for (std::size_t i { 0 }; i < readings.size(); ++i) {
        if (readings[i] > 150) {
            std::cout << "    first over 150 is " << readings[i]
                      << " at index " << i << '\n';
            break;
        }
    }

    // ---------------- range-based for with C++20 init -------------------------
    // you can declare a variable in the loop, scoped to it
    for (int count { 0 }; const auto& label : labels)
        std::cout << "    label " << count++ << ": " << label << '\n';

    // ---------------- a gotcha: modifying while iterating ---------------------
    //
    //   for (const auto& r : readings)
    //       readings.push_back(r);      // UNDEFINED BEHAVIOR
    //
    // push_back may REALLOCATE the vector's storage (16.10), which invalidates
    // everything the loop is holding onto. Never add to or remove from a
    // container you're currently iterating over.

    std::cout << "\n  never push_back() into the container you're looping over --\n"
                 "  reallocation invalidates the loop's references.\n";

    return 0;
}

#include <iostream>
#include <vector>
#include <string>
#include <string_view>

// 16.10 - std::vector resizing and capacity
// 16.11 - std::vector and stack behavior
// 16.12 - std::vector<bool>

// A vector's defining feature is that it can change LENGTH at runtime. This
// file is about what that actually costs, and how to avoid paying it.

void report(std::string_view label, const std::vector<int>& v) {
    std::cout << "  " << label
              << " length=" << v.size()
              << " capacity=" << v.capacity() << '\n';
}

int main() {

    // ---------------- 16.10 LENGTH vs CAPACITY --------------------------------
    //
    // These are two DIFFERENT numbers and confusing them causes real bugs:
    //
    //   LENGTH   (size())     -- how many elements are actually IN USE
    //   CAPACITY (capacity()) -- how many the allocated memory can HOLD
    //
    // capacity >= length, always. The extra room is why push_back is usually
    // cheap: most of the time there's already space for the new element.

    std::cout << "length vs capacity:\n";

    std::vector<int> v { 1, 2, 3 };
    report("initial       ", v);

    // resize() changes the LENGTH.
    v.resize(6);      // new elements are VALUE-INITIALIZED (0 for int)
    report("after resize(6)", v);
    std::cout << "    contents: ";
    for (int e : v) std::cout << e << ' ';
    std::cout << " <- the new ones are zeroed\n";

    v.resize(2);      // shrinking DESTROYS the extra elements...
    report("after resize(2)", v);
    std::cout << "    ...but capacity usually STAYS -- the memory is kept\n"
                 "    in case you grow again. Shrinking doesn't free.\n";

    // ---------------- reallocation --------------------------------------------
    //
    // When push_back needs room and capacity is full, the vector must:
    //   1. allocate a bigger block
    //   2. copy or move every existing element into it
    //   3. destroy the old elements and free the old block
    //
    // That is a REALLOCATION, and it's O(n). It also INVALIDATES every
    // pointer, reference, and iterator into the vector.

    std::cout << "\nwatching reallocation:\n";

    std::vector<int> growing { };
    std::size_t lastCapacity { growing.capacity() };

    for (int i { 0 }; i < 20; ++i) {
        growing.push_back(i);
        if (growing.capacity() != lastCapacity) {
            std::cout << "    REALLOCATED at length " << growing.size()
                      << ": capacity " << lastCapacity
                      << " -> " << growing.capacity() << '\n';
            lastCapacity = growing.capacity();
        }
    }

    // Notice capacity roughly DOUBLES each time rather than growing by one.
    // That's deliberate: it makes push_back amortized O(1) -- reallocations
    // get rarer as the vector gets bigger.

    // ---------------- reserve() -----------------------------------------------
    //
    // If you know roughly how many elements you'll add, reserve() allocates
    // the capacity UP FRONT, so no reallocation happens at all.
    //
    // reserve() changes CAPACITY only. resize() changes LENGTH.

    std::cout << "\nreserve() avoids all of that:\n";

    std::vector<int> reserved { };
    reserved.reserve(20);
    report("after reserve(20)", reserved);

    lastCapacity = reserved.capacity();
    for (int i { 0 }; i < 20; ++i) reserved.push_back(i);

    std::cout << "    after 20 push_backs, capacity is still "
              << reserved.capacity()
              << (reserved.capacity() == lastCapacity
                      ? " -- zero reallocations\n" : " -- it grew\n");

    // ---------------- THE INVALIDATION TRAP -----------------------------------
    //
    //     std::vector<int> v { 1, 2, 3 };
    //     int& ref { v[0] };       // reference into the vector's storage
    //     v.push_back(4);          // may REALLOCATE, moving that storage
    //     std::cout << ref;        // UNDEFINED BEHAVIOR -- dangling
    //
    // The reference points into the OLD block, which has been freed. This is
    // the single most common vector bug. Any operation that can change the
    // capacity invalidates everything.

    std::cout << "\n  never hold a reference/pointer/iterator across a\n"
                 "  push_back, resize, insert, or reserve.\n";

    // ---------------- 16.11 STACK BEHAVIOR -------------------------------------
    //
    // A STACK is last-in-first-out: you push onto the top and pop from the top.
    // std::vector supports this natively, and it's the reason vector is the
    // default stack container.

    std::cout << "\nstack behavior:\n";

    std::vector<std::string> callStack { };

    callStack.push_back("main");          // copies the argument in
    callStack.push_back("navigate");
    callStack.emplace_back("planPath");   // CONSTRUCTS in place -- no temporary

    std::cout << "  stack: ";
    for (const auto& f : callStack) std::cout << f << " ";
    std::cout << "\n  top (back()) = " << callStack.back() << '\n';
    std::cout << "  bottom (front()) = " << callStack.front() << '\n';

    // pop_back() removes the top element and returns NOTHING. If you want the
    // value, read back() FIRST -- reading it after the pop is UB.
    std::string popped { callStack.back() };
    callStack.pop_back();
    std::cout << "  popped \"" << popped << "\", "
              << callStack.size() << " left\n";

    while (!callStack.empty()) {          // empty() is clearer than size() == 0
        std::cout << "    unwinding: " << callStack.back() << '\n';
        callStack.pop_back();
    }

    // emplace_back vs push_back: push_back builds the object then moves/copies
    // it in; emplace_back forwards the arguments and builds it in place.
    // Prefer emplace_back when you'd otherwise be constructing a temporary.

    // (std::stack exists too -- it's a wrapper over a vector or deque that
    //  hides everything except push/pop/top, if you want that restriction
    //  enforced.)

    // ---------------- 16.12 std::vector<bool> ---------------------------------
    //
    // WARNING: std::vector<bool> is NOT a normal vector. It's a special-cased
    // partial specialization that packs 8 booleans per BYTE to save space.
    //
    // That optimization breaks the container's own rules:

    std::cout << "\nstd::vector<bool> -- the standard library's known mistake:\n";

    std::vector<bool> flags { true, false, true, true };

    std::cout << "  it works for reading: " << flags[0] << flags[1]
              << flags[2] << flags[3] << '\n';

    // But operator[] does NOT return bool& -- there's no such thing as a
    // reference to a single BIT. It returns a PROXY OBJECT that pretends to be
    // one. Consequences:
    //
    //   auto value { flags[0] };     // NOT bool -- it's the proxy type, and
    //                                // it stays tied to the vector
    //   bool& ref { flags[0] };      // ERROR: can't bind, no real bool exists
    //   bool* ptr { &flags[0] };     // ERROR: nothing to take the address of
    //
    // And this fails in a range-based for:
    //   for (auto& flag : flags)     // ERROR -- no bool& to bind to

    bool copied { flags[0] };     // fine -- copy it into a REAL bool
    std::cout << "  reading into a real bool works: " << copied << '\n';

    std::cout << "  but auto/references/pointers to elements DON'T.\n";

    // BEST PRACTICE: avoid std::vector<bool>. Use one of:
    //   std::vector<char>   -- if you want a real, addressable container
    //   std::bitset<N>      -- if the size is known at compile time
    //   std::vector<std::uint8_t> -- explicit about the storage
    //
    // Only use vector<bool> when the memory saving genuinely matters and you
    // know about the proxy.

    return 0;
}

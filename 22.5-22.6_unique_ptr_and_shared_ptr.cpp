#include <iostream>
#include <memory>   // for std::unique_ptr, std::shared_ptr, make_unique, make_shared
#include <string>
#include <utility>  // for std::move
#include <vector>

// 22.5 - std::unique_ptr
// 22.6 - std::shared_ptr

// A toy resource so ownership and destruction are visible in the output.
class Motor {
private:
    std::string m_name { };

public:
    explicit Motor(std::string name) : m_name { std::move(name) } {
        std::cout << "    [Motor " << m_name << " constructed]\n";
    }

    ~Motor() {
        std::cout << "    [Motor " << m_name << " destroyed]\n";
    }

    void spin() const { std::cout << "    " << m_name << " spinning\n"; }
    const std::string& name() const { return m_name; }
};

// ------------------------ FUNCTION SIGNATURES THAT EXPRESS OWNERSHIP ---------
//
// This is the underrated benefit of smart pointers: the SIGNATURE documents
// what happens to ownership, and the compiler enforces it.

// "I take ownership" -- the caller must std::move into this, and loses it
void consume(std::unique_ptr<Motor> motor) {
    std::cout << "    consume() now owns " << motor->name() << "\n";
} // destroyed here, at the end of consume()

// "I just need to use it" -- takes a raw pointer or reference, NOT a smart
// pointer. This does NOT participate in ownership at all, which is correct.
void use(const Motor* motor) {
    if (motor) motor->spin();
}

// "I share ownership" -- a copy of the shared_ptr bumps the reference count
void share(std::shared_ptr<Motor> motor) {
    std::cout << "    share() holds it, use_count = " << motor.use_count() << "\n";
}

std::unique_ptr<Motor> makeMotor(const std::string& name) {
    return std::make_unique<Motor>(name);  // returning by value MOVES it out
}

int main() {

    // ---------------- 22.5 std::unique_ptr -----------------------------------
    //
    // unique_ptr is the RAII pointer with EXCLUSIVE ownership:
    //   - exactly one unique_ptr owns the object at a time
    //   - it deletes the object in its destructor, automatically
    //   - it CANNOT be copied (copying is =delete'd) -- only MOVED
    //   - it has zero overhead vs a raw pointer in release builds
    //
    // This should be your default smart pointer.

    std::cout << "unique_ptr:\n";
    {
        // ALWAYS prefer std::make_unique over "new". It's exception-safe and
        // you never write 'new' at all.
        auto left { std::make_unique<Motor>("left") };

        left->spin();               // -> works like a raw pointer
        (*left).spin();             // * works too
        std::cout << "    name via get(): " << left.get()->name() << "\n";
        //                              ^ get() returns the raw pointer.
        //                                Use it to PASS to code that just
        //                                needs to look -- never to delete.

        std::cout << "    left is " << (left ? "holding" : "empty") << "\n";

        // auto copy { left };      // ERROR if uncommented: copying is deleted.
        //                          // This is the compiler PREVENTING a double
        //                          // free, at compile time.

        // transferring ownership requires an explicit move -- ownership
        // transfer is never accidental
        auto moved { std::move(left) };
        std::cout << "    after move, left is " << (left ? "holding" : "empty")
                  << ", moved is " << (moved ? "holding" : "empty") << "\n";

        // use(left.get());  // would pass nullptr now -- left owns nothing
        use(moved.get());    // non-owning access, the right way to "just look"

        std::cout << "  (leaving scope -- destructor runs automatically)\n";
    }

    std::cout << "\n  ownership transfer into a function:\n";
    {
        auto rear { std::make_unique<Motor>("rear") };
        consume(std::move(rear));   // must be explicit
        std::cout << "    back in main, rear is "
                  << (rear ? "holding" : "empty (ownership was transferred)") << "\n";
    }

    std::cout << "\n  returning a unique_ptr from a factory:\n";
    {
        auto made { makeMotor("factory_made") };
        made->spin();
    }

    // unique_ptr in a container works, because the container can MOVE
    std::cout << "\n  unique_ptr in a vector:\n";
    {
        std::vector<std::unique_ptr<Motor>> motors;
        motors.push_back(std::make_unique<Motor>("vec_a"));
        motors.push_back(std::make_unique<Motor>("vec_b"));

        for (const auto& m : motors)   // note const auto& -- copying would fail
            m->spin();

        std::cout << "  (vector going out of scope -- all owned motors freed)\n";
    }

    // for arrays, unique_ptr<T[]> exists and calls delete[] --
    // though std::vector is almost always the better answer.

    // ---------------- 22.6 std::shared_ptr -----------------------------------
    //
    // shared_ptr allows SHARED ownership. It keeps a reference count:
    //   - copying it increments the count
    //   - each destruction decrements it
    //   - the object is deleted when the count hits ZERO
    //
    // The cost: a separate heap-allocated CONTROL BLOCK holding the counts,
    // and atomic increments/decrements so it's thread-safe. Use it only when
    // ownership genuinely is shared -- not as a default.

    std::cout << "\nshared_ptr:\n";
    {
        auto first { std::make_shared<Motor>("shared") };
        // make_shared is preferred over shared_ptr<Motor>(new Motor(...)):
        // it allocates the object AND the control block in ONE allocation.

        std::cout << "    use_count after creation: " << first.use_count() << "\n";

        {
            auto second { first };   // a COPY -- perfectly legal here
            std::cout << "    use_count after a copy: " << first.use_count() << "\n";

            share(first);            // passing by value copies again, temporarily
            std::cout << "    use_count after share() returned: "
                      << first.use_count() << "\n";

            second->spin();
            std::cout << "    both point to the same object: "
                      << (first.get() == second.get()) << "\n";

            std::cout << "  (inner scope ending -- second is destroyed)\n";
        }

        std::cout << "    use_count back to: " << first.use_count() << "\n";
        std::cout << "  (outer scope ending -- count hits 0, object is freed)\n";
    }

    // ---------------- a critical mistake -------------------------------------
    //
    // NEVER construct two smart pointers from the SAME raw pointer:
    //
    //     Motor* raw { new Motor{"bad"} };
    //     std::shared_ptr<Motor> p1 { raw };
    //     std::shared_ptr<Motor> p2 { raw };   // SEPARATE control block!
    //
    // Each has its own count of 1, so BOTH will delete it. Double free.
    // Using make_shared/make_unique makes this mistake impossible to write.
    //
    // To legitimately add an owner, COPY the existing shared_ptr -- that's
    // what shares the control block.

    std::cout << "\nsummary:\n";
    std::cout << "  unique_ptr -- exclusive ownership, zero overhead. Default choice.\n";
    std::cout << "  shared_ptr -- shared ownership, refcount cost. Use when needed.\n";
    std::cout << "  raw pointer/reference -- non-owning observation. Still correct!\n";

    return 0;
}

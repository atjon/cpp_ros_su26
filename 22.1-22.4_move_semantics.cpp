#include <iostream>
#include <string>
#include <utility> // for std::move
#include <vector>

// 22.1 - Introduction to smart pointers and move semantics
// 22.2 - R-value references
// 22.3 - Move constructors and move assignment
// 22.4 - std::move

// ------------------------ 22.1 THE PROBLEM -----------------------------------
//
// Manual new/delete (20.2) is easy to get wrong:
//
//   void f() {
//       Resource* r { new Resource };
//       if (earlyExit) return;   // LEAK -- delete never runs
//       doWork();                // if this THROWS, also a leak
//       delete r;
//   }
//
// The fix is RAII (15.4): wrap the pointer in a class whose DESTRUCTOR does
// the delete. Then cleanup is automatic on every exit path, including
// exceptions. That wrapper is a SMART POINTER.
//
// But a naive RAII wrapper has a fatal flaw: if you COPY it, two objects hold
// the same pointer and BOTH will delete it -- a double free. So we need a way
// to TRANSFER ownership rather than duplicate it. That is MOVE SEMANTICS.

// ------------------------ 22.2 R-VALUE REFERENCES ----------------------------
//
// Recall value categories from 12.2:
//   lvalue -- has an identity, persists (a named variable)
//   rvalue -- a temporary, about to be destroyed
//
// An R-VALUE REFERENCE (T&&) binds ONLY to rvalues. That's the whole trick:
// if a reference binds to an rvalue, the object is a temporary nobody else
// will ever look at again -- so it is SAFE TO GUT IT and steal its contents.

void takes(const std::string& s) { std::cout << "    lvalue ref overload\n"; }
void takes(std::string&& s)      { std::cout << "    rvalue ref overload\n"; }

// ------------------------ 22.3 A CLASS WITH MOVE SEMANTICS -------------------
//
// A toy resource-owning class, to make the copies and moves visible.

class Buffer {
private:
    int* m_data { nullptr };
    std::size_t m_size { 0 };
    std::string m_name { };

public:
    Buffer(std::string name, std::size_t size)
        : m_data { new int[size] { } }, m_size { size }, m_name { std::move(name) } {
        std::cout << "    [CTOR " << m_name << ", " << m_size << " ints]\n";
    }

    // DESTRUCTOR -- the RAII half. Runs automatically, always.
    ~Buffer() {
        std::cout << "    [DTOR " << m_name
                  << (m_data ? "" : " (already moved from)") << "]\n";
        delete[] m_data;   // deleting nullptr is SAFE and does nothing,
                           // which is what makes the moved-from state work
    }

    // COPY CONSTRUCTOR -- DEEP copy. Expensive: allocates and copies everything.
    Buffer(const Buffer& other)
        : m_data { new int[other.m_size] { } }
        , m_size { other.m_size }
        , m_name { other.m_name + "_copy" } {
        for (std::size_t i { 0 }; i < m_size; ++i)
            m_data[i] = other.m_data[i];
        std::cout << "    [COPY CTOR " << m_name << " -- allocated "
                  << m_size << " ints]\n";
    }

    // MOVE CONSTRUCTOR -- STEALS the pointer. No allocation, no copying.
    // Takes an RVALUE reference, so it only binds to temporaries.
    // Marked noexcept: std::vector will only use a move constructor during
    // reallocation if it's noexcept, otherwise it falls back to copying.
    Buffer(Buffer&& other) noexcept
        : m_data { other.m_data }        // steal the pointer
        , m_size { other.m_size }
        , m_name { std::move(other.m_name) } {

        // CRITICAL: leave the source in a valid, destructible state.
        // If we skipped this, other's destructor would delete the memory
        // we just took -- and then ours would delete it again. Double free.
        other.m_data = nullptr;
        other.m_size = 0;

        std::cout << "    [MOVE CTOR " << m_name << " -- stole the pointer]\n";
    }

    // COPY ASSIGNMENT
    Buffer& operator=(const Buffer& other) {
        std::cout << "    [COPY ASSIGN]\n";
        if (this == &other) return *this;   // SELF-ASSIGNMENT check, essential

        delete[] m_data;                     // release what we had
        m_size = other.m_size;
        m_data = new int[m_size] { };
        for (std::size_t i { 0 }; i < m_size; ++i)
            m_data[i] = other.m_data[i];
        m_name = other.m_name + "_copy";
        return *this;
    }

    // MOVE ASSIGNMENT
    Buffer& operator=(Buffer&& other) noexcept {
        std::cout << "    [MOVE ASSIGN]\n";
        if (this == &other) return *this;

        delete[] m_data;                     // release what we had
        m_data = other.m_data;               // steal
        m_size = other.m_size;
        m_name = std::move(other.m_name);

        other.m_data = nullptr;              // and null the source
        other.m_size = 0;
        return *this;
    }

    // THE RULE OF FIVE: if you write any one of
    //   destructor, copy ctor, copy assign, move ctor, move assign
    // you almost certainly need all five. (The RULE OF ZERO is better still:
    // use members that manage themselves -- std::vector, std::string,
    // std::unique_ptr -- and write NONE of them.)

    std::size_t size() const { return m_size; }
    const std::string& name() const { return m_name; }
    bool valid() const { return m_data != nullptr; }
};

Buffer makeBuffer() {
    return Buffer { "temp", 100 };
}

int main() {

    // ---------------- 22.2 rvalue references ---------------------------------
    std::cout << "rvalue references:\n";

    std::string named { "an lvalue" };
    takes(named);                       // lvalue -> const& overload
    takes(std::string { "a temporary" }); // rvalue -> && overload
    takes("a literal");                 // creates a temporary -> && overload

    // an rvalue reference variable is itself an LVALUE (it has a name!).
    // This catches everyone at least once.
    std::string&& rref { std::string { "bound" } };
    takes(rref);   // calls the LVALUE overload, because rref has a name
    std::cout << "    ^ rref is an rvalue REFERENCE but an lvalue EXPRESSION\n";

    // ---------------- 22.3 copy vs move --------------------------------------
    std::cout << "\ncopy vs move:\n";

    std::cout << "  Buffer a { \"original\", 5 }:\n";
    Buffer a { "original", 5 };

    std::cout << "  Buffer b { a };            <- a is an lvalue\n";
    Buffer b { a };                      // COPY -- a is still needed afterwards

    std::cout << "  Buffer c { makeBuffer() }; <- the result is an rvalue\n";
    Buffer c { makeBuffer() };           // MOVE (or elided entirely -- see 14.15)

    std::cout << "  Buffer d { std::move(a) }; <- forced\n";
    Buffer d { std::move(a) };           // MOVE, because we said so

    // ---------------- 22.4 std::move -----------------------------------------
    //
    // std::move does NOT move anything. It is purely a CAST: it converts an
    // lvalue into an rvalue reference, so that overload resolution picks the
    // move overload. The actual moving is done by the move constructor.
    //
    // Think of it as "I'm done with this -- feel free to gut it."

    std::cout << "\nafter std::move(a):\n";
    std::cout << "  a.valid() = " << a.valid() << "  <- a was gutted\n";
    std::cout << "  d.valid() = " << d.valid() << "  <- d took the resource\n";

    // THE MOVED-FROM STATE: 'a' is still a valid object -- it can be destroyed
    // and assigned to -- but its VALUE is unspecified. Don't read it expecting
    // anything meaningful. Assign a fresh value first, or just stop using it.

    std::cout << "\n  reassigning a moved-from object is fine:\n";
    a = Buffer { "reborn", 3 };   // MOVE ASSIGN from a temporary
    std::cout << "  a.valid() = " << a.valid() << " again\n";

    // ---------------- where this pays off in practice -------------------------
    std::cout << "\nwhy it matters -- std::vector reallocation:\n";
    {
        std::vector<Buffer> buffers;
        buffers.reserve(2);   // reserve so we control when reallocation happens

        std::cout << "  push_back(Buffer{...}) -- an rvalue, so it MOVES:\n";
        buffers.push_back(Buffer { "vec0", 4 });

        Buffer keepMe { "vec1", 4 };
        std::cout << "  push_back(keepMe) -- an lvalue, so it COPIES:\n";
        buffers.push_back(keepMe);

        std::cout << "  push_back(std::move(keepMe)) would MOVE instead.\n";
        std::cout << "  (leaving the vector's scope now)\n";
    }

    // Before C++11, every one of those was a deep copy. Move semantics is why
    // returning a large std::vector or std::string by value is cheap now.

    std::cout << "\nend of main -- destructors run in reverse order:\n";
    return 0;
}

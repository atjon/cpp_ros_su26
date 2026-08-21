#include <iostream>
#include <string>

// 15.4 - Introduction to destructors
// 15.5 - Class templates with member functions

// ------------------------ 15.4 DESTRUCTORS -----------------------------------
//
// A destructor runs automatically when an object is DESTROYED. It's the
// counterpart to the constructor.
//
// Rules:
//   - named ~ClassName
//   - NO parameters, NO return type
//   - exactly ONE per class -- it can't be overloaded
//
// Its job is CLEANUP: release whatever the object acquired. If your class
// doesn't own a resource, you usually don't need to write one at all.

class Connection {
private:
    std::string m_name { };
    bool m_open { false };

public:
    explicit Connection(const std::string& name) : m_name { name }, m_open { true } {
        std::cout << "    [OPEN  " << m_name << "]\n";
    }

    ~Connection() {
        // this runs automatically -- nobody has to remember to call it
        if (m_open)
            std::cout << "    [CLOSE " << m_name << "]\n";
        m_open = false;
    }

    void use() const { std::cout << "    using " << m_name << "\n"; }
};

// ------------------------ RAII -----------------------------------------------
//
// The constructor/destructor pair is the foundation of RAII:
// Resource Acquisition Is Initialization.
//
//   acquire the resource in the CONSTRUCTOR
//   release it in the DESTRUCTOR
//
// Then the resource is tied to the object's lifetime, and the compiler
// guarantees cleanup -- on normal exit, on early return, and even when an
// exception unwinds the stack. You cannot forget to clean up, because you
// aren't the one doing it.
//
// This is why C++ doesn't need try/finally, and it's the mechanism behind
// std::unique_ptr, std::vector, std::string, and every RAII type in chapter 22.

// ------------------------ 15.5 CLASS TEMPLATES WITH MEMBER FUNCTIONS ---------
//
// 13.13 introduced class templates with data members. Once member functions
// get involved, the out-of-class definition syntax is the part that trips
// everyone up.

template <typename T>
class Buffer {
private:
    T m_items[8] { };
    int m_count { };

public:
    // defined INSIDE -- simple, implicitly inline, no extra syntax needed
    int count() const { return m_count; }
    bool empty() const { return m_count == 0; }

    // declared here, defined outside the class below
    bool push(const T& item);
    T last() const;
    void print() const;

    // a destructor in a class template works exactly like a normal one
    ~Buffer() {
        std::cout << "    [Buffer destroyed, held " << m_count << " items]\n";
    }
};

// OUT-OF-CLASS DEFINITIONS FOR A CLASS TEMPLATE.
// Three things every one of them needs:
//   1. the template<> header repeated
//   2. the class qualified as Buffer<T>, not just Buffer
//   3. const repeated if the declaration had it

template <typename T>
bool Buffer<T>::push(const T& item) {
    if (m_count >= 8)
        return false;
    m_items[m_count] = item;
    ++m_count;
    return true;
}

template <typename T>
T Buffer<T>::last() const {
    return (m_count > 0) ? m_items[m_count - 1] : T { };
    //                                            ^ T{} value-initializes
    //                                              whatever T happens to be
}

template <typename T>
void Buffer<T>::print() const {
    std::cout << "  buffer(" << m_count << "): ";
    for (int i { 0 }; i < m_count; ++i)
        std::cout << m_items[i] << " ";
    std::cout << "\n";
}

// REMINDER from 11.10: all of this must live in a HEADER if other .cpp files
// are going to use Buffer<T>, because the compiler needs the full definition
// to instantiate it.

int main() {

    // ---------------- 15.4 destructors ---------------------------------------
    std::cout << "destructors and scope:\n";
    {
        Connection a { "serial0" };
        a.use();

        {
            Connection b { "serial1" };
            b.use();
        } // b is destroyed HERE, at the end of its scope

        std::cout << "    (inner scope ended)\n";
        a.use();
    } // a is destroyed here

    std::cout << "  (outer scope ended)\n";

    // DESTRUCTION ORDER is the REVERSE of construction order.
    std::cout << "\nreverse destruction order:\n";
    {
        Connection first { "first" };
        Connection second { "second" };
        Connection third { "third" };
    } // destroyed third, second, first

    // an early return doesn't leak -- the destructor still runs
    std::cout << "\nRAII survives an early return:\n";
    auto risky = [](bool bail) {
        Connection c { "risky" };
        if (bail) {
            std::cout << "    bailing out early\n";
            return;    // ~Connection STILL runs
        }
        c.use();
    };
    risky(true);
    risky(false);

    // ---------------- 15.5 class templates -----------------------------------
    std::cout << "\nclass template with member functions:\n";
    {
        Buffer<int> ints;
        ints.push(10);
        ints.push(20);
        ints.push(30);
        ints.print();
        std::cout << "  count=" << ints.count()
                  << " last=" << ints.last()
                  << " empty=" << ints.empty() << "\n";

        Buffer<std::string> names;   // a completely separate type
        names.push("lidar");
        names.push("imu");
        names.print();
        std::cout << "  last=" << names.last() << "\n";

        Buffer<double> unused;       // empty -- watch T{} handle last()
        std::cout << "  empty buffer last() = " << unused.last()
                  << " (value-initialized T)\n";
    } // all three Buffers destroyed here, in reverse order

    std::cout << "\ndone.\n";
    return 0;
}

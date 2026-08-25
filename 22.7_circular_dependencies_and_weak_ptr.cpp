#include <iostream>
#include <memory> // for shared_ptr, weak_ptr, make_shared
#include <string>
#include <utility>

// 22.7 - Circular dependency issues with std::shared_ptr, and std::weak_ptr

// ------------------------ THE PROBLEM ----------------------------------------
//
// shared_ptr frees its object when the reference count reaches zero.
// But if two objects hold shared_ptrs to EACH OTHER, neither count can ever
// reach zero -- each is kept alive by the other. Nothing is freed. That's a
// CIRCULAR REFERENCE, and it's a genuine memory leak even with smart pointers.

class BrokenNode {
public:
    std::string m_name { };
    std::shared_ptr<BrokenNode> m_partner { };   // <-- the problem

    explicit BrokenNode(std::string name) : m_name { std::move(name) } {
        std::cout << "    [" << m_name << " constructed]\n";
    }

    ~BrokenNode() {
        std::cout << "    [" << m_name << " destroyed]\n";
    }
};

// ------------------------ THE FIX: std::weak_ptr -----------------------------
//
// A weak_ptr OBSERVES a shared_ptr's object WITHOUT owning it:
//   - it does NOT increment the reference count
//   - so it never keeps the object alive
//   - and therefore it can't break a cycle's exit
//
// The catch: because it doesn't own anything, the object may already be gone.
// So you cannot use a weak_ptr directly. You must call lock(), which returns a
// shared_ptr -- non-null if the object is still alive, null if it isn't.
// That lock() is what makes it SAFE where a raw pointer would dangle.

class Node {
public:
    std::string m_name { };
    std::weak_ptr<Node> m_partner { };   // <-- weak, so no cycle

    explicit Node(std::string name) : m_name { std::move(name) } {
        std::cout << "    [" << m_name << " constructed]\n";
    }

    ~Node() {
        std::cout << "    [" << m_name << " destroyed]\n";
    }

    void greetPartner() const {
        // lock() promotes the weak_ptr to a shared_ptr for safe use.
        // It returns null if the object is already destroyed.
        if (auto partner { m_partner.lock() })
            std::cout << "    " << m_name << " -> partner is "
                      << partner->m_name << "\n";
        else
            std::cout << "    " << m_name << " -> partner is gone\n";
    }
};

// The classic real-world shape: a parent owns its children (shared_ptr, going
// down), children refer back to the parent (weak_ptr, going up). Ownership
// flows in ONE direction; back-references are always weak.
class Child;

class Parent {
public:
    std::string m_name { };
    std::shared_ptr<Child> m_child { };   // OWNS the child

    explicit Parent(std::string name) : m_name { std::move(name) } { }
    ~Parent() { std::cout << "    [Parent " << m_name << " destroyed]\n"; }
};

class Child {
public:
    std::string m_name { };
    std::weak_ptr<Parent> m_parent { };   // REFERS BACK, without owning

    explicit Child(std::string name) : m_name { std::move(name) } { }
    ~Child() { std::cout << "    [Child " << m_name << " destroyed]\n"; }
};

int main() {

    // ---------------- the leak -----------------------------------------------
    std::cout << "circular reference with shared_ptr (LEAKS):\n";
    {
        auto a { std::make_shared<BrokenNode>("BrokenA") };
        auto b { std::make_shared<BrokenNode>("BrokenB") };

        std::cout << "    before linking: a.use_count=" << a.use_count()
                  << " b.use_count=" << b.use_count() << "\n";

        a->m_partner = b;   // b's count -> 2
        b->m_partner = a;   // a's count -> 2

        std::cout << "    after linking:  a.use_count=" << a.use_count()
                  << " b.use_count=" << b.use_count() << "\n";

        std::cout << "  (leaving scope -- watch for destructor messages)\n";
    }
    // The local shared_ptrs are destroyed, dropping each count from 2 to 1.
    // Neither reaches 0, so NEITHER DESTRUCTOR RUNS. Notice the total absence
    // of "[BrokenA destroyed]" above. That memory is leaked for the rest of
    // the program's life, and there is no way to reclaim it.
    std::cout << "  ^ no destructors ran. Both objects LEAKED.\n";

    // ---------------- the fix ------------------------------------------------
    std::cout << "\nsame structure with weak_ptr (no leak):\n";
    {
        auto a { std::make_shared<Node>("NodeA") };
        auto b { std::make_shared<Node>("NodeB") };

        a->m_partner = b;   // weak -- does NOT bump b's count
        b->m_partner = a;

        std::cout << "    use_counts stay at: a=" << a.use_count()
                  << " b=" << b.use_count() << "\n";

        a->greetPartner();
        b->greetPartner();

        std::cout << "  (leaving scope)\n";
    }
    std::cout << "  ^ both destructors ran. No leak.\n";

    // ---------------- weak_ptr detecting a dead object ------------------------
    std::cout << "\nweak_ptr safely detects a destroyed object:\n";

    std::weak_ptr<Node> observer;
    {
        auto temp { std::make_shared<Node>("Temporary") };
        observer = temp;

        std::cout << "    expired()? " << observer.expired() << "\n";
        if (auto locked { observer.lock() })
            std::cout << "    locked successfully: " << locked->m_name << "\n";

        std::cout << "  (temp going out of scope)\n";
    }

    std::cout << "    expired()? " << observer.expired() << " <- now true\n";
    if (auto locked { observer.lock() })
        std::cout << "    locked: " << locked->m_name << "\n";
    else
        std::cout << "    lock() returned null -- the object is gone, and we\n"
                     "    found out SAFELY instead of dereferencing a dangling pointer\n";

    // ---------------- the parent/child pattern --------------------------------
    std::cout << "\nparent owns child, child refers back weakly:\n";
    {
        auto parent { std::make_shared<Parent>("P1") };
        auto child { std::make_shared<Child>("C1") };

        parent->m_child = child;    // strong, downward: the parent OWNS
        child->m_parent = parent;   // weak, upward: no cycle

        std::cout << "    parent use_count = " << parent.use_count()
                  << " (still 1 -- the child's weak ref doesn't count)\n";

        if (auto p { child->m_parent.lock() })
            std::cout << "    child can still reach parent: " << p->m_name << "\n";

        std::cout << "  (leaving scope)\n";
    }
    std::cout << "  ^ both freed correctly.\n";

    // RULE OF THUMB: when two objects reference each other, ask which one OWNS
    // the other. The owner holds a shared_ptr; the other direction is a
    // weak_ptr. If NEITHER owns the other, both should probably be weak (or
    // raw non-owning pointers) with ownership held somewhere else entirely.

    return 0;
}

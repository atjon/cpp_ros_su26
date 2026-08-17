#include <iostream>
#include <string>

// 13.10 - Passing and returning structs
// 13.11 - Struct miscellany
// 13.12 - Member selection with pointers and references

// ------------------------ 13.11 STRUCTS CAN NEST -----------------------------

struct Vec2 {
    double x { };
    double y { };
};

// a struct can contain other program-defined types as members
struct Pose {
    Vec2 position { };      // nested struct
    double heading { };
};

struct Robot {
    std::string name { "unnamed" };
    Pose pose { };          // nested two levels deep
    int battery { 100 };
};

// ------------------------ 13.10 PASSING STRUCTS ------------------------------
//
// Structs are usually expensive to copy (they're as big as their members
// combined), so the rule from 12.6 applies hard here:
//   PASS BY CONST REFERENCE unless you have a reason not to.

void printPose(const Pose& pose) {          // const& -- no copy
    std::cout << "  (" << pose.position.x << ", " << pose.position.y
              << ") heading " << pose.heading << "\n";
}

void nudgeByValue(Pose pose) {              // COPY -- caller sees no change
    pose.position.x += 100.0;
}

void nudgeByRef(Pose& pose) {               // reference -- caller's object changes
    pose.position.x += 100.0;
}

// RETURNING a struct by value is fine and idiomatic, even though it looks
// expensive. Copy elision (14.15) usually removes the copy entirely.
Pose makePose(double x, double y, double heading) {
    return Pose { Vec2 { x, y }, heading };
}

// you can even return an anonymous temporary -- the return type is known,
// so the braces are enough
Vec2 origin() {
    return { 0.0, 0.0 };
}

// ------------------------ 13.12 MEMBER SELECTION -----------------------------
//
// Selecting a member depends on what you have in hand:
//   object      ->  use .
//   reference   ->  use .     (a reference IS the object, so nothing changes)
//   pointer     ->  use ->    (or the ugly (*ptr).member )

void viaReference(const Robot& bot) {
    std::cout << "  via reference: " << bot.name       // just a dot
              << " battery " << bot.battery << "\n";
}

void viaPointer(const Robot* bot) {
    if (!bot) return;
    std::cout << "  via pointer:   " << bot->name      // arrow
              << " battery " << bot->battery << "\n";

    // -> is pure syntax sugar. These two lines are identical:
    std::cout << "  same thing:    " << (*bot).name << "\n";
    //                                  ^ the parens are REQUIRED, because .
    //                                    binds tighter than *. Without them,
    //                                    *bot.name would try to deref bot.name.
}

int main() {

    // ---------------- 13.11 nested structs ------------------------------------
    // nested braces mirror the nested structure
    Robot bot { "scout", { { 1.0, 2.0 }, 90.0 }, 87 };
    //           name      x    y      heading   battery

    // designated initializers make deep nesting far more readable
    Robot bot2 {
        .name = "hauler",
        .pose = { .position = { .x = 5.0, .y = 5.0 }, .heading = 270.0 },
        .battery = 42,
    };

    // chained member selection walks down the nesting
    std::cout << bot.name << " at x=" << bot.pose.position.x
              << " y=" << bot.pose.position.y << "\n";
    std::cout << bot2.name << " at x=" << bot2.pose.position.x
              << " y=" << bot2.pose.position.y << "\n";

    // ---------------- 13.10 passing and returning -----------------------------
    std::cout << "\npassing:\n";
    Pose pose { { 1.0, 1.0 }, 0.0 };

    printPose(pose);

    nudgeByValue(pose);
    std::cout << "  after nudgeByValue: x = " << pose.position.x
              << " (unchanged -- it got a copy)\n";

    nudgeByRef(pose);
    std::cout << "  after nudgeByRef:   x = " << pose.position.x
              << " (changed -- it got a reference)\n";

    std::cout << "\nreturning:\n";
    Pose made { makePose(3.0, 4.0, 45.0) };
    printPose(made);

    Vec2 o { origin() };
    std::cout << "  origin() = (" << o.x << ", " << o.y << ")\n";

    // ---------------- 13.12 selection through pointers ------------------------
    std::cout << "\nmember selection:\n";
    viaReference(bot);
    viaPointer(&bot);

    Robot* ptr { &bot2 };
    ptr->battery = 15;                 // write through the arrow
    std::cout << "  set via ptr->: " << bot2.battery << "\n";

    // mixing . and -> when the members are themselves structs:
    // ptr is a pointer, so the FIRST step uses ->, then the rest are plain dots
    std::cout << "  ptr->pose.position.x = " << ptr->pose.position.x << "\n";

    // ---------------- 13.11 struct size and padding ---------------------------
    //
    // sizeof(struct) is usually >= the sum of its members, because the compiler
    // inserts PADDING so each member lands on a favorable memory boundary.
    struct Padded {
        char  flag;   // 1 byte ... then 3 bytes of padding
        int   count;  // 4 bytes
        char  tag;    // 1 byte ... then 3 more bytes of padding
    };
    std::cout << "\nsizeof(char)+sizeof(int)+sizeof(char) = "
              << sizeof(char) + sizeof(int) + sizeof(char) << "\n";
    std::cout << "but sizeof(Padded) = " << sizeof(Padded) << " (padding!)\n";
    std::cout << "reordering members largest-first can shrink a struct.\n";

    return 0;
}

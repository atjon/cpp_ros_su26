#include <iostream>

// global variables are any vars defined outside a function body.
int g_ {}; // when nothing is intialized, it's 0 by default.

// constant global variables must be intialized when defined
const int a = 2;
constexpr int b = 3;


namespace Goofball {
    int h_ {}; // if global variables created, it's actually preferred
               // to be defined in a global namespace instead
}

int main() {
    //local variables are defined within functions

    int a = 3; 
    int b = 4;
    int x {5}; // local variable within this block scope (area between the {})
    int y {7}; // y should be defined in this scope b/c its used here
    y += 1;        

    std::cout << "a from the local scope = " << a <<
    "\n::a, or a from the global scope = " << ::a << "\n";
    

    g_ = 2;           // global variables can be used anywhere in file
    Goofball::h_ = 2; // since the namespace is defined in the global namespace
                       // we can still use h_x
    
    
    //to drive the concept further, we can enter another block here
    {
        int x {6}; // this x is in a separate scope from the x in the main scope.
               // x's lifetime ends after the last bracket on the next line

           
        g_ += 2; // literally anywhere
    }

    y-=1;
    g_ -= 2;
    return 0; // RECALL: x's lifetime ends here
}
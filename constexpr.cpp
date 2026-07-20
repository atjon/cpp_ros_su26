#include <climits>
#include <iostream>

int main() {

    // the point of constexpr and constexpr ifs is giving programmers the ability
    // to move the computation done during runtime to compile time.

    const int t = 20; // clearly a constant variable & will NEVER change
                      // ideally, if compiler could, it should want to optimize
                      // this piece of code during compile time, instead of it 
                      // taking valuable runtime 
                      // just const is the same as a macro (#DEFINE)

    constexpr int x = 5; // constexpr answers this need. A constexpr variable 
                         // must always be defined w/ a constant expression
                         // useful when combined with template meta programming

    if constexpr (x == 5) { // when the conditional is a constant expression, 
                            // its best practice to use constexpr if rather than not
                            // it compeletely gets rid of the entire block of code 
                            // that won't be executed at runtime 
        std::cout << "x is normal\n";
    } else {
        std::cout << "x isn't normal\n";
    }

    
    return 0;
}
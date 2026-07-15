#include <iostream>
//ANYTHING with a # is a "preprocessor directive,
//which also ends w/ a newline , not a ;"
//these lines of code with # have separate syntax from C++

#define PRINTING 0 
// when defining PRINTING with no argument after it, 
// anywhere in the code where PRINTING is being used, 
// it's replaced with nothing

//HOWEVER; note that when the macro is used in a 
//preprocessor directive (the ifdef below), it doesn't
// replace it with nothing.

int main() {
#ifdef PRINTING 
    //ifdef is another preprocessor directive 
    //ifdef --> IF this macro is DEFined, execute the code below
    std::cout << "made it past the print ifdef\n";
#endif

#ifdef NOPRINTING
    //since NOPRINTING is not defined, 
    //this shouldn't run
    std::cout << "made it through the NOPRINTING ifdef";
#endif

    return 0;
}
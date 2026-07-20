#include <iostream>
#include <string>
#include "namespaces.h" // for Calculator::add()

using namespace std; //NEVER use this; if we defined a function called cout,
                     //we would get a compile error.

// -------------------------- CH.2 INTRO to NAMESPACES -----------------------------------------


//Macro definitions, basically variable for the rest of the file. 
//Generally defined in the global namespace
//ONLY LASTS until the end of this file, doesn't work if the #included in another file
#define USERNAME "atjon"

//All statements not in a class, function, or a namespace
//are part of a global namespace

void foo();
int x;
int y { 5 };
// x = 5; //executable statements not allowed in namespaces (only in main or other functions)

// -------------------------- END OF CH.2 INTRO to NAMESPACES -----------------------------------------

// ------------------------- CH. 7 CREATING YOUR OWN NAMESPACES ---------------------------------------

namespace Foo {
    //namespaces can be created w/ this namespace keyword
    //they should be thought about as program-defined namespaces
    
    //This mathOperation() belongs to the namespace Foo
    int mathOperation() {
        return 1+1;
    }
}

namespace Goo {
    //This mathOperation() belongs to the namespace Goo
    int mathOperation() {
        return 2+2;
    }
}

namespace Calculator {
    //from the header file, we need to define the actual function we have,
    //notice we are defining it in the same namespace as the header file
    int add(int x, int y) {
        return x + y;
    }
 
    //you can declare namespaces in other files than the header and corresponding
    //cpp file. It's all valid. If across multiple files, then you must #include them at the top of the program you use the namespace.
}

namespace Calculator {
    // you can also add to a namespace within the same file
    
    // you can also nest namespaces within one another
    // in this case, Screen is a namespace inside the Calculator namespace
    namespace Screen {
        std::string output() {
            return "Stream output of characters on screen here..";
        }
    }

}

// You can also declare nested namespaces like the following:
namespace Calculator::Screen {
    // Additions to the Screen namespace here
}

int main() {
    //The :: is a scope resolution operator
    //This is a good example, where we have two different functions that have the same name,
    //peform a similar operation, and we can use namespaces to separate them. This can be done across files,
    //where the header file has a namespace defined and has the method header defined within that namespace.
    //Then in the cpp file, the linker can find the function definition.
    
    std::cout << USERNAME << "\n";
    cout << "std:: not needed to print when using namespace std\n";

    std::cout << "\n\nFoo:mathOperation() = " << Foo::mathOperation() << "\n";
    std::cout << "Goo:mathOperation() = " << Goo::mathOperation() << "\n";
    std::cout << "\nCalculator::add(2,3) = " << Calculator::add(2, 3) << "\n";
    std::cout << "\nCalculator::Screen::output() = " << Calculator::Screen::output() << "\n";

    namespace Active = Calculator::Screen; // namespace alias
                                           // you can make variables for long strings of nested namespaces
                                           // just w/ the namespace keyword and the name of the alias
    std::cout << "namespace Active can be used like: Active::output()" << Active::output() << "\n";
    return 0;
}

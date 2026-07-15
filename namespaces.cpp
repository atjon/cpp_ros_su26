#include <iostream>

using namespace std; //NEVER use this; if we defined a function called cout,
                     //we would get a compile error.


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

int main() {
    //The :: is a scope resolution operator
    
    std::cout << USERNAME << "\n";
    cout << "std:: not needed to print when using namespace std\n";
    return 0;
}

void goo();
#include <iostream>
#include <string>

//This file is used in company with linkageCopy.cpp
//to simulate how internal linkage works.

//main idea: adding the static keyword converts default,
//externally linked things (functions/variables) into 
//internally linked things (only accessible within that file)

//PURPOSE OF INTERNAL LINKAGE: 
// - global vars we don't want to mess w/
// - helper function we don't want anywhere else
// - learncpp recommends internal linkage when there's
//   an explicit reason to disallow access to other files.

static int g_ {}; //non constant global variables
                  //have external linkage by default,
                  //but are given internal linkage 
                  //w/ the static keyword.

const int h_ {1}; //const globals have internal linkage by default
constexpr int k_ {2}; //internal linkage by default.


extern const int m_ {}; //const global vars can be made external
                        //by adding the extern keyword

static int add(int x, int y) {
    std::cout << "This add function is unique to this file.\n";
    return x + y;
}

std::string sayHi(); //forward declaration sources sayHi() 
              //from linkageCopy.cpp's definition.

inline double pi() {
    //note that inline.cpp also has this function defintion
    //across multiple translation units, inline functions 
    //must share the exact same function definition.
    return 3.14159;
}

int main() {
    std::cout << g_ << " " << h_ << " " << k_ << "\n";
    std::cout << add(3,5) << "\n";
    std::cout << "sayHi(), which is forward declared in this file, " << 
    "utilizes external linkage to retrieve the function declaration in linkageCopy.cpp." << 
    " sayHi() says: " << sayHi();
    std::cout << "pi(), which is defined with inline keyword " <<
    "in both linkage.cpp and inline.cpp, is allowed. pi() = " << pi() << "\n";

}
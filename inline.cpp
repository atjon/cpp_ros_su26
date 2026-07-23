#include <iostream>

//inline --> keyword
// used to tell compiler to directly replace the operations it performed
// to happen within the same line to help compile time
// now, compiler can do automatically, so it's not needed for that anymore
// (takes care of it for you whether or not you put inline keyword)

//MODERN USE OF INLINE: 
// when used before a function, it's telling compiler "multiple definitions are allowed
// of this function"

// inline functions are allowed to be defined within multiple 
// translation units (w/o violating ODR)
inline double pi() {
    //note that linkage.cpp also has this same function
    //you cannot have this defined again in this translation unit.
    return 3.14159;
}

// NOTE: when linking files together, you can only have
// one file that has a main function.

// int main() {
    
//     return 0;
// }

namespace constants {
    constexpr double pi {3.14159}; // one way to have globally defined constants
                                   // is to have this defined in a namespace. 
                                   // constexpr helps w/ saving runtime 
    inline constexpr double myGravity {9.6}; // inline helps declare this constexpr as 
                                             // externally linkable
}
#include <iostream>

// casting in CPP is not the same as it is in java.
// not for everything can you just write the (type)
// next to what you want to cast

int main(){
    
    double x = 5.5; 

    //it's good practice to explicitly downcast or narrow convert like this.
    //compiler checks it, and doesn't have too much power so it doesn't let 
    //dangerous conversions go through
    int y = static_cast<int>(x); // almost for everything, static_cast will work
                                 // static_cast won't work with pointer casting,
                                 // and for const something? I dodn't understand enough for the const
                                 // thingy.

    std::cout << "x= " << x << "\ny= " << y << "\n";
    // note that standard conversions like inputting a integer to a parameter
    // that expects a double still gets converted w/o anything extra from the user

    // int z {7.6}; // note that brace initialization will prevent this default conversion

    //note that float is narrower than double
    float z { static_cast<float>(x)}; // this is not flagged by compiler as narrowing. DANGEROUS!
                                      // x is being converted from double --> float

    // C-style casts (same casts as Java and Python)
    int a = int(5.5); // in CPP, avoid since it may do more than what it looks like. Bad practice!

    return 0;


}
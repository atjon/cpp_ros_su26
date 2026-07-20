#include <iostream>

//Operators have a specific order in which they are run
//This is the same way that Math has a specific order of operations
//Notably, the namespace operator (::) has the highest precedence

int main() {

    //make it clear with parentheses for non-trivial expressions. Ex:
    int a, b, c, d;
    a = 1;
    b = 2;
    c = 3;
    d = 4;
    int x = (a + b) * (c + d);
    std::cout << "Instead of a + b * c + d, we can group " << 
    "the expression as: (a + b) * (c + d)\n" << "x=" << x << "\n";


    //It's best practice to avoid the comma operator, except within for loops
    std::cout << "a was: " << a << "\nLet's increment with some comma operators.\n" << 
    (a++, b++) << "\nNow a= " << a << "\nNow b= " 
    << b << "\n"; // increment a and b, then 
                  // returns the result of the 
                  // right operand

    int i, j;
    for (i=0; i<5; i++, j++) { //comma operator is okay for multiple 
                               //increment/decrement operations within one for loop.
        for (j=0; j<5; j++) {
            d++;
        }
    }
                  
    return 0;
}
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


    return 0;
}
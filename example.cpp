#include <iostream>

int main() {
    //The :: is used b/c of namespaces. To make sure the namespaces.
    //To make sure that the cout we are talking about is from the 
    //standard (std) library, not another third party library.

    //The << is an operator used to pass the string into the object
    //as as parameter.
    std::cout << "TR is the greatest thing to ever exist";
    return 0;
}

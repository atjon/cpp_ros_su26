#include <iostream>
//#include is the import of C++. It brings in the iostream library


int main() {
    //The :: is used b/c of namespaces. To make sure the namespaces.
    //To make sure that the cout we are talking about is from the 
    //iostream  library, not another third party library.

    //The << is an operator used to pass the string into the object
    //as as parameter.
    std::cout << "TR is the greatest thing to ever exist\n";
    std::cout << "This is a test to see how cmake works\n";
    return 0;
}

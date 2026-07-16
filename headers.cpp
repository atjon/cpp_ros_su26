#include <iostream>
#include "headers.h"
#include "fake.h"

int main() {
    std::cout << "add(3,4) = " << add(3,4) << "\n";
    std::cout << "This is a simple placeholder: " << placeHolder() << "\n";

    return 0;
}

int add(int x, int y) {
    //since headers.h forward declares add, 
    //we can define add() after main
    return x + y;
}

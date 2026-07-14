#include <iostream>

void doPrint() {
    //example function
    std::cout << "In doPrint()\n";
}
char* doB() {
    return "In doB()\n";
}

void doA() {
    std::cout << "Starting doA()\n";
    std::cout << doB();
    std::cout << "Ending doA()\n";
}

int returnSix() {
    return 6;
}

int returnSeven() {
    return 7;
}

int add(int a, int b) {
    return a + b;
}

int main() {
    std::cout << "Starting main()\n";
    doPrint(); // Interrupts main() and calls doPrint()
    std::cout << "Function calls other functions\n";
    doA();
    std::cout << returnSix() << " " << returnSeven() << "!\n";
    std::cout << "add(4, 5) = " << add(4,5) << " " 
    "add(5, 6)" << add(5, 6) << "\n";
    std::cout << "Ending main()\n"; //This is after the doPrint() statement

    return 0;
}

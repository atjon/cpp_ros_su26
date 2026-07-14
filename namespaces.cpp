#include <iostream>

//All statements not in a class, function, or a namespace
//are part of a global namespace

void foo();
int x;
int y { 5 };
// x = 5; //executable statements not allowed in namespaces (only in main or other functions)

int main() {
    //The :: is a scope resolution operator
    

    return 0;
}

void goo();
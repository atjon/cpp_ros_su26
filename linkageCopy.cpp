#include <iostream>

//This file is used in company with linkage.cpp
//to simulate how internal linkage works.




static int g_ {5}; //This separate internal g_ is only accessible within main.cpp
                   //This should be intuitive. Same var, different files, so they are 
                   //accessed within their respective files 

void sayHi() { // this function has internal linkage, and is seen by other files
    std::cout << "Hi!\n";
}

int main() {
    std::cout << g_ << "\n"; // uses linkageCopy.cpp's g_, printing 5;
}
#include <iostream>
//size of types can be found @
//https://www.learncpp.com/cpp-tutorial/object-sizes-and-the-sizeof-operator/

int main() {
    
    short x = 1;                     
    int y = 1000000;                   
    long z = 12130192839123;            
    long long a = 120938109238809123;   
    bool isBanana = false;
    char* arr = "Meow";
    char charA = 'A';


    std::cout << "Below are a bunch of variables of different types and their outputs:\n\n";
    std::cout << "short x="<< x << "\n" << "int y=" << y << "\n" << "long z=" << 
    z << "\n" << "long long a=" << a << "\n" << "bool isBanana=" << isBanana << "\n" 
    << "char* arr=" << arr << "\n" << "char charA=" << charA << "\n";

    std::cout << "\n\nsizeof(short)="<< sizeof(int) << "\n" << "sizeof(int)" 
    << sizeof(int) << "\n" << "sizeof(long)=" << sizeof(long) << "\n" << 
    "sizeof(long long)=" << sizeof(long long) << "\n" << "sizeof(bool)=" <<
    sizeof(bool) << "\n" << "sizeof(char*)=" << sizeof(char*) << "\n" 
    << "sizeof(char)=" << sizeof(char) << "\n";


    // assuming 4 byte integers
    int g { 2'147'483'647 }; // the maximum value of a 4-byte signed integer
    std::cout << "\n\n" << g << '\n';

    g = g + 1; // integer overflow, undefined behavior (cse29 signed vs unsigned ints)
               // when bits are all maxed out, adding 1 just carrys the 1 until all 
               // digits are 0 and the leftover 1 doesn't fit in the block reserved
               // then reset to the last number
    std::cout << g << '\n';

    return 0;
}
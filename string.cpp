#include <iostream>
#include <string> //allows the use of std::string
                  //easy ways to work w/ strings

int main() {

    std::string title {"Banana"};
    std::string author = "Meow";
    std::string empty = "";
    int titleLength = title.length();

    std::cout << "Below are a bunch of string variables and their outputs:\n\n";
    std::cout << "std::string title=" << title << "\n" << "std::string author="
    << author << "\n" << "std::string empty=" << empty << "\n"
    << "int titleLength=" << titleLength << "\n";

    return 0;
}
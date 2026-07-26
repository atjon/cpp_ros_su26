#include <iostream>
#include <random> // allows us to utilize classes in std
                  // namespace to create random numbers
                 
#include <chrono> //allows us to use the IRL time

int main() {

    // Mersenne Twister is the type that 
    // is the most random in C++

    std::mt19937 x{};
    std::uniform_int_distribution die{1,6}; //the object takes the min and max values


    // Only problem with these two rngs is that they are the same after running twice
    for (int i=1; i<=5; i++) {
        std::cout << "RNG #" << i << ": " << 
        x() << // x() is shorthand for x.operator()
        "\t";

        std::cout << "RNG bet 1-6: " << die(x) << "\t"; // takes a random number as the seed?
        
    }
    std::cout << "\n";



    //one way we can avoid the rngs returning the same # is by generating a random seed using 
    //the time and clock on the pc

    std::mt19937 y{static_cast<std::mt19937::result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count())};

    std::cout << "Random seed based of time: " << y << "\n\n";
    for (int i=1; i<=5; i++) {

        std::cout << "RNG# " << i << ":" << y() << "\t"; // takes a random number as the seed?

    } 
    std::cout << "\n";
    return 0;
}
#include <iostream>
#include "./emoji.h"

int main() {
    std::cout << emojicpp::emojize("Emoji :smile: for c++ :+1:") << std::endl;
    return 0;
}

// g++ example.cpp -std=c++11 -o example &&  ./example

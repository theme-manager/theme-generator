#include <sstream>
#include <iostream>

int convertCharToInt(char* c) {
    std::stringstream ss(c);
    int i;
    if( ss >> i ) {} else {
        std::cout << "Character " << c << "is not convertible to int" << std::endl;
        exit(1);
    }
    return i;
}
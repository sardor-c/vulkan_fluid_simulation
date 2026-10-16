#include "vec2.hpp"
#include <iostream>

int main() {
    std::cout << "dot({1, 0}, {0, 1}): " << dot({ 1,0 }, { 0,1 }) << std::endl;
    std::cout << "dot({1, 0}, {1, 0}): " << dot({ 1,0 }, { 1,0 }) << std::endl;
    return 0;
}
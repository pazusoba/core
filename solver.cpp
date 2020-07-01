#include <iostream>

/**
 * seal is a tape over a row and orbs cannot move,
 * disabled means it will not get erased even if connected.
 */
enum orbs { empty, fire, water, wood, light, dark, recovery, obstacle, bomb, poison, poison_plus, seal, disabled };

int main() {
    std::cout << "Hello World\n";
    return 0;
}

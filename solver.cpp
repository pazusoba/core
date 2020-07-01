#include <iostream>
#include <fstream>
#include <string>

/**
 * seal is a tape over a row and orbs cannot move,
 * disabled means it will not get erased even if connected.
 */
enum orbs { empty, fire, water, wood, light, dark, recovery, obstacle, bomb, poison, poison_plus, seal, disabled };

// Maybe a good idea to consider the weight

int main() {
    using namespace std;
    ifstream patternFile("pattern.txt");
    string lines;
    while (getline(patternFile, lines)) {
        // Ignore lines that start with `//`
        if (lines.find("//") == 0) continue;
        cout << lines << endl;
    }
    patternFile.close();
    return 0;
}

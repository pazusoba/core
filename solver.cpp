#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

/**
 * All orbs in the game.
 * 
 * seal is a tape over a row and orbs cannot move,
 * disabled means it will not get erased even if connected.
 * 
 * Also, consider weight because it can be quite important
 */
enum orbs { empty, fire, water, wood, light, dark, recovery, obstacle, bomb, poison, poison_plus, seal, disabled };

/**
 * Read orbs from pattern.t 
 */
void readPattern();

int main() {
    readPattern();
    return 0;
}

void readPattern() {
    using namespace std;
    string lines;
    int row = 0, column = 0;
    // This saves all orbs, it supports numbers more than 10
    vector<orbs> patternOrbs;

    ifstream patternFile("pattern.txt");
    while (getline(patternFile, lines)) {
        // Ignore lines that start with `//`
        if (lines.find("//") == 0) continue;

        // Keep reading until error, it will get rid of spaces automatically
        stringstream ss(lines);
        while (ss.good()) {
            // Only add one to row if we are in the first column, 
            // the size is fixed so there won't be a row with a different number of orbs
            if (column == 0) row++;
            // Read it out as a number
            int a = 0; ss >> a;
            // Convert int into orbs
            patternOrbs.push_back(orbs(a));
        }

        column++;
    }
    patternFile.close();

    // Print everything out nicely
    cout << column << " x " << row << endl;
    int counter = 0;
    for (auto orb : patternOrbs) {
        counter++;
        cout << orb << " ";
        if (counter % row == 0) cout << endl;
    }
}

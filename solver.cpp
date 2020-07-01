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
enum orbs { empty, fire, water, wood, light, dark, recovery, jammer, bomb, poison, poison_plus, seal, disabled };
const int ORB_COUNT = 13;
// For displaying the orb name
const std::string ORB_NAMES[ORB_COUNT] = { "Empty", "Fire", "Water", "Wood", "Light", "Dark", "Heal", "Jammer", "Bomb", "Poison", "Poison+", "---", "-X-"};

/**
 * Read orbs from board.txt
 */
void readBoard();

/**
 * Print out a board nicely formatted
 */
void printBoard(int row, int column, std::vector<orbs> orbList);

/**
 * Calculate max combo from a list of orbs
 */
int getMaxCombo(std::vector<orbs> orbList);

int main() {
    readBoard();
    return 0;
}

void readBoard() {
    using namespace std;
    string lines;
    int row = 0, column = 0;
    // This saves all orbs, it supports numbers more than 10
    vector<orbs> boardOrbs;

    ifstream boardFile("Board.txt");
    while (getline(boardFile, lines)) {
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
            boardOrbs.push_back(orbs(a));
        }

        column++;
    }
    boardFile.close();

    printBoard(row, column, boardOrbs);
    cout << getMaxCombo(boardOrbs) << " combo\n";
}

void printBoard(int row, int column, std::vector<orbs> orbList) {
    using namespace std;
    // Print everything out nicely
    cout << column << " x " << row << endl;
    int counter = 0;
    for (auto orb : orbList) {
        counter++;
        cout << ORB_NAMES[orb] << "\t";
        if (counter % row == 0) cout << endl;
    }

}

int getMaxCombo(std::vector<orbs> orbList) {
    using namespace std;
    int combo = 0;

    int *counter = new int[ORB_COUNT] {0};
    for (auto orb : orbList) {
        counter[orb]++;
        // TODO: this is a naive way of getting max combo. If the entire board only has one type, it only has one combo
        // You can make one combo with 3 orbs
        if (counter[orb] == 3) {
            combo++;
            // Reset current counter
            counter[orb] = 0;
        }
    }
    delete[] counter;

    return combo;
}

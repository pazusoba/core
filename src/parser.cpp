#include <pazusoba/constant.h>
#include <pazusoba/parser.h>

namespace pazusoba {
Parser::Parser(int argc, char* argv[]) {
    // parse command line arguments
    if (argc > 1) {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            showUsage();
        } else {
            this->board = argv[1];
        }
    }

    if (argc > 2) {
        int minErase = atoi(argv[2]);
        // min 3, max 5 for now
        if (minErase < 3)
            minErase = 3;
        if (minErase > 5)
            minErase = 5;
        this->minErase = minErase;
    }

    if (argc > 3) {
        this->maxSteps = atoi(argv[3]);
    }

    if (argc > 4) {
        this->beamSize = atoi(argv[4]);
    }

    if (DEBUG_PRINT)
        fmt::print("board: {}\nminErase: {}\nmaxStep: {}\nbeamSize: {}\n",
                   Board(), MinErase(), MaxSteps(), BeamSize());
}

Parser::Parser(const std::string& board,
               int minErase,
               int maxSteps,
               int beamSize) {
    this->board = board;
    this->minErase = minErase;
    this->maxSteps = maxSteps;
    this->beamSize = beamSize;
}

void Parser::parse() {
    //
}
}  // namespace pazusoba

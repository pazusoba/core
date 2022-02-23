#include "pazusoba.h"

int main(int argc, char* argv[]) {
    pazusoba::parse_args(argc, argv);
    pazusoba::explore();
    return 0;
}
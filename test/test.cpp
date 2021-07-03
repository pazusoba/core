#include <iostream>
#include <assert.h>

void testAll();

int main()
{
    using namespace std;
    cout << "=== TEST ===\n";
    testAll();
    cout << "All Good!\n";
}

void testAll()
{
    assert(true);
}

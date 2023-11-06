/******************************************************************************
* Test functionality of Texas Hold 'Em game loop.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <iostream>
#include <string>
// Project headers
#include "test_the.hpp"
// Using statements
using std::cout;
using std::endl;
using namespace constants;

/* Texas Hold 'Em Functional Tests
******************************************************************************/
void test_multi_way_ties()
{
    execute_test(test_multi_way_ties_1());
    execute_test(test_multi_way_ties_2());
}

int main()
{
    cout << endl << "Beginning tests...\n" << endl;
    test_multi_way_ties();
    cout << endl << "\nAll " << test_constants::test_count <<
        " tests completed successfully!\n" << endl;

    return 0;
}
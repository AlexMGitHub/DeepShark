/******************************************************************************
* Test functionality of the Monte Carlo simulations.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <iomanip>      // For std::fixed and std::setprecision()
#include <string>
// Project headers
#include "test_monte_carlo.hpp"
// Using statements
using std::cout;
using std::endl;
using std::vector;
using namespace constants;

/* Monte Carlo Functional Tests
******************************************************************************/
int main()
{
    cout << endl << "Beginning tests...\n" << endl;
    execute_test(test_pocket_rockets_2player());
    execute_test(test_pocket_rockets_10player());
    execute_test(test_royal_flush_10player());
    cout << endl << "\nAll tests completed successfully!\n" << endl;
    return 0;
}
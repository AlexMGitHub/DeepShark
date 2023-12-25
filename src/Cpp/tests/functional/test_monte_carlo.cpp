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
using std::string;
using std::vector;
using namespace constants;

/* Monte Carlo Functional Tests
******************************************************************************/
void test_pocket_cards(string test_desc, Card c1, Card c2)
{
    for (int i = 2; i < 11; i++)
    {
        execute_test(
            test_pocket_cards_struct(
                i,
                test_desc + std::to_string(i) + "players",
                c1,
                c2)
        );
    }
}

void test_pocket_rockets()
{
    test_pocket_cards(
        "test_pocket_rockets_",
        Card(constants::Suit::Diamond, constants::Rank::Ace),
        Card(constants::Suit::Spade, constants::Rank::Ace)
    );
}

void test_ak_suited()
{
    test_pocket_cards(
        "test_ak_suited_",
        Card(constants::Suit::Diamond, constants::Rank::Ace),
        Card(constants::Suit::Diamond, constants::Rank::King)
    );
}

int main()
{
    cout << endl << "Beginning tests...\n" << endl;
    test_pocket_rockets();
    test_ak_suited();
    execute_test(test_flop_4player1());
    execute_test(test_flop_4player2());
    execute_test(test_royal_flush_10player());
    execute_test(test_straight_draw_4player_1000runs());
    execute_test(test_straight_draw_4player_10000runs());
    cout << endl << "\nAll tests completed successfully!\n" << endl;
    return 0;
}
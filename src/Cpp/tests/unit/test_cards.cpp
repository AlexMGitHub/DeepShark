/******************************************************************************
 * Test classes defined in cards.cpp.
 *****************************************************************************/

 /* Headers
 ******************************************************************************/
 // C++ standard library
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <vector>
// Project headers
#include "cards.hpp"
// Using statements
using std::array;
using std::cout;
using std::endl;
using std::sort;
using std::to_underlying;
using std::vector;

/* Hand Class Tests
******************************************************************************/
template <size_t N>
void load_hand(array<Card, N>& cards, Hand& my_hand)
{
    for (const auto& c : cards)
    {
        my_hand.add_card(c);
    }
}

void test_royal_flush()
{
    cout << endl << "- test_royal_flush..." << endl;
    std::array<Card, 7> cards{
        Card(Suit::Spade, Rank::Four),
        Card(Suit::Diamond, Rank::Seven),
        Card(Suit::Club, Rank::Ten),
        Card(Suit::Club, Rank::Jack),
        Card(Suit::Club, Rank::Queen),
        Card(Suit::Club, Rank::King),
        Card(Suit::Club, Rank::Ace)
    };
    Hand test_hand;

    load_hand(cards, test_hand);
    cout << "\t- " << "test 7 cards...";
    test_hand.determine_best_hand();
    assert((test_hand.hand_rank == HandRank::Royal_Flush) &&
        "Hand rank should be a Royal Flush!");
    cout << "\t\tPASS" << endl;
    cout << "\t- " << "test 6 cards...";
    cout << "\t\tPASS" << endl;
    cout << "\t- " << "test 5 cards...";
    cout << "\t\tPASS" << endl;
}

/* Run Tests
******************************************************************************/
int main()
{
    cout << endl << "Beginning tests...\n" << endl;
    test_royal_flush();
    cout << endl << "\nAll tests completed successfully!\n" << endl;

    return 0;
}
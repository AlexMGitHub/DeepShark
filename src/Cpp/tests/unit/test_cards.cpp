/******************************************************************************
* Test classes defined in cards.cpp.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <span>
#include <vector>
// Project headers
#include "cards.hpp"
// Using statements
using std::array;
using std::cout;
using std::endl;
using std::sort;
using std::span;
using std::to_underlying;
using std::vector;

/* Hand Class Tests
******************************************************************************/
void load_hand(span<Card>& cards, Hand& my_hand)
{
    for (const auto& c : cards)
    {
        my_hand.add_card(c);
    }
}

bool compare_hands(
    array<Card, MAX_CARDS_IN_HAND>& h1,
    array<Card, MAX_CARDS_IN_HAND>& h2)
{
    for (size_t i = 0; i < MAX_CARDS_IN_HAND; i++)
    {
        if (h1[i] != h2[i]) { return false; }
    }
    return true;
}

void hand_assert(
    span<Card>& cards,
    array<Card, MAX_CARDS_IN_HAND>& correct_hand,
    HandRank correct_hand_rank)
{
    Hand test_hand;
    load_hand(cards, test_hand);
    test_hand.determine_best_hand();
    assert((test_hand.hand_rank == correct_hand_rank) &&
        "Incorrect hand rank!");
    assert((compare_hands(test_hand.best_hand, correct_hand)) &&
        "Best hand is not correct!");
    cout << "\t\tPASS" << endl;
}

void test_royal_flush()
{
    cout << endl << "- test_royal_flush..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Club, Rank::Four),
        Card(Suit::Club, Rank::Seven),
        Card(Suit::Club, Rank::Ten),
        Card(Suit::Club, Rank::Jack),
        Card(Suit::Club, Rank::Queen),
        Card(Suit::Club, Rank::King),
        Card(Suit::Club, Rank::Ace)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin() + 1, cards.end());
    span<Card> cards5(cards.begin() + 2, cards.end());
    array<Card, 5> correct_hand{
        Card(Suit::Club, Rank::Ten),
        Card(Suit::Club, Rank::Jack),
        Card(Suit::Club, Rank::Queen),
        Card(Suit::Club, Rank::King),
        Card(Suit::Club, Rank::Ace)
    };
    HandRank correct_hand_rank = HandRank::Royal_Flush;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand, correct_hand_rank);
}

void test_straight_flush()
{
    cout << endl << "- test_straight_flush..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Heart, Rank::Four),
        Card(Suit::Heart, Rank::Seven),
        Card(Suit::Heart, Rank::Ten),
        Card(Suit::Heart, Rank::Jack),
        Card(Suit::Heart, Rank::Queen),
        Card(Suit::Heart, Rank::King),
        Card(Suit::Heart, Rank::Nine)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin() + 1, cards.end());
    span<Card> cards5(cards.begin() + 2, cards.end());
    array<Card, 5> correct_hand{
        Card(Suit::Heart, Rank::Nine),
        Card(Suit::Heart, Rank::Ten),
        Card(Suit::Heart, Rank::Jack),
        Card(Suit::Heart, Rank::Queen),
        Card(Suit::Heart, Rank::King)
    };
    HandRank correct_hand_rank = HandRank::Straight_Flush;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand, correct_hand_rank);
}

void test_four_of_a_kind()
{
    cout << endl << "- test_four_of_a_kind..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Spade, Rank::Five),
        Card(Suit::Heart, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::Six),
        Card(Suit::Club, Rank::Six),
        Card(Suit::Heart, Rank::Six),
        Card(Suit::Diamond, Rank::Ten)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin() + 1, cards.end());
    span<Card> cards5(cards.begin() + 2, cards.end());
    array<Card, 5> correct_hand{
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::Six),
        Card(Suit::Club, Rank::Six),
        Card(Suit::Heart, Rank::Six),
        Card(Suit::Diamond, Rank::Ten)
    };
    HandRank correct_hand_rank = HandRank::Four_of_a_Kind;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand, correct_hand_rank);
}

void test_full_house()
{
    cout << endl << "- test_full_house..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Spade, Rank::Five),
        Card(Suit::Heart, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::Six),
        Card(Suit::Club, Rank::Ten),
        Card(Suit::Heart, Rank::Ten),
        Card(Suit::Diamond, Rank::Ten)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin() + 1, cards.end());
    span<Card> cards5(cards.begin() + 2, cards.end());
    array<Card, 5> correct_hand{
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::Six),
        Card(Suit::Club, Rank::Ten),
        Card(Suit::Heart, Rank::Ten),
        Card(Suit::Diamond, Rank::Ten)
    };
    HandRank correct_hand_rank = HandRank::Full_House;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand, correct_hand_rank);
}

void test_flush()
{
    cout << endl << "- test_flush..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Spade, Rank::Four),
        Card(Suit::Spade, Rank::Two),
        Card(Suit::Spade, Rank::Ten),
        Card(Suit::Spade, Rank::Jack),
        Card(Suit::Spade, Rank::Seven),
        Card(Suit::Spade, Rank::King),
        Card(Suit::Spade, Rank::Nine)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin() + 1, cards.end());
    span<Card> cards5(cards.begin() + 2, cards.end());
    array<Card, 5> correct_hand{
        Card(Suit::Spade, Rank::Seven),
        Card(Suit::Spade, Rank::Nine),
        Card(Suit::Spade, Rank::Ten),
        Card(Suit::Spade, Rank::Jack),
        Card(Suit::Spade, Rank::King)
    };
    HandRank correct_hand_rank = HandRank::Flush;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand, correct_hand_rank);
}

void test_straight()
{
    cout << endl << "- test_straight..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Spade, Rank::Five),
        Card(Suit::Heart, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::Seven),
        Card(Suit::Club, Rank::Eight),
        Card(Suit::Heart, Rank::Nine),
        Card(Suit::Diamond, Rank::Ten)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin() + 1, cards.end());
    span<Card> cards5(cards.begin() + 2, cards.end());
    array<Card, 5> correct_hand{
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::Seven),
        Card(Suit::Club, Rank::Eight),
        Card(Suit::Heart, Rank::Nine),
        Card(Suit::Diamond, Rank::Ten)
    };
    HandRank correct_hand_rank = HandRank::Straight;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand, correct_hand_rank);
}

void test_three_of_a_kind()
{
    cout << endl << "- test_three_of_a_kind..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Spade, Rank::Two),
        Card(Suit::Heart, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::King),
        Card(Suit::Club, Rank::Ten),
        Card(Suit::Heart, Rank::Ten),
        Card(Suit::Diamond, Rank::Ten)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin() + 1, cards.end());
    span<Card> cards5(cards.begin() + 2, cards.end());
    array<Card, 5> correct_hand{
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Club, Rank::Ten),
        Card(Suit::Heart, Rank::Ten),
        Card(Suit::Diamond, Rank::Ten),
        Card(Suit::Spade, Rank::King)
    };
    HandRank correct_hand_rank = HandRank::Three_of_a_Kind;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand, correct_hand_rank);
}

void test_two_pair()
{
    cout << endl << "- test_two_pair..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Spade, Rank::Four),
        Card(Suit::Heart, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::Six),
        Card(Suit::Spade, Rank::Ace),
        Card(Suit::Heart, Rank::Eight),
        Card(Suit::Diamond, Rank::Eight)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin() + 1, cards.end());
    span<Card> cards5(cards.begin() + 2, cards.end());
    array<Card, 5> correct_hand{
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::Six),
        Card(Suit::Heart, Rank::Eight),
        Card(Suit::Diamond, Rank::Eight),
        Card(Suit::Spade, Rank::Ace)
    };
    HandRank correct_hand_rank = HandRank::Two_Pair;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand, correct_hand_rank);
}

void test_one_pair()
{
    cout << endl << "- test_one_pair..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Spade, Rank::Four),
        Card(Suit::Heart, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::Six),
        Card(Suit::Spade, Rank::Ace),
        Card(Suit::Heart, Rank::Eight),
        Card(Suit::Diamond, Rank::Ten)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin() + 1, cards.end());
    span<Card> cards5(cards.begin() + 2, cards.end());
    array<Card, 5> correct_hand{
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Spade, Rank::Six),
        Card(Suit::Heart, Rank::Eight),
        Card(Suit::Diamond, Rank::Ten),
        Card(Suit::Spade, Rank::Ace)
    };
    HandRank correct_hand_rank = HandRank::Pair;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand, correct_hand_rank);
}

void test_high_card()
{
    cout << endl << "- test_high_card..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Club, Rank::Two),
        Card(Suit::Diamond, Rank::Four),
        Card(Suit::Spade, Rank::Five),
        Card(Suit::Heart, Rank::Jack),
        Card(Suit::Spade, Rank::Ace),
        Card(Suit::Heart, Rank::Eight),
        Card(Suit::Diamond, Rank::Ten)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin() + 1, cards.end());
    span<Card> cards5(cards.begin() + 2, cards.end());
    array<Card, 5> correct_hand{
        Card(Suit::Spade, Rank::Five),
        Card(Suit::Heart, Rank::Eight),
        Card(Suit::Diamond, Rank::Ten),
        Card(Suit::Heart, Rank::Jack),
        Card(Suit::Spade, Rank::Ace)
    };
    HandRank correct_hand_rank = HandRank::High_Card;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand, correct_hand_rank);
}

/* Run Tests
******************************************************************************/
int main()
{
    cout << endl << "Beginning tests...\n" << endl;
    test_royal_flush();
    test_straight_flush();
    test_four_of_a_kind();
    test_full_house();
    test_flush();
    test_straight();
    test_three_of_a_kind();
    test_two_pair();
    test_one_pair();
    test_high_card();
    cout << endl << "\nAll tests completed successfully!\n" << endl;

    return 0;
}
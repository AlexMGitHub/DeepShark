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
#include "constants.hpp"
// Using statements
using std::array;
using std::cout;
using std::endl;
using std::sort;
using std::span;
using std::to_underlying;
using std::vector;
using namespace constants;

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

void test_wheel_straight_flush1()
{
    /**
     * Test is_straight_flush() with a "wheel" straight where the ace is rank 1.
    */
    cout << endl << "- test_wheel_straight_flush1..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Diamond, Rank::Ace),
        Card(Suit::Diamond, Rank::Three),
        Card(Suit::Diamond, Rank::Two),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Diamond, Rank::Five),
        Card(Suit::Diamond, Rank::Four),
        Card(Suit::Diamond, Rank::Six)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin(), cards.end() - 1);
    span<Card> cards5(cards.begin(), cards.end() - 2);
    array<Card, 5> correct_hand5{
        Card(Suit::Diamond, Rank::Two),
        Card(Suit::Diamond, Rank::Three),
        Card(Suit::Diamond, Rank::Five),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Diamond, Rank::Ace),
    };
    array<Card, 5> correct_hand6{
        Card(Suit::Diamond, Rank::Ace),
        Card(Suit::Diamond, Rank::Two),
        Card(Suit::Diamond, Rank::Three),
        Card(Suit::Diamond, Rank::Four),
        Card(Suit::Diamond, Rank::Five),
    };
    array<Card, 5> correct_hand7{
        Card(Suit::Diamond, Rank::Two),
        Card(Suit::Diamond, Rank::Three),
        Card(Suit::Diamond, Rank::Four),
        Card(Suit::Diamond, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
    };
    HandRank correct5_hand_rank = HandRank::Flush;
    HandRank correct6_hand_rank = HandRank::Wheel_Straight_Flush;
    HandRank correct7_hand_rank = HandRank::Straight_Flush;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand7, correct7_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand6, correct6_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand5, correct5_hand_rank);
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

void test_full_house2()
{
    cout << endl << "- test_full_house2..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Spade, Rank::Five),
        Card(Suit::Heart, Rank::Six),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Spade, Rank::King),
        Card(Suit::Club, Rank::King),
        Card(Suit::Club, Rank::Six),
        Card(Suit::Diamond, Rank::Six)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin(), cards.end() - 1);
    span<Card> cards5(cards.begin(), cards.end() - 2);
    array<Card, 5> correct_hand7{
        Card(Suit::Heart, Rank::Six),
        Card(Suit::Club, Rank::Six),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Spade, Rank::King),
        Card(Suit::Club, Rank::King)
    };
    array<Card, 5> correct_hand6{
        Card(Suit::Heart, Rank::Six),
        Card(Suit::Club, Rank::Six),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Spade, Rank::King),
        Card(Suit::Club, Rank::King),
    };
    array<Card, 5> correct_hand5{
        Card(Suit::Spade, Rank::Five),
        Card(Suit::Heart, Rank::Six),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Spade, Rank::King),
        Card(Suit::Club, Rank::King),
    };

    HandRank correct_hand_rank7 = HandRank::Full_House;
    HandRank correct_hand_rank6 = HandRank::Full_House;
    HandRank correct_hand_rank5 = HandRank::Three_of_a_Kind;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand7, correct_hand_rank7);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand6, correct_hand_rank6);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand5, correct_hand_rank5);
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

void test_straight2()
{
    /**
     * Test scenario caught by functional testing.
    */
    cout << endl << "- test_straight2..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Diamond, Rank::Queen),
        Card(Suit::Club, Rank::Ace),
        Card(Suit::Club, Rank::Ten),
        Card(Suit::Diamond, Rank::Jack),
        Card(Suit::Club, Rank::Seven),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Club, Rank::Jack)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin(), cards.end() - 1);
    span<Card> cards5(cards.begin(), cards.end() - 2);
    array<Card, 5> correct_hand{
        Card(Suit::Club, Rank::Ten),
        Card(Suit::Diamond, Rank::Jack),
        Card(Suit::Diamond, Rank::Queen),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Club, Rank::Ace)
    };
    array<Card, 5> correct_5card_hand{
        Card(Suit::Club, Rank::Seven),
        Card(Suit::Club, Rank::Ten),
        Card(Suit::Diamond, Rank::Jack),
        Card(Suit::Diamond, Rank::Queen),
        Card(Suit::Club, Rank::Ace)
    };
    HandRank correct_hand_rank = HandRank::Straight;
    HandRank correct_5card_hand_rank = HandRank::High_Card;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand, correct_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_5card_hand, correct_5card_hand_rank);
}

void test_straight3()
{
    /**
     * Test is_straight() functionality with a hand with 3 of a kind.
    */
    cout << endl << "- test_straight3..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Diamond, Rank::Three),
        Card(Suit::Club, Rank::Four),
        Card(Suit::Heart, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Club, Rank::Seven),
        Card(Suit::Spade, Rank::Five),
        Card(Suit::Club, Rank::Five)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin(), cards.end() - 1);
    span<Card> cards5(cards.begin(), cards.end() - 2);
    array<Card, 5> correct_hand{
        Card(Suit::Diamond, Rank::Three),
        Card(Suit::Club, Rank::Four),
        Card(Suit::Heart, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Club, Rank::Seven),
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

void test_straight4()
{
    /**
     * Test is_straight() functionality with a low rank 3 of a kind.
    */
    cout << endl << "- test_straight4..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Diamond, Rank::Three),
        Card(Suit::Club, Rank::Four),
        Card(Suit::Heart, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Club, Rank::Seven),
        Card(Suit::Spade, Rank::Three),
        Card(Suit::Club, Rank::Three)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin(), cards.end() - 1);
    span<Card> cards5(cards.begin(), cards.end() - 2);
    array<Card, 5> correct_hand{
        Card(Suit::Diamond, Rank::Three),
        Card(Suit::Club, Rank::Four),
        Card(Suit::Heart, Rank::Five),
        Card(Suit::Diamond, Rank::Six),
        Card(Suit::Club, Rank::Seven),
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

void test_wheel_straight1()
{
    /**
     * Test is_straight() with a "wheel" straight where the ace is rank 1.
    */
    cout << endl << "- test_wheel_straight1..." << endl;
    // Test Fixtures
    std::array<Card, 7> cards{
        Card(Suit::Diamond, Rank::Ace),
        Card(Suit::Club, Rank::Three),
        Card(Suit::Heart, Rank::Two),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Club, Rank::Five),
        Card(Suit::Heart, Rank::Ace),
        Card(Suit::Spade, Rank::Four)
    };
    span<Card> cards7(cards.begin(), cards.end());
    span<Card> cards6(cards.begin(), cards.end() - 1);
    span<Card> cards5(cards.begin(), cards.end() - 2);
    array<Card, 5> correct_hand5{
        Card(Suit::Heart, Rank::Two),
        Card(Suit::Club, Rank::Three),
        Card(Suit::Club, Rank::Five),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Diamond, Rank::Ace),
    };
    array<Card, 5> correct_hand6{
        Card(Suit::Club, Rank::Three),
        Card(Suit::Club, Rank::Five),
        Card(Suit::Diamond, Rank::King),
        Card(Suit::Diamond, Rank::Ace),
        Card(Suit::Heart, Rank::Ace),
    };
    array<Card, 5> correct_hand7{
        Card(Suit::Heart, Rank::Ace),
        Card(Suit::Heart, Rank::Two),
        Card(Suit::Club, Rank::Three),
        Card(Suit::Spade, Rank::Four),
        Card(Suit::Club, Rank::Five),
    };
    HandRank correct5_hand_rank = HandRank::High_Card;
    HandRank correct6_hand_rank = HandRank::Pair;
    HandRank correct7_hand_rank = HandRank::Wheel_Straight;
    // Execute tests
    cout << "\t- " << "test 7 cards...";
    hand_assert(cards7, correct_hand7, correct7_hand_rank);
    cout << "\t- " << "test 6 cards...";
    hand_assert(cards6, correct_hand6, correct6_hand_rank);
    cout << "\t- " << "test 5 cards...";
    hand_assert(cards5, correct_hand5, correct5_hand_rank);
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
    test_wheel_straight_flush1();
    test_four_of_a_kind();
    test_full_house();
    test_full_house2();
    test_flush();
    test_straight();
    test_straight2();
    test_straight3();
    test_straight4();
    test_wheel_straight1();
    test_three_of_a_kind();
    test_two_pair();
    test_one_pair();
    test_high_card();
    cout << endl << "\nAll tests completed successfully!\n" << endl;

    return 0;
}
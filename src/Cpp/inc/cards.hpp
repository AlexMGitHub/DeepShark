#pragma once

/******************************************************************************
* Declare constants and classes used to represent playing cards.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <random> // For shuffle() and mt19937
#include <vector>
// Project headers
#include "constants.hpp"

/* Declarations
******************************************************************************/
struct Card
{
    // Default constructor initializes a "blank" card
    Card() : suit(constants::Suit::No_Card), rank(constants::Rank::No_Card) {}
    // Otherwise a suit and rank are required on instantiation
    Card(constants::Suit s, constants::Rank r) : suit(s), rank(r) {}
    // Data Members
    constants::Suit suit;
    constants::Rank rank;
    // Member Functions
    bool operator==(const Card& other);
    bool operator<(const Card& other);
    friend std::ostream& operator<<(std::ostream& os, const Card& c);
};

class Deck
{
public:
    // Constructors
    Deck(std::mt19937& rng) : rng(rng)
    {
        m_build_deck();
    }
    // Data Members
    std::mt19937 rng;
    // Member Functions
    void burn_one_card();
    Card deal_one_card();
    void print_cards();
    void shuffle_deck();
    void stack_the_deck(std::vector<Card> cards);
private:
    // Data Members
    int m_top_card_idx = 0;
    std::array<Card, constants::NUMBER_CARDS_IN_DECK> m_cards;
    // Member Functions
    void m_build_deck();
};

class Hand
{
public:
    // Constructors
    Hand() {}
    // Data Members
    Card hole_card1;
    Card hole_card2;
    std::vector<Card> available_cards;
    constants::HandRank hand_rank = constants::HandRank::High_Card;
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand;
    // Member Functions
    void add_card(const Card c);
    void clear_hand();
    void determine_best_hand();
    void print_best_hand();
    void print_hand();
    void print_hand_rank();
    static bool card_sorter(const Card& lhs, const Card& rhs);
    static bool reverse_card_sorter(const Card& lhs, const Card& rhs);
private:
    // Data Members
    std::vector<Card> d, h, c, s, flush_hand, straight_hand, high_hand;
    // Member Functions
    void m_sort_cards();
    bool m_is_one_pair();
    bool m_is_two_pair();
    bool m_is_three_of_a_kind();
    bool m_is_wheel_straight();
    bool m_is_wheel_straight(std::vector<Card> hand);
    bool m_is_straight();
    bool m_is_straight(std::vector<Card> hand);
    bool m_is_flush();
    bool m_is_full_house();
    bool m_is_four_of_a_kind();
    bool m_is_wheel_straight_flush(std::vector<Card> hand);
    bool m_is_straight_flush(std::vector<Card> hand);
};
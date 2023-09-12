#pragma once

/******************************************************************************
 * Declare constants and classes used to represent playing cards.
 *****************************************************************************/

 /* Headers
 ******************************************************************************/
 // C++ standard library
#include <array>
#include <map>
#include <vector>

/* Constants and Enums
******************************************************************************/
const size_t NUMBER_CARDS_IN_DECK = 52;
const size_t NUMBER_CARDS_IN_SUIT = 13;
const size_t NUMBER_SUITS = 4;
const size_t MAX_CARDS_IN_HAND = 5;

enum class Rank : int
{
    No_Card = 0,
    Two = 2,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King,
    Ace
};

enum class Suit : char
{
    No_Card = '0',
    Diamond = 'D',
    Heart = 'H',
    Spade = 'S',
    Club = 'C'
};

enum class HandRank : int
{
    High_Card,
    Pair,
    Two_Pair,
    Three_of_a_Kind,
    Straight,
    Flush,
    Full_House,
    Four_of_a_Kind,
    Straight_Flush,
    Royal_Flush
};

const std::array<Rank, NUMBER_CARDS_IN_SUIT> Card_Ranks{
    Rank::Two,
    Rank::Three,
    Rank::Four,
    Rank::Five,
    Rank::Six,
    Rank::Seven,
    Rank::Eight,
    Rank::Nine,
    Rank::Ten,
    Rank::Jack,
    Rank::Queen,
    Rank::King,
    Rank::Ace };

const std::array<Suit, NUMBER_SUITS> Card_Suits{
    Suit::Diamond,
    Suit::Heart,
    Suit::Spade,
    Suit::Club };

/* Declarations
******************************************************************************/
struct Card
{
    // Default constructor initializes a "blank" card
    Card() : suit(Suit::No_Card), rank(Rank::No_Card) {}
    // Otherwise a suit and rank are required on instantiation
    Card(Suit s, Rank r) : suit(s), rank(r) {}
    // Data Members
    Suit suit;
    Rank rank;
    // Member Functions
    bool operator==(const Card& other);
};

class Deck
{
public:
    // Constructors
    Deck()
    {
        m_build_deck();
        shuffle_deck();
    }
    // Member Functions
    void burn_one_card();
    Card& deal_one_card();
    void print_cards();
    void shuffle_deck();

private:
    // Data Members
    int m_top_card_idx = 0;
    std::array<Card, NUMBER_CARDS_IN_DECK> m_cards;
    // Member Functions
    void m_build_deck();
};

class Hand
{
public:
    // Constructors
    Hand() {}
    // Data Members
    HandRank hand_rank = HandRank::High_Card;
    std::array<Card, MAX_CARDS_IN_HAND> best_hand;
    // Member Functions
    void add_card(const Card& c);
    void clear_hand();
    void determine_best_hand();
    void print_best_hand();
    void print_hand();
    void print_hand_rank();

private:
    // Data Members
    std::vector<Card> m_available_cards;
    std::vector<Card> d, h, c, s, flush_hand, straight_hand, high_hand;
    // Member Functions
    static bool card_sorter(const Card& lhs, const Card& rhs);
    static bool reverse_card_sorter(const Card& lhs, const Card& rhs);
    void sort_cards();
    bool is_one_pair();
    bool is_two_pair();
    bool is_three_of_a_kind();
    bool is_straight();
    bool is_straight(std::vector<Card> hand);
    bool is_flush();
    bool is_full_house();
    bool is_four_of_a_kind();
    bool is_straight_flush();
};
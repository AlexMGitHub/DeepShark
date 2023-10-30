#pragma once

/******************************************************************************
* Declare global constants as inline constexpr
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <map>

/* Global Constants
******************************************************************************/
namespace constants
{
    inline constexpr int NUM_CARDS_FLOP = 3;
    inline constexpr int NUM_CARDS_TURN = 1;
    inline constexpr int NUM_CARDS_RIVER = 1;
    inline constexpr int MAX_PLAYER_COUNT = 10;
    inline constexpr unsigned SMALL_BLIND = 25;
    inline constexpr unsigned BIG_BLIND = 2 * SMALL_BLIND;
    inline constexpr unsigned MAX_BUY_IN = 100 * BIG_BLIND;
    inline constexpr int MAX_BETTING_ROUNDS = 4;
    inline constexpr int NUMBER_CARDS_IN_DECK = 52;
    inline constexpr int NUMBER_CARDS_IN_SUIT = 13;
    inline constexpr int NUMBER_SUITS = 4;
    inline constexpr int MAX_CARDS_IN_HAND = 5;
    inline constexpr int MAX_CONSOLE_LINES = 5;

    const std::map<unsigned, std::pair<unsigned, unsigned>> BLINDS_STRUCTURE
    {
        // Maps blind values to player count
        // Note: inline constexpr means can't use operator[], 
        // must use at() instead

        // Player_Count, {Small_Blind, Big_Blind}
        {10, {SMALL_BLIND, BIG_BLIND}},
        {9, {2 * SMALL_BLIND, 2 * BIG_BLIND}},
        {8, {3 * SMALL_BLIND, 3 * BIG_BLIND}},
        {7, {5 * SMALL_BLIND, 5 * BIG_BLIND}},
        {6, {10 * SMALL_BLIND, 10 * BIG_BLIND}},
        {5, {15 * SMALL_BLIND, 15 * BIG_BLIND}},
        {4, {20 * SMALL_BLIND, 20 * BIG_BLIND}},
        {3, {40 * SMALL_BLIND, 40 * BIG_BLIND}},
        {2, {80 * SMALL_BLIND, 80 * BIG_BLIND}},
    };

    enum class Round : int
    {
        Pre_Flop,
        Flop,
        Turn,
        River,
        Showdown
    };

    enum class Action : int
    {
        No_Action,
        All_In,
        Bet,
        Call,
        Check,
        Fold,
        Raise,
        Re_Raise
    };

    enum class Blind : int
    {
        No_Blind,
        Dealer_Button,
        Small_Blind,
        Big_Blind,
        Dealer_Button_and_Small_Blind,
        UTG,
        UTG_1,
        UTG_2,
        UTG_3,
        UTG_4,
        UTG_5,
        UTG_6,
    };

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

    inline constexpr std::array<Rank, NUMBER_CARDS_IN_SUIT> Card_Ranks
    {
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
        Rank::Ace
    };

    inline constexpr std::array<Suit, NUMBER_SUITS> Card_Suits
    {
        Suit::Diamond,
        Suit::Heart,
        Suit::Spade,
        Suit::Club
    };

    inline constexpr std::array<Blind, MAX_PLAYER_COUNT - 3> UTG_Players
    {
        Blind::UTG,
        Blind::UTG_1,
        Blind::UTG_2,
        Blind::UTG_3,
        Blind::UTG_4,
        Blind::UTG_5,
        Blind::UTG_6
    };

    enum class AI_Type : int
    {
        Random
    };
}
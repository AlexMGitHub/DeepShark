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
        Showdown,
        Game_Result
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
        Random,
        Scripted,
        CheckCall
    };

    const std::map<Rank, std::array<double, 9>> HOLE_CARD_PAIR_PROBS
    {
        {Rank::King, {0.0049, 0.0098, 0.0147, 0.0196, 0.0244, 0.0293, 0.0342, 0.0391, 0.0439}},
        {Rank::Queen, {0.0098, 0.0195, 0.0292, 0.0388, 0.0484, 0.0579, 0.0673, 0.0766, 0.0859}},
        {Rank::Jack, {0.0147, 0.0292, 0.0436, 0.0577, 0.0717, 0.0856, 0.0992, 0.1127, 0.1259}},
        {Rank::Ten, {0.0196, 0.0389, 0.0578, 0.0764, 0.0946, 0.1124, 0.1299, 0.1470, 0.1637}},
        {Rank::Nine, {0.0245, 0.0484, 0.0718, 0.0946, 0.1168, 0.1384, 0.1593, 0.1795, 0.1990}},
        {Rank::Eight, {0.0294, 0.0580, 0.0857, 0.1125, 0.1384, 0.1634, 0.1873, 0.2101, 0.2318}},
        {Rank::Seven, {0.0343, 0.0674, 0.0994, 0.1301, 0.1595, 0.1874, 0.2138, 0.2387, 0.2619}},
        {Rank::Six, {0.0392, 0.0769, 0.1130, 0.1473, 0.1799, 0.2104, 0.2389, 0.2651, 0.2890}},
        {Rank::Five, {0.0441, 0.0862, 0.1263, 0.1642, 0.1996, 0.2324, 0.2623, 0.2892, 0.3129}},
        {Rank::Four, {0.0490, 0.0956, 0.1395, 0.1806, 0.2186, 0.2532, 0.2841, 0.3109, 0.3334}},
        {Rank::Three, {0.0539, 0.1048, 0.1526, 0.1967, 0.2370, 0.2729, 0.3040, 0.3300, 0.3503}},
        {Rank::Two, {0.0588, 0.1141, 0.1654, 0.2124, 0.2546, 0.2914, 0.3222, 0.3464, 0.3633}}
    };

    const std::map<Rank, std::array<double, 9>> HOLE_CARD_NON_PAIR_PROBS
    {
        {Rank::Ace, {0.0588235294117647, 0.117647058823529, 0.176470588235294, 0.235294117647059, 0.294117647058824, 0.352941176470588, 0.411764705882353, 0.470588235294118, 0.529411764705882}},
        {Rank::King, {0.203619909502262, 0.407239819004525, 0.610859728506787, 0.81447963800905, 1, 1, 1, 1, 1}},
        {Rank::Queen, {0.336349924585219, 0.672699849170438, 1, 1, 1, 1, 1, 1, 1}},
        {Rank::Jack, {0.457013574660633, 0.914027149321267, 1, 1, 1, 1, 1, 1, 1}},
        {Rank::Ten, {0.565610859728507, 1, 1, 1, 1, 1, 1, 1, 1}},
        {Rank::Nine, {0.662141779788839, 1, 1, 1, 1, 1, 1, 1, 1}},
        {Rank::Eight, {0.746606334841629, 1, 1, 1, 1, 1, 1, 1, 1}},
        {Rank::Seven, {0.819004524886878, 1, 1, 1, 1, 1, 1, 1, 1}},
        {Rank::Six, {0.879336349924585, 1, 1, 1, 1, 1, 1, 1, 1}},
        {Rank::Five, {0.927601809954751, 1, 1, 1, 1, 1, 1, 1, 1}},
        {Rank::Four, {0.963800904977376, 1, 1, 1, 1, 1, 1, 1, 1}},
        {Rank::Three, {0.987933634992459, 1, 1, 1, 1, 1, 1, 1, 1}}
    };
}
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
                // Maps blind values to a poker tournament blinds schedule
                // Note: inline constexpr means can't use operator[], 
                // must use at() instead

                // Blind level, {Small_Blind, Big_Blind}
                {0, {SMALL_BLIND, BIG_BLIND}},
                {1, {2 * SMALL_BLIND, 2 * BIG_BLIND}},
                {2, {3 * SMALL_BLIND, 3 * BIG_BLIND}},
                {3, {5 * SMALL_BLIND, 5 * BIG_BLIND}},
                {4, {10 * SMALL_BLIND, 10 * BIG_BLIND}},
                {5, {15 * SMALL_BLIND, 15 * BIG_BLIND}},
                {6, {20 * SMALL_BLIND, 20 * BIG_BLIND}},
                {7, {40 * SMALL_BLIND, 40 * BIG_BLIND}},
                {8, {80 * SMALL_BLIND, 80 * BIG_BLIND}},
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
                Dealer_Button_and_Small_Blind,
                Dealer_Button,
                Small_Blind,
                Big_Blind,
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
                Wheel_Straight,
                Straight,
                Wheel_Straight_Flush,
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
                CheckCall,
                Heuristic_TAG,
                Heuristic_LAG
        };

        enum class TableSize : int
        {
                Full_Ring,      // 7 to 10 players, play tighter
                Short_Handed    // 6 or fewer players, play looser
        };

        enum class Position : int
        {
                Early,          // Play tightest
                Mid,            // Play looser
                Late,           // Play loosest
                Not_Playable    // Cards are not worth playing
        };

        enum class PlayStyle : int
        {
                TAG,            // Tight aggressive style
                LAG             // Loose aggressive style
        };

        enum class HoleCards
        {
                Unsuited,       // Hole cards are not same suit
                Suited          // Hole cards are same suit
        };

        const std::map<
                std::tuple<Rank, Rank, HoleCards>,
                Position>
                FULL_RING_TAG_STARTING_HANDS
        {
                // Full Ring, Early, TAG
                {std::make_tuple(Rank::Ace, Rank::Ace, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::King, Rank::King, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Queen, Rank::Queen, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Jack, Rank::Jack, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Ace, Rank::King, HoleCards::Suited),
                        Position::Early},
                // Full Ring, Mid, TAG
                {std::make_tuple(Rank::Ace, Rank::Queen, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Ten, Rank::Ten, HoleCards::Unsuited),
                        Position::Mid},
                {std::make_tuple(Rank::Ace, Rank::King, HoleCards::Unsuited),
                        Position::Mid},
                // Full Ring, Late, TAG
                {std::make_tuple(Rank::Ace, Rank::Jack, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::King, Rank::Queen, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Ace, Rank::Queen, HoleCards::Unsuited),
                        Position::Late},
                {std::make_tuple(Rank::Nine, Rank::Nine, HoleCards::Unsuited),
                        Position::Late},
        };

        const std::map<
                std::tuple<Rank, Rank, HoleCards>,
                Position>
                SHORT_HANDED_TAG_STARTING_HANDS
        {
                // Short Handed, Early, TAG
                {std::make_tuple(Rank::Ace, Rank::Ace, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::King, Rank::King, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Queen, Rank::Queen, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Jack, Rank::Jack, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Ace, Rank::King, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Ace, Rank::Queen, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Ace, Rank::King, HoleCards::Unsuited),
                        Position::Early},
                // Short Handed, Mid, TAG
                {std::make_tuple(Rank::Ace, Rank::Queen, HoleCards::Unsuited),
                        Position::Mid},
                {std::make_tuple(Rank::Ace, Rank::Jack, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::King, Rank::Queen, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Ten, Rank::Ten, HoleCards::Unsuited),
                        Position::Mid},
                {std::make_tuple(Rank::Nine, Rank::Nine, HoleCards::Unsuited),
                        Position::Mid},
                // Short Hand, Late, TAG
                {std::make_tuple(Rank::Ace, Rank::Ten, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Ace, Rank::Jack, HoleCards::Unsuited),
                        Position::Late},
                {std::make_tuple(Rank::King, Rank::Jack, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Eight, Rank::Eight, HoleCards::Unsuited),
                        Position::Late},
        };

        const std::map<
                std::tuple<Rank, Rank, HoleCards>,
                Position>
                LAG_STARTING_HANDS
        {
                // Early, LAG
                {std::make_tuple(Rank::Ace, Rank::Ace, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::King, Rank::King, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Queen, Rank::Queen, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Jack, Rank::Jack, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Ten, Rank::Ten, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Nine, Rank::Nine, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Eight, Rank::Eight, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Seven, Rank::Seven, HoleCards::Unsuited),
                        Position::Early},
                {std::make_tuple(Rank::Ace, Rank::King, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Ace, Rank::Queen, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Ace, Rank::Jack, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Ace, Rank::Ten, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::King, Rank::Queen, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::King, Rank::Jack, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::King, Rank::Ten, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Queen, Rank::Jack, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Queen, Rank::Ten, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Jack, Rank::Ten, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Jack, Rank::Nine, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Ten, Rank::Nine, HoleCards::Suited),
                        Position::Early},
                {std::make_tuple(Rank::Ace, Rank::King, HoleCards::Unsuited),
                        Position::Early},
                // Mid, LAG
                {std::make_tuple(Rank::Six, Rank::Six, HoleCards::Unsuited),
                        Position::Mid},
                {std::make_tuple(Rank::Five, Rank::Five, HoleCards::Unsuited),
                        Position::Mid},
                {std::make_tuple(Rank::Ace, Rank::Nine, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Ace, Rank::Eight, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Ace, Rank::Seven, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Ace, Rank::Six, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::King, Rank::Nine, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Queen, Rank::Nine, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Queen, Rank::Eight, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Jack, Rank::Eight, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Ten, Rank::Eight, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Nine, Rank::Eight, HoleCards::Suited),
                        Position::Mid},
                {std::make_tuple(Rank::Ace, Rank::Queen, HoleCards::Unsuited),
                        Position::Mid},
                // Late, LAG
                {std::make_tuple(Rank::Four, Rank::Four, HoleCards::Unsuited),
                        Position::Late},
                {std::make_tuple(Rank::Three, Rank::Three, HoleCards::Unsuited),
                        Position::Late},
                {std::make_tuple(Rank::Two, Rank::Two, HoleCards::Unsuited),
                        Position::Late},
                {std::make_tuple(Rank::Ace, Rank::Five, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Ace, Rank::Four, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Ace, Rank::Three, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Ace, Rank::Two, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::King, Rank::Eight, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::King, Rank::Seven, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::King, Rank::Six, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::King, Rank::Five, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::King, Rank::Four, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::King, Rank::Three, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::King, Rank::Two, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Jack, Rank::Seven, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Ten, Rank::Seven, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Nine, Rank::Seven, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Nine, Rank::Six, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Eight, Rank::Seven, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Eight, Rank::Six, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Seven, Rank::Six, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Seven, Rank::Five, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Six, Rank::Five, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Five, Rank::Four, HoleCards::Suited),
                        Position::Late},
                {std::make_tuple(Rank::Ace, Rank::Jack, HoleCards::Unsuited),
                        Position::Late},
        };
}
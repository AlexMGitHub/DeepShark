/******************************************************************************
* Define classes used to store records of Texas Hold 'Em games.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"
#include "player.hpp"
#include "storage.hpp"
#include "texasholdem.hpp"
// Using statements
using std::array;
using std::vector;
using namespace constants;

/* Constants and Enums
******************************************************************************/

/* GameState Method Definitions
******************************************************************************/
Rank ShowdownStruct::identify_card_groups(
    const array<Card, MAX_CARDS_IN_HAND> best_hand,
    int group_size,
    Rank ignore_rank) const
{
    std::map<Rank, int> count;
    for (const auto& card : best_hand)
    {
        count[card.rank]++;
    }
    // Identify rank of pair, trips, or quads
    for (const auto& [key, value] : count)
    {
        if (value == group_size && key != ignore_rank)
        {
            return key;
        }
    }
    exit(-1);
}

vector<Rank> ShowdownStruct::identify_kickers(
    const array<Card, MAX_CARDS_IN_HAND> best_hand
) const
{
    std::map<Rank, int> count;
    for (const auto& card : best_hand)
    {
        count[card.rank]++;
    }
    // Identify kicker cards and sort in descending order
    vector<Rank> kickers;
    for (const auto& [key, value] : count)
    {
        if ((value == 1))
        {
            kickers.push_back(key);
        }
    }
    std::sort(kickers.rbegin(), kickers.rend());
    return kickers;
}

bool ShowdownStruct::operator>(const ShowdownStruct& other) const
{
    if (hand_rank > other.hand_rank)
    {
        return true;
    }
    else if (hand_rank == other.hand_rank)
    {
        if (hand_rank == HandRank::Pair)
        {
            Rank pair_rank = identify_card_groups(best_hand, 2);
            Rank other_rank = identify_card_groups(other.best_hand, 2);
            if (pair_rank > other_rank) { return true; }
            else if (pair_rank < other_rank) { return false; }
            else
            {
                vector<Rank> kickers = identify_kickers(best_hand);
                vector<Rank> other_kickers = identify_kickers(other.best_hand);
                for (size_t i = 0; i < kickers.size(); i++)
                {
                    if (kickers[i] > other_kickers[i]) { return true; }
                    else if (kickers[i] < other_kickers[i]) { return false; }
                }
                // Tie, sort by amount players bet
            }
        }
        else if (hand_rank == HandRank::Two_Pair)
        {
            Rank pair_rank1 = identify_card_groups(best_hand, 2);
            Rank pair_rank2 = identify_card_groups(best_hand, 2, pair_rank1);
            Rank other_rank1 = identify_card_groups(other.best_hand, 2);
            Rank other_rank2 = identify_card_groups(other.best_hand, 2, other_rank1);
            Rank high_pair;
            Rank low_pair;
            Rank other_high;
            Rank other_low;
            if (pair_rank1 > pair_rank2)
            {
                high_pair = pair_rank1;
                low_pair = pair_rank2;
            }
            else
            {
                high_pair = pair_rank2;
                low_pair = pair_rank1;
            }
            if (other_rank1 > other_rank2)
            {
                other_high = other_rank1;
                other_low = other_rank2;
            }
            else
            {
                other_high = other_rank2;
                other_low = other_rank1;
            }
            if (high_pair > other_high) { return true; }
            else if (high_pair < other_high) { return false; }
            else
            {
                if (low_pair > other_low) { return true; }
                else if (low_pair < other_low) { return false; }
                else
                {
                    Rank kicker = identify_kickers(best_hand)[0];
                    Rank other_kicker = identify_kickers(other.best_hand)[0];
                    if (kicker > other_kicker) { return true; }
                    else if (kicker < other_kicker) { return false; }
                    else {
                        // Tie, sort by amount players bet 
                    }
                }
            }
        }
        else if (hand_rank == HandRank::Three_of_a_Kind)
        {
            Rank trip_rank = identify_card_groups(best_hand, 3);
            Rank other_rank = identify_card_groups(other.best_hand, 3);
            if (trip_rank > other_rank) { return true; }
            else if (trip_rank < other_rank) { return false; }
            else
            {
                vector<Rank> kickers = identify_kickers(best_hand);
                vector<Rank> other_kickers = identify_kickers(other.best_hand);
                if (kickers[0] > other_kickers[0]) { return true; }
                else if (kickers[0] < other_kickers[0]) { return false; }
                else {
                    if (kickers[1] > other_kickers[1]) { return true; }
                    else if (kickers[1] < other_kickers[1]) { return false; }
                    else {
                        // Tie, sort by amount players bet 
                    }
                }
            }
        }
        else if (hand_rank == HandRank::Full_House)
        {
            Rank trip_rank = identify_card_groups(best_hand, 3);
            Rank other_trip = identify_card_groups(other.best_hand, 3);
            Rank pair_rank = identify_card_groups(best_hand, 2);
            Rank other_pair = identify_card_groups(other.best_hand, 2);
            if (trip_rank > other_trip) { return true; }
            else if (trip_rank < other_trip) { return false; }
            else
            {
                if (pair_rank > other_pair) { return true; }
                else if (pair_rank < other_pair) { return false; }
                else {
                    // Tie, sort by amount players bet 
                }
            }
        }
        else if (hand_rank == HandRank::Four_of_a_Kind)
        {
            Rank quad_rank = identify_card_groups(best_hand, 4);
            Rank other_quad = identify_card_groups(other.best_hand, 4);
            if (quad_rank > other_quad) { return true; }
            else if (quad_rank < other_quad) { return false; }
            else
            {
                Rank kicker = identify_kickers(best_hand)[0];
                Rank other_kicker = identify_kickers(other.best_hand)[0];
                if (kicker > other_kicker) { return true; }
                if (kicker < other_kicker) { return false; }
                else {
                    // Tie, sort by amount players bet 
                }
            }
        }
        else  // Flushes, Royal flushes, straights, and high cards
        {
            for (int i = MAX_CARDS_IN_HAND - 1; i >= 0; i--)
            {
                // If hand ranks are equal determine who has high card or best kicker
                if (best_hand[i].rank > other.best_hand[i].rank)
                {
                    return true;
                }
                else if (best_hand[i].rank < other.best_hand[i].rank)
                {
                    return false;
                }
            }
        }
    }
    else
    {
        // Other hand rank is higher
        return false;
    }
    if (total_chips_bet < other.total_chips_bet)
    {
        return true;  // In case of tie sort smaller player bets first
    }
    return false;
}

bool ShowdownStruct::operator==(const ShowdownStruct& other)
{
    if (hand_rank != other.hand_rank) { return false; }
    for (int i = MAX_CARDS_IN_HAND - 1; i >= 0; i--)
    {
        if (best_hand[i].rank != other.best_hand[i].rank)
        {
            return false;
        }
    }
    return true;
}
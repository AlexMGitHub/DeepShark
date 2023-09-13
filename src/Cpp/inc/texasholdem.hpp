#pragma once

/******************************************************************************
 * Declare functions necessary to implement the Texas Hold 'Em environment.
 *****************************************************************************/

 /* Headers
 ******************************************************************************/
 // C++ standard library
#include <array>
#include <map>
#include <vector>
// Project headers
#include "cards.hpp"
#include "player.hpp"

/* Constants and Enums
******************************************************************************/
unsigned NUM_CARDS_FLOP = 3;
unsigned NUM_CARDS_TURN = 1;
unsigned NUM_CARDS_RIVER = 1;


const std::map<int, std::pair<int, int>> BLINDS_STRUCTURE
{
    // Maps blind values to player count
    // Note: const means can't use operator[], must use at() instead

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

/* Declarations
******************************************************************************/
class Pot
{
public:
    // Member Functions
    void add_chips(unsigned chips);
    unsigned get_chip_count();
private:
    // Friends
    friend unsigned Dealer::pay_to_winner();
    // Data Members
    unsigned m_chip_count = 0;
    // Member Functions
    unsigned m_pay_to_winner();
};

class Dealer
{
public:
    // Data Members

    // Member Functions
    void burn_one_card();
    Card& deal_one_card();
    void shuffle_deck();
    unsigned pay_to_winner();
    void populate_table();
    void start_round();
    void deal_flop();
    void deal_turn;
    void deal_river;
    void determine_winner();
    void end_round();
private:
    // Data Members
    std::vector<Player> m_player_list;
    Deck m_deck;
    int m_small_blind_idx = 0;
    int m_big_blind_idx = 1;
    std::array<Card, NUM_CARDS_FLOP> m_flop;
    Card m_turn;
    Card m_river;

};

class Table
{
public:
    // Member Functions
    std::vector<Player> player_list;
    void populate_table();
private:
};

class TexasHoldEm
{
public:
    // Member Functions
    void begin_tournament();
    void begin_round();
private:
    // Member Functions
    void m_move_blinds()
        void m_pre_flop();
    void m_flop();
    void m_turn();
    void m_river();
    void m_showdown();
    Player m_tournament_winner;
}
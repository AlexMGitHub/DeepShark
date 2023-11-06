#pragma once

/******************************************************************************
* Declare classes representing a poker dealer and poker pot.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <random> // For mt19937 and uniform distribution
#include <vector>
// Project headers
#include "constants.hpp"
#include "cards.hpp"

/* Forward Declarations
******************************************************************************/
class Player;
class TexasHoldEm;

/* Declarations
******************************************************************************/
class Pot
{
public:
    // Constructors
    // Member Functions
    void add_chips(unsigned chips, unsigned player_idx, constants::Round rnd);
    unsigned get_chip_count();
    unsigned get_total_player_bets(int player_idx) const;
    unsigned get_player_bets_by_round(unsigned player_idx, constants::Round rnd) const;
private:
    // Friends
    friend TexasHoldEm;
    // Data Members
    std::array<
        std::array<unsigned, constants::MAX_BETTING_ROUNDS>, constants::MAX_PLAYER_COUNT>
        m_player_bets = { 0 };
    unsigned m_chip_count = 0;
    // Member Functions
    unsigned m_pay_to_winner(unsigned chips_bet);
    void m_clear_players_bets(int player_idx);
};

class Dealer
{
public:
    // Constructors
    Dealer(std::mt19937& rng) : m_deck(rng) {}
    // Data Members
    Card flop_card1;
    Card flop_card2;
    Card flop_card3;
    Card turn_card;
    Card river_card;
    // Member Functions
    void burn_one_card();
    void clear_cards_from_table();
    Card deal_one_card();
    void shuffle_deck();
    void deal_to_players(
        std::vector<Player>& player_list,
        int num_players,
        int button_idx);
    void deal_flop(std::vector<Player>& player_list);
    void deal_turn(std::vector<Player>& player_list);
    void deal_river(std::vector<Player>& player_list);
    void stack_the_deck(std::vector<Card> cards);
private:
    // Data Members
    Deck m_deck;
};
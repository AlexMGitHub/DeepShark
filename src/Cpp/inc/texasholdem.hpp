#pragma once

/******************************************************************************
* Declare functions necessary to implement the Texas Hold 'Em environment.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <cassert>
#include <functional> // For reference_wrapper()
#include <map>
#include <vector>
// Project headers
#include "cards.hpp"
#include "player.hpp"

/* Constants and Enums
******************************************************************************/
const unsigned NUM_CARDS_FLOP = 3;
const unsigned NUM_CARDS_TURN = 1;
const unsigned NUM_CARDS_RIVER = 1;
const unsigned MAX_PLAYER_COUNT = 10;
const unsigned SMALL_BLIND = 25;
const unsigned BIG_BLIND = 2 * SMALL_BLIND;
const unsigned MAX_BUY_IN = 100 * BIG_BLIND;
const unsigned MAX_BETTING_ROUNDS = 4;

const std::map<unsigned, std::pair<unsigned, unsigned>> BLINDS_STRUCTURE
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

enum class Round : int
{
    Pre_Flop,
    Flop,
    Turn,
    River,
    Showdown
};

/* Declarations
******************************************************************************/
class TexasHoldEm
{
public:
    // Constructors
    TexasHoldEm(unsigned num_players = 10) :
        initial_num_players(num_players),
        num_players(num_players),
        pot(num_players)
    {
        assert((num_players >= 2 && num_players <= 10) &&
            "Number of players must be between 2 and 10!");
    }
    // Data Members
    const unsigned initial_num_players;
    unsigned num_players;
    std::vector<Player> full_player_list;
    std::vector<std::reference_wrapper<Player>> remaining_player_list;
    Pot pot;
    Dealer dealer;
    // Member Functions
    void begin_tournament();
private:
    // Data Members
    int m_tournament_winner;
    int m_round_winner;
    bool m_tournament_started = false;
    bool m_tournament_completed = false;
    int m_button_idx;  // Dealer button
    int m_sb_idx;  // Small blind
    int m_bb_idx;  // Big blind
    unsigned m_round_count = 0;
    GameHistory game_hist;
    // Member Functions
    //void m_prepare_table();
    //bool is_legal_action();
    void m_begin_round();
    void m_pre_flop();
    void m_flop();
    void m_turn();
    void m_river();
    void m_showdown();
    void m_end_round(int winner_idx, int side_pot_winner_idx = -1);
    bool m_is_betting_over(Round rnd) const;
    unsigned m_get_num_active_players() const;
    std::vector<int> m_get_vec_active_players() const;
    void m_move_blinds();
    void m_update_game_state(GameState& gs, RoundData& round);
    std::vector<unsigned> m_get_player_chip_counts() const;
    unsigned m_get_num_active_players() const;
    void m_betting_loop(RoundData& rd, GameState& gs);

};

struct LegalActions
{
    std::vector<Action> legal_actions;
    unsigned chips_to_call;
    unsigned max_bet;
    unsigned min_to_raise;
};


class Dealer
{
public:
    // Data Members

    // Member Functions
    void burn_one_card();
    Card& deal_one_card();
    void shuffle_deck();
    //unsigned pay_to_winner();
    //void start_round();
    void deal_to_players(
        vector<Player>& player_list,
        unsigned num_players,
        int button_idx);
    void deal_flop(vector<Player>& player_list);
    void deal_turn(vector<Player>& player_list);
    void deal_river(vector<Player>& player_list);
    //void determine_winner();
    //void end_round();
private:
    // Data Members
    // Deck m_deck;
    // std::array<Card, NUM_CARDS_FLOP> m_flop;
    // Card m_turn;
    // Card m_river;

};


class Pot
{
public:
    // Constructors
    Pot(unsigned num_players) :
        m_player_bets(num_players,
            std::vector<unsigned>(MAX_BETTING_ROUNDS, 0))
    {}
    // Member Functions
    void add_chips(unsigned chips, unsigned player_idx, Round rnd);
    unsigned get_chip_count();
    unsigned get_player_bets_by_round(unsigned player_idx, Round rnd) const;
private:
    // Friends
    friend TexasHoldEm;
    // Data Members
    std::array<
        std::array<unsigned, MAX_BETTING_ROUNDS>, MAX_PLAYER_COUNT>
        m_player_bets = { 0 };
    unsigned m_chip_count = 0;
    // Member Functions
    unsigned m_pay_to_winner();
};

class Table
{
public:
    // Member Functions


private:
};



// Game loop should have the following:
    // TexasHoldEM the;
    // Pot pot;
    // Pot side_pot;
    // Dealer dealer;
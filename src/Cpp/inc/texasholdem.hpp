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
#include <random> // For mt19937 and uniform distribution
#include <vector>
// Project headers
#include "constants.hpp"
#include "storage.hpp"

/* Forward Declarations
******************************************************************************/
struct Card;
// struct GameHistory;
// struct GameState;
class Player;
// struct RoundData;

/* Declarations
******************************************************************************/
class TexasHoldEm;

class Pot
{
public:
    // Constructors
    // Member Functions
    void add_chips(unsigned chips, unsigned player_idx, constants::Round rnd);
    unsigned get_chip_count();
    unsigned get_total_player_bets(int player_idx);
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
    //unsigned m_pay_to_winner(GameState gs, int winner_idx);
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
    //unsigned pay_to_winner();
    //void start_round();
    void deal_to_players(
        std::vector<Player>& player_list,
        int num_players,
        int button_idx);
    void deal_flop(std::vector<Player>& player_list);
    void deal_turn(std::vector<Player>& player_list);
    void deal_river(std::vector<Player>& player_list);
    //void determine_winner();
    //void end_round();
private:
    // Data Members
    Deck m_deck;
    // std::array<Card, NUM_CARDS_FLOP> m_flop;
    // Card m_turn;
    // Card m_river;

};

class TexasHoldEm
{
public:
    // Constructors
    TexasHoldEm(int num_players = 10,
        int random_seed = 0,
        int tournament_number = 0) :
        initial_num_players(num_players),
        num_players(num_players),
        random_seed(random_seed),
        rng(random_seed),
        dealer(rng),
        tournament_number(tournament_number)
    {
        assert((num_players >= 2 && num_players <= 10) &&
            "Number of players must be between 2 and 10!");
    }
    // Data Members
    const int initial_num_players;
    int num_players;
    int random_seed;
    std::vector<Player> full_player_list;
    std::vector<std::reference_wrapper<Player>> remaining_player_list;
    Pot pot;
    std::mt19937 rng;
    Dealer dealer;
    const int tournament_number;
    // Member Functions
    void begin_tournament();
private:
    // Data Members
    int m_tournament_winner;
    int m_round_winner;
    bool m_tournament_started = false;
    bool m_tournament_completed = false;
    int m_button_idx = 0;  // Dealer button
    int m_sb_idx = 1;  // Small blind
    int m_bb_idx = 2;  // Big blind
    GameHistory game_hist;
    TournamentHistory tourn_hist;
    // Member Functions
    //void m_prepare_table();
    //bool is_legal_action();
    void m_begin_round(GameState gs);
    void m_pre_flop(GameState gs);
    void m_flop(GameState gs);
    void m_turn(GameState gs);
    void m_river(GameState gs);
    void m_showdown(GameState gs);
    void m_end_round(GameState gs);
    bool m_is_betting_over(constants::Round rnd) const;
    int m_get_num_active_players() const;
    int m_get_num_active_not_allin_players() const;
    std::vector<int> m_get_vec_active_players() const;
    void m_move_blinds();
    void m_update_game_state(GameState& gs);
    std::vector<unsigned> m_get_player_chip_counts() const;
    void m_betting_loop(GameState& gs, int starting_player);
    int m_player_closest_to_left_of_dealer(std::vector<int> tie_list);
};

struct LegalActions
{
    std::vector<constants::Action> legal_actions;
    unsigned chips_to_call;
    unsigned max_bet;
    unsigned min_to_raise;
};


// Game loop should have the following:
    // TexasHoldEM the;
    // Pot pot;
    // Pot side_pot;
    // Dealer dealer;
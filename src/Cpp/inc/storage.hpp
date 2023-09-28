#pragma once

/******************************************************************************
* Declare classes used to store records of Texas Hold 'Em games.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <vector>
// Project headers
#include "cards.hpp"
#include "player.hpp"
#include "texasholdem.hpp"

/* Constants and Enums
******************************************************************************/

/* Declarations
******************************************************************************/
class RunTournaments
{
    int initial_player_count;
    // Constructor needs AIs
    int num_tournaments;
    std::vector<int> random_seeds;
    std::vector<TournamentHistory> tournaments;
    // Methods to run tournaments in parallel
};

struct TournamentHistory
{
    int initial_player_count;
    int random_seed;
    std::vector<GameHistory> games;
};

struct GameHistory
{
    int game_number;
    std::vector<GameState> states;
};

struct GameState
{
    // Data Members
    int player_idx;
    int action_number;
    Blind blind;
    Round round;
    // Before action
    bool raise_active;
    int raise_player_idx;
    int num_active_players;
    unsigned pot_chip_count;
    std::vector<unsigned> player_chip_counts;
    std::vector<int> active_player_list;
    std::array<Card, MAX_CARDS_IN_HAND> best_hand;
    std::vector<Card> available_cards;
    std::vector<Action> legal_actions;
    // After action
    Action player_action;
    unsigned player_bet;
    // After Showdown
    std::vector<int> showdown_player_idx;
    std::vector<std::array<Card, MAX_CARDS_IN_HAND>> showdown_hands;
    std::vector<HandRank> showdown_hand_ranks;
    int winner_idx;
    // Member Functions
};

struct RoundData
{
    int player_idx;
    Round round;
    unsigned pot_chip_count;
    unsigned sum_prev_bets;
    unsigned chips_to_call;
    bool legal_to_raise;
    bool raise_active;
    int raise_player_idx;
    bool all_in_below_min_raise;
    unsigned action_number;
    unsigned min_bet;
    unsigned max_bet;
    unsigned player_bet;
    unsigned min_to_raise;
    unsigned num_active_players;
    Action player_action;
    std::vector<Action> legal_actions;
};
#pragma once

/******************************************************************************
* Declare functions necessary to implement the Texas Hold 'Em environment.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <cassert>
#include <deque>
#include <random> // For mt19937 and uniform distribution
#include <string>
#include <vector>
// Project headers
#include "constants.hpp"
#include "pot_dealer.hpp"
#include "storage.hpp"

/* Forward Declarations
******************************************************************************/
struct Card;
class Player;

/* Declarations
******************************************************************************/
class TexasHoldEm
{
public:
    // Constructors
    TexasHoldEm(
        std::vector<constants::AI_Type> player_ai_types,
        int num_players = 10,
        int random_seed = 0,
        int tournament_number = 0,
        bool debug = false,
        int num_games_per_blind_level = 30,
        std::string tourn_directory = "") :
        player_ai_types(player_ai_types),
        initial_num_players(num_players),
        num_players(num_players),
        random_seed(random_seed),
        rng(random_seed),
        dealer(rng),
        tournament_number(tournament_number),
        debug(debug),
        num_games_per_blind_level(num_games_per_blind_level),
        tourn_directory(tourn_directory)
    {
        assert((num_players >= 2 && num_players <= 10) &&
            "Number of players must be between 2 and 10!");
        // Initialize players
        for (int i = 0; i < initial_num_players; i++)
        {
            full_player_list.push_back(
                Player(
                    i,
                    constants::MAX_BUY_IN,
                    player_ai_types[i],
                    rng));
        }
        // Initalize data storage structures
        tourn_hist.tournament_number = tournament_number;
        tourn_hist.initial_player_count = initial_num_players;
        tourn_hist.random_seed = random_seed;
        game_hist.game_number = 0;
        tourn_hist.player_ai_types = player_ai_types;
    }
    // Data Members
    std::vector<constants::AI_Type> player_ai_types;
    const int initial_num_players;
    int num_players;
    const int random_seed;
    std::vector<Player> full_player_list;
    Pot pot;
    std::mt19937 rng;
    Dealer dealer;
    const int tournament_number;
    bool debug;
    std::string filename;
    size_t mc_total_runs = 0;
    size_t mc_run_wins = 0;
    int num_games_per_blind_level;
    std::string tourn_directory;
    // Member Functions
    void begin_tournament();
    void begin_test_game(TestCase tc);
    void begin_mc_game(std::vector<Card> player_hand, size_t num_runs);
private:
    // Data Members
    int m_tournament_winner;
    int m_round_winner;
    bool m_tournament_started = false;
    bool m_tournament_completed = false;
    bool m_test_game = false;
    bool m_monte_carlo_game = false;
    int m_button_idx = 0;  // Dealer button
    int m_sb_idx = 1;  // Small blind
    int m_bb_idx = 2;  // Big blind
    int m_start_action = 0;
    constants::Round m_round = constants::Round::Pre_Flop;
    int m_adjusted_num_players = constants::MAX_PLAYER_COUNT;
    TestCase m_test_case;
    GameHistory game_hist;
    TournamentHistory tourn_hist;
    std::vector<Card> m_partial_deck;
    std::vector<Card> m_mc_deck;
    // Member Functions
    void m_begin_game(GameState gs);
    void m_pre_flop(GameState gs);
    void m_flop(GameState gs);
    void m_turn(GameState gs);
    void m_river(GameState gs);
    void m_showdown(GameState gs);
    void m_end_game(GameState gs);
    void m_determine_game_winner(GameState& gs);
    void m_betting_loop(GameState& gs, int starting_player);
    void m_determine_legal_actions(GameState& gs, int plyr_idx);
    void m_validate_player_action(GameState& gs, int plyr_idx);
    bool m_is_betting_over(GameState& gs);
    void m_move_blinds();
    void m_update_game_state(GameState& gs);
    int m_get_num_active_players() const;
    int m_get_num_active_not_allin_players() const;
    int m_player_closest_to_left_of_dealer(std::vector<int> tie_list);
    std::vector<int> m_get_vec_active_players() const;
    std::vector<int> m_get_vec_remaining_players() const;
    std::vector<unsigned> m_get_player_chip_counts() const;
    void m_load_script(GameState gs);
    void m_validate_test_results(GameState gs);
    void m_load_mc_deck(GameState gs);
    void m_build_partial_deck(std::vector<Card> exclude);
    void m_build_mc_deck(std::vector<Card> starting_hand);
    int m_blind_level();
};
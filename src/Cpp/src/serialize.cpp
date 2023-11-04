/******************************************************************************
* Define classes used to serialize and deserialize game states.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"
#include "serialize.hpp"
#include "storage.hpp"
// Using statements
using std::cout;
using std::endl;
using std::sort;
using std::string;
using std::to_underlying;
using std::vector;
using namespace constants;

/* Forward Declarations
******************************************************************************/


/* Serialize/Deserialize Definitions
******************************************************************************/
ShowdownStruct read_showdownstruct(std::ifstream& fs)
{
    if (!fs.is_open())
    {
        cout << "Failed to open file!" << endl;
        exit(-1);
    }
    else
    {
        ShowdownStruct sd;
        fs.read(reinterpret_cast<char*>(&sd.player_idx), sizeof sd.player_idx);
        fs.read(reinterpret_cast<char*>(&sd.best_hand[0]),
            sizeof sd.best_hand[0] * MAX_CARDS_IN_HAND);
        fs.read(reinterpret_cast<char*>(&sd.hand_rank), sizeof sd.hand_rank);
        fs.read(reinterpret_cast<char*>(&sd.total_chips_bet), sizeof sd.total_chips_bet);
        fs.read(reinterpret_cast<char*>(&sd.chips_won), sizeof sd.chips_won);
        return sd;
    }
}

void write_showdownstruct(std::ofstream& fs, ShowdownStruct& sd)
{
    if (!fs.is_open())
    {
        cout << "Failed to open file!" << endl;
        exit(-1);
    }
    else
    {
        fs.write(reinterpret_cast<char*>(&sd.player_idx), sizeof sd.player_idx);
        fs.write(reinterpret_cast<char*>(&sd.best_hand[0]),
            sizeof sd.best_hand[0] * MAX_CARDS_IN_HAND);
        fs.write(reinterpret_cast<char*>(&sd.hand_rank), sizeof sd.hand_rank);
        fs.write(reinterpret_cast<char*>(&sd.total_chips_bet), sizeof sd.total_chips_bet);
        fs.write(reinterpret_cast<char*>(&sd.chips_won), sizeof sd.chips_won);
    }
}

GameState read_gamestate(std::ifstream& fs)
{
    if (!fs.is_open())
    {
        cout << "Failed to open file!" << endl;
        exit(-1);
    }
    else
    {
        GameState gs;
        fs.read(reinterpret_cast<char*>(&gs.random_seed), sizeof gs.random_seed);
        fs.read(reinterpret_cast<char*>(&gs.tournament_number), sizeof gs.tournament_number);
        fs.read(reinterpret_cast<char*>(&gs.game_number), sizeof gs.game_number);
        fs.read(reinterpret_cast<char*>(&gs.initial_num_players), sizeof gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.num_players), sizeof gs.num_players);
        fs.read(reinterpret_cast<char*>(&gs.num_active_players), sizeof gs.num_active_players);
        fs.read(reinterpret_cast<char*>(&gs.action_number), sizeof gs.action_number);
        fs.read(reinterpret_cast<char*>(&gs.round), sizeof gs.round);
        // Player info        
        fs.read(reinterpret_cast<char*>(&gs.player_idx), sizeof gs.player_idx);
        gs.blinds.resize(gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.blinds[0]), sizeof(gs.blinds[0]) * gs.initial_num_players);
        gs.last_actions.resize(gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.last_actions[0]), sizeof(gs.last_actions[0]) * gs.initial_num_players);
        gs.ai_types.resize(gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.ai_types[0]), sizeof(gs.ai_types[0]) * gs.initial_num_players);
        gs.hole_cards.resize(gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.hole_cards[0]), sizeof(gs.hole_cards[0]) * gs.initial_num_players);
        gs.best_hands.resize(gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.best_hands[0]), sizeof(gs.best_hands[0]) * gs.initial_num_players);
        gs.hand_ranks.resize(gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.hand_ranks[0]), sizeof(gs.hand_ranks[0]) * gs.initial_num_players);
        gs.remaining_players.resize(gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.remaining_players[0]), sizeof(gs.remaining_players[0]) * gs.initial_num_players);
        // Before action
        fs.read(reinterpret_cast<char*>(&gs.sum_prev_bets), sizeof gs.sum_prev_bets);
        fs.read(reinterpret_cast<char*>(&gs.legal_to_raise), sizeof gs.legal_to_raise);
        fs.read(reinterpret_cast<char*>(&gs.raise_active), sizeof gs.raise_active);
        fs.read(reinterpret_cast<char*>(&gs.all_in_below_min_raise), sizeof gs.all_in_below_min_raise);
        fs.read(reinterpret_cast<char*>(&gs.raise_player_idx), sizeof gs.raise_player_idx);
        fs.read(reinterpret_cast<char*>(&gs.pot_chip_count), sizeof gs.pot_chip_count);
        fs.read(reinterpret_cast<char*>(&gs.chips_to_call), sizeof gs.chips_to_call);
        fs.read(reinterpret_cast<char*>(&gs.min_bet), sizeof gs.min_bet);
        fs.read(reinterpret_cast<char*>(&gs.max_bet), sizeof gs.max_bet);
        fs.read(reinterpret_cast<char*>(&gs.min_to_raise), sizeof gs.min_to_raise);
        fs.read(reinterpret_cast<char*>(&gs.flop_card1), sizeof gs.flop_card1);
        fs.read(reinterpret_cast<char*>(&gs.flop_card2), sizeof gs.flop_card2);
        fs.read(reinterpret_cast<char*>(&gs.flop_card3), sizeof gs.flop_card3);
        fs.read(reinterpret_cast<char*>(&gs.turn_card), sizeof gs.turn_card);
        fs.read(reinterpret_cast<char*>(&gs.river_card), sizeof gs.river_card);

        // Vectors and arrays
        fs.read(reinterpret_cast<char*>(&gs.pot_player_bets[0]),
            sizeof(unsigned) * MAX_PLAYER_COUNT * MAX_BETTING_ROUNDS);

        gs.player_chip_counts.resize(gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.player_chip_counts[0]),
            sizeof(gs.player_chip_counts[0]) * gs.initial_num_players);


        gs.active_player_list.resize(gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.active_player_list[0]),
            sizeof(gs.active_player_list[0]) * gs.initial_num_players);

        fs.read(reinterpret_cast<char*>(&gs.best_hand[0]),
            sizeof(Card) * MAX_CARDS_IN_HAND);

        fs.read(reinterpret_cast<char*>(&gs.num_available_cards), sizeof gs.num_available_cards);
        gs.available_cards.resize(gs.num_available_cards);
        fs.read(reinterpret_cast<char*>(&gs.available_cards[0]),
            sizeof(gs.available_cards[0]) * gs.num_available_cards);

        fs.read(reinterpret_cast<char*>(&gs.num_legal_actions), sizeof gs.num_legal_actions);
        gs.legal_actions.resize(gs.num_legal_actions);
        fs.read(reinterpret_cast<char*>(&gs.legal_actions[0]),
            sizeof(gs.legal_actions[0]) * gs.num_legal_actions);

        // After action
        fs.read(reinterpret_cast<char*>(&gs.player_action), sizeof gs.player_action);
        fs.read(reinterpret_cast<char*>(&gs.player_bet), sizeof gs.player_bet);

        // After Showdown
        fs.read(reinterpret_cast<char*>(&gs.num_showdown_players), sizeof gs.num_showdown_players);
        for (size_t i = 0; i < gs.num_showdown_players; i++)
        {
            gs.showdown_players.push_back(read_showdownstruct(fs));
        }
        return gs;
    }
}

void write_gamestate(std::ofstream& fs, GameState& gs)
{
    if (!fs.is_open())
    {
        cout << "Failed to open file!" << endl;
        exit(-1);
    }
    else
    {
        fs.write(reinterpret_cast<char*>(&gs.random_seed), sizeof gs.random_seed);
        fs.write(reinterpret_cast<char*>(&gs.tournament_number), sizeof gs.tournament_number);
        fs.write(reinterpret_cast<char*>(&gs.game_number), sizeof gs.game_number);
        fs.write(reinterpret_cast<char*>(&gs.initial_num_players), sizeof gs.initial_num_players);
        fs.write(reinterpret_cast<char*>(&gs.num_players), sizeof gs.num_players);
        fs.write(reinterpret_cast<char*>(&gs.num_active_players), sizeof gs.num_active_players);
        fs.write(reinterpret_cast<char*>(&gs.action_number), sizeof gs.action_number);
        fs.write(reinterpret_cast<char*>(&gs.round), sizeof gs.round);
        // Player info        
        fs.write(reinterpret_cast<char*>(&gs.player_idx), sizeof gs.player_idx);
        fs.write(reinterpret_cast<char*>(&gs.blinds[0]), sizeof(gs.blinds[0]) * gs.initial_num_players);
        fs.write(reinterpret_cast<char*>(&gs.last_actions[0]), sizeof(gs.last_actions[0]) * gs.initial_num_players);
        fs.write(reinterpret_cast<char*>(&gs.ai_types[0]), sizeof(gs.ai_types[0]) * gs.initial_num_players);
        fs.write(reinterpret_cast<char*>(&gs.hole_cards[0]), sizeof(gs.hole_cards[0]) * gs.initial_num_players);
        fs.write(reinterpret_cast<char*>(&gs.best_hands[0]), sizeof(gs.best_hands[0]) * gs.initial_num_players);
        fs.write(reinterpret_cast<char*>(&gs.hand_ranks[0]), sizeof(gs.hand_ranks[0]) * gs.initial_num_players);
        fs.write(reinterpret_cast<char*>(&gs.remaining_players[0]), sizeof(gs.remaining_players[0]) * gs.initial_num_players);
        // Before action
        fs.write(reinterpret_cast<char*>(&gs.sum_prev_bets), sizeof gs.sum_prev_bets);
        fs.write(reinterpret_cast<char*>(&gs.legal_to_raise), sizeof gs.legal_to_raise);
        fs.write(reinterpret_cast<char*>(&gs.raise_active), sizeof gs.raise_active);
        fs.write(reinterpret_cast<char*>(&gs.all_in_below_min_raise), sizeof gs.all_in_below_min_raise);
        fs.write(reinterpret_cast<char*>(&gs.raise_player_idx), sizeof gs.raise_player_idx);
        fs.write(reinterpret_cast<char*>(&gs.pot_chip_count), sizeof gs.pot_chip_count);
        fs.write(reinterpret_cast<char*>(&gs.chips_to_call), sizeof gs.chips_to_call);
        fs.write(reinterpret_cast<char*>(&gs.min_bet), sizeof gs.min_bet);
        fs.write(reinterpret_cast<char*>(&gs.max_bet), sizeof gs.max_bet);
        fs.write(reinterpret_cast<char*>(&gs.min_to_raise), sizeof gs.min_to_raise);
        fs.write(reinterpret_cast<char*>(&gs.flop_card1), sizeof gs.flop_card1);
        fs.write(reinterpret_cast<char*>(&gs.flop_card2), sizeof gs.flop_card2);
        fs.write(reinterpret_cast<char*>(&gs.flop_card3), sizeof gs.flop_card3);
        fs.write(reinterpret_cast<char*>(&gs.turn_card), sizeof gs.turn_card);
        fs.write(reinterpret_cast<char*>(&gs.river_card), sizeof gs.river_card);

        // Vectors and arrays
        fs.write(reinterpret_cast<char*>(&gs.pot_player_bets[0]),
            sizeof(unsigned) * MAX_PLAYER_COUNT * MAX_BETTING_ROUNDS);

        fs.write(reinterpret_cast<char*>(&gs.player_chip_counts[0]),
            sizeof(gs.player_chip_counts[0]) * gs.initial_num_players);

        fs.write(reinterpret_cast<char*>(&gs.active_player_list[0]),
            sizeof(gs.active_player_list[0]) * gs.initial_num_players);

        fs.write(reinterpret_cast<char*>(&gs.best_hand[0]),
            sizeof(gs.best_hand[0]) * MAX_CARDS_IN_HAND);

        fs.write(reinterpret_cast<char*>(&gs.num_available_cards), sizeof gs.num_available_cards);
        fs.write(reinterpret_cast<char*>(&gs.available_cards[0]),
            sizeof(gs.available_cards[0]) * gs.num_available_cards);

        fs.write(reinterpret_cast<char*>(&gs.num_legal_actions), sizeof gs.num_legal_actions);
        fs.write(reinterpret_cast<char*>(&gs.legal_actions[0]),
            sizeof(gs.legal_actions[0]) * gs.num_legal_actions);

        // After action
        fs.write(reinterpret_cast<char*>(&gs.player_action), sizeof gs.player_action);
        fs.write(reinterpret_cast<char*>(&gs.player_bet), sizeof gs.player_bet);

        // After Showdown
        fs.write(reinterpret_cast<char*>(&gs.num_showdown_players), sizeof gs.num_showdown_players);
        for (size_t i = 0; i < gs.num_showdown_players; i++)
        {
            write_showdownstruct(fs, gs.showdown_players[i]);
        }
    }
}

GameHistory read_gamehistory(std::ifstream& fs)
{
    GameHistory gh;
    if (!fs.is_open())
    {
        cout << "Failed to open file!" << endl;
        exit(-1);
    }
    else
    {
        fs.read(reinterpret_cast<char*>(&gh.game_number), sizeof gh.game_number);
        fs.read(reinterpret_cast<char*>(&gh.num_states), sizeof gh.num_states);
        for (size_t i = 0; i < gh.num_states; i++)
        {
            gh.states.push_back(read_gamestate(fs));
        }
    }
    return gh;
}

void write_gamehistory(std::ofstream& fs, GameHistory& gh)
{
    if (!fs.is_open())
    {
        cout << "Failed to open file!" << endl;
        exit(-1);
    }
    else
    {
        fs.write(reinterpret_cast<char*>(&gh.game_number), sizeof gh.game_number);
        fs.write(reinterpret_cast<char*>(&gh.num_states), sizeof gh.num_states);
        for (size_t i = 0; i < gh.num_states; i++)
        {
            write_gamestate(fs, gh.states[i]);
        }
    }
}

TournamentHistory read_tournamenthistory(string filename)
{
    TournamentHistory th;
    std::ifstream fs(filename, std::ios::in | std::ios::binary);
    if (!fs.is_open())
    {
        cout << "Failed to open " << filename << endl;
        exit(-1);
    }
    else
    {
        fs.read(reinterpret_cast<char*>(&th.tournament_number), sizeof th.tournament_number);
        fs.read(reinterpret_cast<char*>(&th.random_seed), sizeof th.random_seed);
        fs.read(reinterpret_cast<char*>(&th.initial_player_count), sizeof th.initial_player_count);

        th.finishing_order.resize(th.initial_player_count);
        fs.read(reinterpret_cast<char*>(&th.finishing_order[0]),
            sizeof(th.finishing_order[0]) * th.initial_player_count);

        th.game_eliminated.resize(th.initial_player_count);
        fs.read(reinterpret_cast<char*>(&th.game_eliminated[0]),
            sizeof(th.game_eliminated[0]) * th.initial_player_count);

        fs.read(reinterpret_cast<char*>(&th.num_games), sizeof th.num_games);
        for (size_t i = 0; i < th.num_games; i++)
        {
            th.games.push_back(read_gamehistory(fs));
        }
    }
    fs.close();
    return th;
}

void write_tournamenthistory(string filename, TournamentHistory& th)
{
    std::ofstream fs(filename, std::ios::out | std::ios::binary);
    if (!fs.is_open())
    {
        cout << "Failed to open " << filename << endl;
        exit(-1);
    }
    else
    {
        fs.write(reinterpret_cast<char*>(&th.tournament_number), sizeof th.tournament_number);
        fs.write(reinterpret_cast<char*>(&th.random_seed), sizeof th.random_seed);
        fs.write(reinterpret_cast<char*>(&th.initial_player_count), sizeof th.initial_player_count);

        fs.write(reinterpret_cast<char*>(&th.finishing_order[0]),
            sizeof(th.finishing_order[0]) * th.initial_player_count);

        fs.write(reinterpret_cast<char*>(&th.game_eliminated[0]),
            sizeof(th.game_eliminated[0]) * th.initial_player_count);

        fs.write(reinterpret_cast<char*>(&th.num_games), sizeof th.num_games);
        for (size_t i = 0; i < th.num_games; i++)
        {
            write_gamehistory(fs, th.games[i]);
        }
    }
    fs.close();
}
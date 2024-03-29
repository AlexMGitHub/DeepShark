/******************************************************************************
* Define classes used to serialize and deserialize game states.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <algorithm>    // For std::replace()
#include <chrono>       // For timestamp() function
#include <fstream>
#include <iostream>
#include <iomanip>      // For put_time() in timestamp() function
#include <sstream>      // stringstream in timestamp() function
#include <string>
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"
#include "serialize.hpp"
#include "storage.hpp"
// Using statements
using std::array;
using std::cout;
using std::endl;
using std::replace;
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
        fs.read(reinterpret_cast<char*>(&gs.num_games_per_blind_level), sizeof gs.num_games_per_blind_level);
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
        gs.win_perc.resize(gs.initial_num_players);
        fs.read(reinterpret_cast<char*>(&gs.win_perc[0]),
            sizeof(gs.win_perc[0]) * gs.initial_num_players);

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
        fs.write(reinterpret_cast<char*>(&gs.num_games_per_blind_level), sizeof gs.num_games_per_blind_level);
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
        fs.write(reinterpret_cast<char*>(&gs.win_perc[0]),
            sizeof(gs.win_perc[0]) * gs.initial_num_players);

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

TournamentHistory read_tournamenthistory(string filename, bool summary)
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

        th.player_ai_types.resize(th.initial_player_count);
        fs.read(reinterpret_cast<char*>(&th.player_ai_types[0]),
            sizeof(th.player_ai_types[0]) * th.initial_player_count);

        th.finishing_order.resize(th.initial_player_count);
        fs.read(reinterpret_cast<char*>(&th.finishing_order[0]),
            sizeof(th.finishing_order[0]) * th.initial_player_count);

        th.game_eliminated.resize(th.initial_player_count);
        fs.read(reinterpret_cast<char*>(&th.game_eliminated[0]),
            sizeof(th.game_eliminated[0]) * th.initial_player_count);

        fs.read(reinterpret_cast<char*>(&th.num_games), sizeof th.num_games);

        if (!summary)  // For Python code to get summary of tourament results
        {
            for (size_t i = 0; i < th.num_games; i++)
            {
                th.games.push_back(read_gamehistory(fs));
            }
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

        fs.write(reinterpret_cast<char*>(&th.player_ai_types[0]),
            sizeof(th.player_ai_types[0]) * th.initial_player_count);

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

std::string create_timestamp()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream datetime;
    datetime << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d__%X");
    std::string s = datetime.str();
    replace(s.begin(), s.end(), ':', '-');  // Replace all colons with hypens
    return s;
}

std::string get_date()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream datestamp;
    datestamp << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
    return datestamp.str();
}

void write_nn_vector_data(
    std::string read_filename, std::string write_filename)
{
    vector<double> nn_vector;
    TournamentHistory th = read_tournamenthistory(read_filename);
    std::ofstream fs(write_filename, std::ios::app | std::ios::binary);
    if (!fs.is_open())
    {
        cout << "Failed to open " << write_filename << endl;
        exit(-1);
    }
    else
    {
        for (const auto& gh : th.games)
        {
            for (const auto& gs : gh.states)
            {
                nn_vector = game_state_to_nn_vector(gs);
                fs.write(reinterpret_cast<char*>(&nn_vector[0]),
                    sizeof(nn_vector[0]) * NN_VECTOR_SIZE);
            }
        }
    }
    fs.close();
}

vector<double> game_state_to_nn_vector(GameState gs)
{
    vector<double> nn_vector(NN_VECTOR_SIZE, 0);
    // Legal actions (7 inputs)
    for (const auto& act : gs.legal_actions)
    {
        nn_vector[to_underlying(act) - 1] = 1;
    }
    // Hole Cards (10 inputs)
    nn_vector[7] = to_underlying(gs.hole_cards[gs.player_idx].first.rank) / 14.0;
    int suit_idx = return_suit_one_hot(gs.hole_cards[gs.player_idx].first.suit);
    if (suit_idx != 0) { nn_vector[7 + suit_idx] = 1; }
    nn_vector[12] = to_underlying(gs.hole_cards[gs.player_idx].second.rank) / 14.0;
    suit_idx = return_suit_one_hot(gs.hole_cards[gs.player_idx].second.suit);
    if (suit_idx != 0) { nn_vector[12 + suit_idx] = 1; }
    // Flop cards (15 inputs)
    nn_vector[17] = to_underlying(gs.flop_card1.rank) / 14.0;
    suit_idx = return_suit_one_hot(gs.flop_card1.suit);
    if (suit_idx != 0) { nn_vector[17 + suit_idx] = 1; }
    nn_vector[22] = to_underlying(gs.flop_card2.rank) / 14.0;
    suit_idx = return_suit_one_hot(gs.flop_card2.suit);
    if (suit_idx != 0) { nn_vector[22 + suit_idx] = 1; }
    nn_vector[27] = to_underlying(gs.flop_card3.rank) / 14.0;
    suit_idx = return_suit_one_hot(gs.flop_card3.suit);
    if (suit_idx != 0) { nn_vector[27 + suit_idx] = 1; }
    // Turn card (5 inputs)
    nn_vector[32] = to_underlying(gs.turn_card.rank) / 14.0;
    suit_idx = return_suit_one_hot(gs.turn_card.suit);
    if (suit_idx != 0) { nn_vector[32 + suit_idx] = 1; }
    // River card (5 inputs)
    nn_vector[37] = to_underlying(gs.river_card.rank) / 14.0;
    suit_idx = return_suit_one_hot(gs.river_card.suit);
    if (suit_idx != 0) { nn_vector[37 + suit_idx] = 1; }
    // Chips to call (1 input)
    nn_vector[42] = gs.chips_to_call /
        static_cast<double>(gs.initial_num_players * MAX_BUY_IN);
    // Stack size / max bet (1 input)
    nn_vector[43] = gs.max_bet /
        static_cast<double>(gs.initial_num_players * MAX_BUY_IN);
    // Total amount in pot (1 input)
    nn_vector[44] = gs.pot_chip_count /
        static_cast<double>(gs.initial_num_players * MAX_BUY_IN);
    // Amount in pot from player (1 input)
    nn_vector[45] = get_total_player_bets(gs.player_idx, gs.pot_player_bets) /
        static_cast<double>(gs.initial_num_players * MAX_BUY_IN);
    // Position/blinds (11 inputs)
    int blind_idx = to_underlying(gs.blinds[gs.player_idx]);
    if (blind_idx != 0) { nn_vector[45 + blind_idx] = 1; }
    // Hand rank (1 input)
    nn_vector[57] = to_underlying(gs.hand_ranks[gs.player_idx]) / 11.0;
    // Number of active (unfolded) players remaining (1 input)
    nn_vector[58] = gs.num_active_players /
        static_cast<double>(gs.initial_num_players);
    // Number of players remaining in tournament (1 input)
    nn_vector[59] = gs.num_players /
        static_cast<double>(gs.initial_num_players);
    // One-hot encoding of chosen action (7 outputs)
    nn_vector[60 + to_underlying(gs.player_action) - 1] = 1;
    // Player bet scaled by player's stack size (1 output)
    nn_vector[67] = static_cast<double>(gs.player_bet) / gs.max_bet;
    return nn_vector;
}

int return_suit_one_hot(Suit suit)
{
    for (int i = 0; i < NUMBER_SUITS; i++)
    {
        if (suit == Card_Suits[i])
        {
            return i + 1;
        }
    }
    return 0;  // If Suit is No_Card one-hot will be all zeros
}

unsigned get_total_player_bets(
    int player_idx,
    std::array<
    std::array<unsigned, constants::MAX_BETTING_ROUNDS>, constants::MAX_PLAYER_COUNT>
    m_player_bets
)
{
    /**
     * Return the total number of chips bet by the player in the game so far.
    */
    unsigned total = 0;
    for (int i = 0; i < MAX_BETTING_ROUNDS; i++)
    {
        total += m_player_bets[player_idx][i];
    }
    return total;
}
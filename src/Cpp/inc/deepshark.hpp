#pragma once

/******************************************************************************
* Declare C interface functions between Python and Cpp code.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <stdexcept>
#include <string>
#include <vector>
// Project headers
#include "constants.hpp"

/* Declarations
******************************************************************************/
extern "C"
{
    int add_ints(int x, int y);
    int run_tournaments(
        int num_players,
        int player_ai_types[],
        int num_games_per_blind_level,
        char tourn_directory[],
        int seeds[],
        unsigned num_processors,
        int num_tournaments,
        bool debug = false
    );
    struct tournament_summary
    {
        int tournament_number;
        int random_seed;
        int initial_player_count;
        int* player_ai_types;
        int* finishing_order;
        int* game_eliminated;
        size_t num_games;
    };
    void get_tournament_summary(struct tournament_summary* summary, char filename[]);
}

struct tourn_data
{
    tourn_data(
        int num_players,
        int int_player_ai_types[],
        int num_games_per_blind_level,
        int seed,
        char char_tourn_directory[],
        bool debug) :
        num_players(num_players),
        num_games_per_blind_level(num_games_per_blind_level),
        seed(seed),
        debug(debug)
    {
        tourn_directory = char_tourn_directory;
        for (int i = 0; i < num_players; i++)
        {
            if (int_player_ai_types[i] == 0)
            {
                player_ai_types.push_back(constants::AI_Type::Random);
            }
            else if (int_player_ai_types[i] == 1)
            {
                player_ai_types.push_back(constants::AI_Type::Scripted);
            }
            else if (int_player_ai_types[i] == 2)
            {
                player_ai_types.push_back(constants::AI_Type::CheckCall);
            }
            else if (int_player_ai_types[i] == 3)
            {
                player_ai_types.push_back(constants::AI_Type::Heuristic_TAG);
            }
            else if (int_player_ai_types[i] == 4)
            {
                player_ai_types.push_back(constants::AI_Type::Heuristic_LAG);
            }
            else if (int_player_ai_types[i] == 5)
            {
                player_ai_types.push_back(constants::AI_Type::Heuristic_MTAG);
            }
            else
            {
                throw std::invalid_argument("Invalid AI type!");
            }
        }
    }
    int num_players;
    std::vector<constants::AI_Type> player_ai_types;
    int num_games_per_blind_level;
    int seed;
    std::string tourn_directory;
    bool debug;
};

void run_tournament_thread(int thread_index, tourn_data data);
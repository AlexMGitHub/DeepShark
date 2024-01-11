#pragma once

/******************************************************************************
* Serialize and deserialize states generated during Texas Hold 'Em game play.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// Project headers
#include "constants.hpp"
#include "storage.hpp"

/* Declarations
******************************************************************************/
ShowdownStruct read_showdownstruct(std::ifstream& fs);
void write_showdownstruct(std::ofstream& fs, ShowdownStruct& sd);
GameState read_gamestate(std::ifstream& fs);
void write_gamestate(std::ofstream& fs, GameState& gs);
GameHistory read_gamehistory(std::ifstream& fs);
void write_gamehistory(std::ofstream& fs, GameHistory& gh);
TournamentHistory read_tournamenthistory(
    std::string filename,
    bool summary = false
);
void write_tournamenthistory(std::string filename, TournamentHistory& th);
std::string create_timestamp();
std::string get_date();
void write_nn_vector_data(std::string read_filename, std::string write_filename);
std::vector<double> game_state_to_nn_vector(GameState gs);
int return_suit_one_hot(constants::Suit suit);
unsigned get_total_player_bets(
    int player_idx,
    std::array<
    std::array<unsigned, constants::MAX_BETTING_ROUNDS>, constants::MAX_PLAYER_COUNT>
    m_player_bets
);
#pragma once

/******************************************************************************
* Serialize and deserialize states generated during Texas Hold 'Em game play.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// Project headers
#include "constants.hpp"
#include "storage.hpp"

/* Declarations
******************************************************************************/
TournamentHistory read_tournamenthistory(std::string filename);
GameHistory read_gamehistory(std::ifstream& fs);
GameState read_gamestate(std::ifstream& fs);
ShowdownStruct read_showdownstruct(std::ifstream& fs);
void write_tournamenthistory(std::string filename, TournamentHistory& th);
void write_gamehistory(std::ofstream& fs, GameHistory& gh);
void write_gamestate(std::ofstream& fs, GameState& gs);
void write_showdownstruct(std::ofstream& fs, ShowdownStruct& sd);
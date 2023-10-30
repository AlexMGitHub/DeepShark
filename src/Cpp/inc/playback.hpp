#pragma once

/******************************************************************************
* Playback a text representation of the recorded game states to the terminal.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <deque>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// Project headers
#include "constants.hpp"
#include "storage.hpp"

/* Declarations
******************************************************************************/
void playback_tournament(TournamentHistory& th);
void print_state(const GameState& gh);
void print_console_output(std::deque<std::string>& console_output, const GameState& gs);
std::string to_string(constants::Round& rnd);
std::string to_string(constants::Action& action);
std::string to_string(constants::AI_Type& ai);
std::string to_string(constants::Blind& blind);
std::string to_string(Card& c);
std::string to_string(std::pair<Card, Card>& hole_cards);
std::string to_string(std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand);
std::string calc_pot_per_round(GameState& gs, constants::Round rd);
std::string calc_pot_per_player(GameState& gs, int plyr_idx);
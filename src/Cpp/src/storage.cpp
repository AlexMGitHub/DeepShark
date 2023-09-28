/******************************************************************************
* Define classes used to store records of Texas Hold 'Em games.
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

/* GameState Method Definitions
******************************************************************************/
void GameState::update_state(vector<Player>& players, int plyr_idx, Pot& pot) const
{
    player = players[plyr_idx];
    player_idx = player.player_idx;
    blind = player.blind;
    best_hand = player.get_best_hand();
    available_cards = player.get_available_cards();
    gs.player_chip_counts.clear();
    for (const auto& plyr : players)
    {
        gs.player_chip_counts.push_back(plyr.get_chip_count());
    }
    gs.pot_chip_count = pot.get_chip_count();
}
/******************************************************************************
* Contains functions necessary to implement the Texas Hold 'Em environment.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <algorithm> // For sort()
#include <cassert>
#include <functional> // For std::greater
#include <iostream>
#include <random> // For shuffle()
#include <ranges> // For views::drop()
#include <stdexcept>
#include <string>
#include <utility> // For to_underlying()
// Project headers
#include "cards.hpp"
#include "texasholdem.hpp"
#include "serialize.hpp"
#include "storage.hpp"
#include "player.hpp"
//#include "tabulate/tabulate.hpp"
// Using statements
using std::cout;
using std::endl;
using std::sort;
using std::to_underlying;
using std::vector;
//using namespace tabulate;
//using Row_t = Table::Row_t;
#include <filesystem>
#include "playback.hpp"

int main()
{
    TexasHoldEm the(10, 1, 0, false);
    the.begin_tournament();
    cout << std::filesystem::current_path() << endl;
    TournamentHistory th = read_tournamenthistory("test_game.bin");
    cout << "HERE" << endl;
    playback_tournament(th);
    return 0;
}
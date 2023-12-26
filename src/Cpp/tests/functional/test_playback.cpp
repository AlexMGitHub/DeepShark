/******************************************************************************
* Test functionality of playback and serialization.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <iostream>
#include <string>
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"
#include "playback.hpp"
#include "player.hpp"
#include "serialize.hpp"
#include "texasholdem.hpp"
// Using statements
using std::cout;
using std::endl;
using std::vector;
using namespace constants;

/* Playback Functional Tests
******************************************************************************/
int main()
{
    // THE constructor arguments
    int num_players = 10;
    vector<AI_Type> player_ai_types(num_players, AI_Type::Random);
    int seed = 12;
    int tournament_num = seed;
    bool debug = false;
    // Instantiate THE object and begin tournament
    TexasHoldEm the(player_ai_types, num_players, seed, tournament_num, debug);
    the.begin_tournament();
    // Deserialize and playback recorded game
    //cout << std::filesystem::current_path() << endl;
    TournamentHistory th = read_tournamenthistory(the.filename);
    // TournamentHistory th = read_tournamenthistory("recorded_games/2023-12-25/multithread_testing/tourn_9__2023-12-25__16-02-10.bin");
    //playback_tournament(th);
    return 0;
}
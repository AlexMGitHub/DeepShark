/******************************************************************************
* Test functionality of playback and serialization.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <filesystem>
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
    vector<AI_Type> player_ai_types;
    for (int i = 0; i < 10; i += 2)
    {
        player_ai_types.push_back(AI_Type::Heuristic_TAG);
        player_ai_types.push_back(AI_Type::Heuristic_LAG);
    }
    int seed = 0;
    int tournament_num = 0;
    bool debug = true;
    int num_games_per_blind_level = 20;
    // Instantiate THE object and begin tournament
    TexasHoldEm the(
        player_ai_types,
        num_players,
        seed,
        tournament_num,
        debug,
        num_games_per_blind_level
    );
    the.begin_tournament();
    // Deserialize and playback recorded game
    TournamentHistory th = read_tournamenthistory(the.filename);
    // Uncomment to playback existing recorded game
    // cout << std::filesystem::current_path() << endl;
    // TournamentHistory th = read_tournamenthistory("recorded_games/2023-12-24/tourn_0__2023-12-24__22-07-10.bin");
    playback_tournament(th);
    return 0;
}
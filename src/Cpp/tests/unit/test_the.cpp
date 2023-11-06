/******************************************************************************
* Contains functions necessary to implement the Texas Hold 'Em environment.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <algorithm> // For sort()
#include <array>
#include <cassert>
#include <functional> // For std::greater
#include <iostream>
#include <random> // For shuffle()
#include <ranges> // For views::drop()
#include <stdexcept>
#include <string>
#include <utility> // For to_underlying(), std::pair
// Project headers
#include "cards.hpp"
#include "constants.hpp"
#include "texasholdem.hpp"
#include "serialize.hpp"
#include "storage.hpp"
#include "player.hpp"
//#include "tabulate/tabulate.hpp"
// Using statements
using std::array;
using std::cout;
using std::endl;
using std::pair;
using std::sort;
using std::to_underlying;
using std::vector;
using namespace constants;
//using namespace tabulate;
//using Row_t = Table::Row_t;
#include <filesystem>
#include "playback.hpp"

int main()
{
    int branch = 1;
    if (branch == 0)
    {
        int num_players = 10;
        vector<AI_Type> player_ai_types(num_players, AI_Type::Random);
        int seed = 0;
        int tournament_num = 0;
        bool debug = false;
        TexasHoldEm the(player_ai_types, num_players, seed, tournament_num, debug);
        the.begin_tournament();
        cout << std::filesystem::current_path() << endl;
        TournamentHistory th = read_tournamenthistory("test_game.bin");
        cout << "HERE" << endl;
        playback_tournament(th);
    }
    else
    {
        int num_players = 4;
        vector<AI_Type> player_ai_types(num_players, AI_Type::Scripted);
        int seed = 0;
        int tournament_num = 0;
        bool debug = false;
        TexasHoldEm the(player_ai_types, num_players, seed, tournament_num, debug);
        vector<Card> cards = {
            Card(Suit::Club, Rank::Two),        // Player 0 hole cards
            Card(Suit::Heart, Rank::King),
            Card(Suit::Diamond, Rank::Ace),     // Player 1 hole cards
            Card(Suit::Spade, Rank::Seven),
            Card(Suit::Club, Rank::Eight),      // Player 2 hole cards
            Card(Suit::Spade, Rank::Queen),
            Card(Suit::Heart, Rank::Nine),      // Player 3 hole cards
            Card(Suit::Diamond, Rank::Three),
            Card(),                             // Burn card
            Card(Suit::Club, Rank::Ten),        // Flop Cards
            Card(Suit::Club, Rank::Jack),
            Card(Suit::Club, Rank::Queen),
            Card(),                             // Burn card
            Card(Suit::Club, Rank::King),       // Turn card
            Card(),                             // Burn card
            Card(Suit::Club, Rank::Ace)         // River card
        };
        vector<unsigned> player_chip_counts = {
            BIG_BLIND * 100,                    // Player 0
            BIG_BLIND * 100,                    // Player 1
            BIG_BLIND * 100,                    // Player 2
            BIG_BLIND * 100                     // Player 3
        };
        vector<vector<pair<Action, unsigned>>> scripted_actions;
        vector<pair<Action, unsigned>> player0_actions = {
            std::make_pair(Action::All_In, player_chip_counts[0])
        };
        vector<pair<Action, unsigned>> player1_actions = {
            std::make_pair(Action::All_In, player_chip_counts[1] -
            BLINDS_STRUCTURE.at(4).first)
        };
        vector<pair<Action, unsigned>> player2_actions = {
            std::make_pair(Action::All_In, player_chip_counts[2] -
            BLINDS_STRUCTURE.at(4).second)
        };
        vector<pair<Action, unsigned>> player3_actions = {
            std::make_pair(Action::All_In, player_chip_counts[3])
        };
        scripted_actions.push_back(player0_actions);
        scripted_actions.push_back(player1_actions);
        scripted_actions.push_back(player2_actions);
        scripted_actions.push_back(player3_actions);
        PlayerScript script;
        script.cards = cards;
        script.player_chip_counts = player_chip_counts;
        script.scripted_actions = scripted_actions;
        TestCase tc;
        tc.player_script = script;
        tc.test_desc = "Test a 4-way tie with equal player chip counts...";
        GameState gs;
        gs.num_players = num_players;
        gs.player_chip_counts = vector(num_players, BIG_BLIND * 100);
        gs.num_showdown_players = num_players;
        array<Card, MAX_CARDS_IN_HAND> best_hand = {
            Card(Suit::Club, Rank::Ten),        // Flop Cards
            Card(Suit::Club, Rank::Jack),
            Card(Suit::Club, Rank::Queen),
            Card(Suit::Club, Rank::King),       // Turn card
            Card(Suit::Club, Rank::Ace)
        };
        int num_sd_plyrs = static_cast<int>(gs.num_showdown_players);
        for (int i = 0; i < num_sd_plyrs; i++)
        {
            gs.showdown_players.push_back(
                ShowdownStruct(
                    i,
                    best_hand,
                    HandRank::Royal_Flush,
                    BIG_BLIND * 100,
                    BIG_BLIND * 100
                ));
        }
        tc.ground_truth = gs;
        cout << endl << "Beginning tests...\n" << endl;
        the.begin_test_game(tc);
        cout << endl << "\nAll tests completed successfully!\n" << endl;
    }
    return 0;
}
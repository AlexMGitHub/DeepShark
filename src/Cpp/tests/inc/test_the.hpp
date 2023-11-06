#pragma once

/******************************************************************************
* Store test fixtures for Texas Hold 'Em functional testing.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <string>
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"
#include "player.hpp"
#include "storage.hpp"
#include "texasholdem.hpp"

/* Helper Functions
******************************************************************************/
namespace test_constants
{
    int test_count = 0;
}

unsigned sum_chip_count(const std::vector<unsigned>& plyr_chip_cnts)
{
    unsigned sum = 0;
    for (const auto& chips : plyr_chip_cnts)
    {
        sum += chips;
    }
    return sum;
}

template <typename T>
void execute_test(const T test)
{
    //  Build PlayerScript struct
    PlayerScript script(
        test.test_cards,
        test.player_chip_counts,
        test.scripted_actions
    );
    // Build ground truth
    GameState gs;
    gs.num_players = test.num_players;
    gs.player_chip_counts = test.player_chip_counts;
    gs.num_showdown_players = test.num_players;
    int num_sd_plyrs = static_cast<int>(gs.num_showdown_players);
    for (int i = 0; i < num_sd_plyrs; i++)
    {
        gs.showdown_players.push_back(
            ShowdownStruct(
                test.player_order[i],
                test.best_hands[i],
                test.hand_ranks[i],
                test.chips_bet[i],
                test.chips_won[i]
            ));
    }
    // Build TestCase struct
    TestCase tc(test.test_desc, script, gs);
    // Instantiate THE object
    TexasHoldEm the(
        test.player_ai_types,
        test.num_players,
        test.seed,
        test.tournament_num,
        test.debug
    );
    // Check test fixture for mistakes
    assert((test.test_cards.size() ==
        static_cast<size_t>(test.num_players * 2 + 8)) &&
        "Incorrect number of test cards!");
    assert((test.player_chip_counts.size() ==
        static_cast<size_t>(test.num_players)) &&
        "Incorrect number of player chip counts!");
    assert((test.scripted_actions.size() ==
        static_cast<size_t>(test.num_players)) &&
        "Incorrect number of player scripts!");
    assert((sum_chip_count(test.player_chip_counts) ==
        test.num_players * constants::BIG_BLIND * 100) && "Invalid total chip count!");
    assert((test.player_order.size() == static_cast<size_t>(test.num_players))
        && "Incorrect number of players!");
    assert((test.best_hands.size() == static_cast<size_t>(test.num_players)) &&
        "Incorrect number of best hands!");
    assert((test.hand_ranks.size() == static_cast<size_t>(test.num_players)) &&
        "Incorrect number of hand ranks!");
    assert((test.chips_bet.size() == static_cast<size_t>(test.num_players)) &&
        "Incorrect number of chips bet!");
    assert((test.chips_won.size() == static_cast<size_t>(test.num_players)) &&
        "Incorrect number of chips won!");
    // Execute tests
    the.begin_test_game(tc);
    std::cout << std::endl;
    test_constants::test_count += 3 + test.num_players * 4;
}

/* Test Fixtures
******************************************************************************/
struct test_multi_way_ties_1
{
    // Build TestCase struct
    std::string test_desc = "Test a 4-way tie with equal player chip counts...";
    // THE constructor arguments
    int num_players = 4;
    std::vector<constants::AI_Type> player_ai_types = {
        constants::AI_Type::Scripted,
        constants::AI_Type::Scripted,
        constants::AI_Type::Scripted,
        constants::AI_Type::Scripted
    };
    int seed = 0;
    int tournament_num = 0;
    bool debug = false;
    // Test cards to replace top of deck
    std::vector<Card> test_cards = {
        Card(constants::Suit::Club, constants::Rank::Two),          // Player 0 hole cards
        Card(constants::Suit::Heart, constants::Rank::King),
        Card(constants::Suit::Diamond, constants::Rank::Ace),       // Player 1 hole cards
        Card(constants::Suit::Spade, constants::Rank::Seven),
        Card(constants::Suit::Club, constants::Rank::Eight),        // Player 2 hole cards
        Card(constants::Suit::Spade, constants::Rank::Queen),
        Card(constants::Suit::Heart, constants::Rank::Nine),        // Player 3 hole cards
        Card(constants::Suit::Diamond, constants::Rank::Three),
        Card(),                                                     // Burn card
        Card(constants::Suit::Club, constants::Rank::Ten),          // Flop Cards
        Card(constants::Suit::Club, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::Queen),
        Card(),                                                     // Burn card
        Card(constants::Suit::Club, constants::Rank::King),         // Turn card
        Card(),                                                     // Burn card
        Card(constants::Suit::Club, constants::Rank::Ace)           // River card
    };
    // Player chip counts
    std::vector<unsigned> player_chip_counts = {
        constants::BIG_BLIND * 100,     // Player 0
        constants::BIG_BLIND * 100,     // Player 1
        constants::BIG_BLIND * 100,     // Player 2
        constants::BIG_BLIND * 100      // Player 3
    };
    // Scripted actions
    std::vector<std::vector<std::pair<constants::Action, unsigned>>> scripted_actions =
    {
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[0])
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[1] -
        constants::BLINDS_STRUCTURE.at(num_players).first)
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[2] -
        constants::BLINDS_STRUCTURE.at(num_players).second)
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[3])
    )
    };
    // Build ground truth
    std::vector<int> player_order = { 0, 1, 2, 3 };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand = {
        Card(constants::Suit::Club, constants::Rank::Ten),        // Flop Cards
        Card(constants::Suit::Club, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::Queen),
        Card(constants::Suit::Club, constants::Rank::King),       // Turn card
        Card(constants::Suit::Club, constants::Rank::Ace)
    };
    std::vector<std::array<Card, constants::MAX_CARDS_IN_HAND>> best_hands = {
        best_hand,
        best_hand,
        best_hand,
        best_hand
    };
    std::vector<constants::HandRank> hand_ranks = {
        constants::HandRank::Royal_Flush,
        constants::HandRank::Royal_Flush,
        constants::HandRank::Royal_Flush,
        constants::HandRank::Royal_Flush
    };
    std::vector<unsigned> chips_bet = player_chip_counts;
    std::vector<unsigned> chips_won = player_chip_counts;
};

struct test_multi_way_ties_2
{
    // Build TestCase struct
    std::string test_desc = "Test a 5-way tie with unequal player chip counts...";
    // THE constructor arguments
    int num_players = 5;
    std::vector<constants::AI_Type> player_ai_types = {
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted
    };
    int seed = 0;
    int tournament_num = 0;
    bool debug = false;
    // Test cards to replace top of deck
    std::vector<Card> test_cards = {
        Card(constants::Suit::Club, constants::Rank::Two),          // Player 0 hole cards
        Card(constants::Suit::Heart, constants::Rank::King),
        Card(constants::Suit::Diamond, constants::Rank::Ace),       // Player 1 hole cards
        Card(constants::Suit::Spade, constants::Rank::Seven),
        Card(constants::Suit::Club, constants::Rank::Eight),        // Player 2 hole cards
        Card(constants::Suit::Spade, constants::Rank::Queen),
        Card(constants::Suit::Heart, constants::Rank::Nine),        // Player 3 hole cards
        Card(constants::Suit::Diamond, constants::Rank::Three),
        Card(constants::Suit::Spade, constants::Rank::Nine),        // Player 4 hole cards
        Card(constants::Suit::Spade, constants::Rank::Jack),
        Card(),                                                     // Burn card
        Card(constants::Suit::Club, constants::Rank::Ten),          // Flop Cards
        Card(constants::Suit::Club, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::Queen),
        Card(),                                                     // Burn card
        Card(constants::Suit::Club, constants::Rank::King),         // Turn card
        Card(),                                                     // Burn card
        Card(constants::Suit::Club, constants::Rank::Ace)           // River card
    };
    // Player chip counts
    std::vector<unsigned> player_chip_counts = {
        constants::BIG_BLIND * 100,     // Player 0
        constants::BIG_BLIND * 150,     // Player 1
        constants::BIG_BLIND * 50,      // Player 2
        constants::BIG_BLIND * 75,      // Player 3
        constants::BIG_BLIND * 125      // Player 4
    };
    // Scripted actions
    std::vector<std::vector<std::pair<constants::Action, unsigned>>> scripted_actions =
    {
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[0])
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[1] -
        constants::BLINDS_STRUCTURE.at(num_players).first)
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[2] -
        constants::BLINDS_STRUCTURE.at(num_players).second)
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[3])
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[4])
    )
    };
    // Build ground truth
    std::vector<int> player_order = { 2, 3, 0, 4, 1 };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand = {
        Card(constants::Suit::Club, constants::Rank::Ten),        // Flop Cards
        Card(constants::Suit::Club, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::Queen),
        Card(constants::Suit::Club, constants::Rank::King),       // Turn card
        Card(constants::Suit::Club, constants::Rank::Ace)
    };
    std::vector<std::array<Card, constants::MAX_CARDS_IN_HAND>> best_hands = {
            best_hand,
            best_hand,
            best_hand,
            best_hand,
            best_hand
    };
    std::vector<constants::HandRank> hand_ranks = {
        constants::HandRank::Royal_Flush,
        constants::HandRank::Royal_Flush,
        constants::HandRank::Royal_Flush,
        constants::HandRank::Royal_Flush,
        constants::HandRank::Royal_Flush
    };
    std::vector<unsigned> chips_bet = {
        constants::BIG_BLIND * 50,      // Player 2
        constants::BIG_BLIND * 75,      // Player 3
        constants::BIG_BLIND * 100,     // Player 0
        constants::BIG_BLIND * 125,     // Player 4
        constants::BIG_BLIND * 150      // Player 1
    };
    std::vector<unsigned> chips_won = {
        constants::BIG_BLIND * 50,      // Player 2
        constants::BIG_BLIND * 75,      // Player 3
        constants::BIG_BLIND * 100,     // Player 0
        constants::BIG_BLIND * 125,     // Player 4
        constants::BIG_BLIND * 150      // Player 1
    };
};
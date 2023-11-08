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
    gs.num_showdown_players = test.num_showdown_players;
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
        test.initial_num_players,
        test.seed,
        test.tournament_num,
        test.debug
    );
    // Check test fixture for mistakes
    assert((test.test_cards.size() ==
        static_cast<size_t>(test.initial_num_players * 2 + 8)) &&
        "Incorrect number of test cards!");
    assert((test.player_chip_counts.size() ==
        static_cast<size_t>(test.initial_num_players)) &&
        "Incorrect number of player chip counts!");
    assert((test.scripted_actions.size() ==
        static_cast<size_t>(test.initial_num_players)) &&
        "Incorrect number of player scripts!");
    assert((sum_chip_count(test.player_chip_counts) ==
        test.initial_num_players * constants::BIG_BLIND * 100) && "Invalid total chip count!");
    assert((test.player_order.size() == static_cast<size_t>(test.num_showdown_players))
        && "Incorrect number of players!");
    assert((test.best_hands.size() == static_cast<size_t>(test.num_showdown_players)) &&
        "Incorrect number of best hands!");
    assert((test.hand_ranks.size() == static_cast<size_t>(test.num_showdown_players)) &&
        "Incorrect number of hand ranks!");
    assert((test.chips_bet.size() == static_cast<size_t>(test.num_showdown_players)) &&
        "Incorrect number of chips bet!");
    assert((test.chips_won.size() == static_cast<size_t>(test.num_showdown_players)) &&
        "Incorrect number of chips won!");
    // Execute tests
    the.begin_test_game(tc);
    std::cout << std::endl;
    test_constants::test_count += 3 + test.num_showdown_players * 4;
}

/* Test Fixtures
******************************************************************************/
struct test_multi_way_ties_1
{
    // Build TestCase struct
    std::string test_desc = "Test a 4-way tie with equal player chip counts...";
    // THE constructor arguments
    int initial_num_players = 4;
    int num_showdown_players = 4;
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
        Card(constants::Suit::Diamond, constants::Rank::Ace),       // Player 1 hole card 1
        Card(constants::Suit::Club, constants::Rank::Eight),        // Player 2 hole card 1
        Card(constants::Suit::Heart, constants::Rank::Nine),        // Player 3 hole card 1
        Card(constants::Suit::Club, constants::Rank::Two),          // Player 0 hole card 1 (Dealer)
        Card(constants::Suit::Spade, constants::Rank::Seven),       // Player 1 hole card 2
        Card(constants::Suit::Spade, constants::Rank::Queen),       // Player 2 hole card 2
        Card(constants::Suit::Diamond, constants::Rank::Three),     // Player 3 hole card 2
        Card(constants::Suit::Heart, constants::Rank::King),        // Player 0 hole card 2 (Dealer)
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
        constants::BLINDS_STRUCTURE.at(initial_num_players).first)
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[2] -
        constants::BLINDS_STRUCTURE.at(initial_num_players).second)
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[3])
    )
    };
    // Build ground truth
    int num_players = 4;  // Remaining players at end of game
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
    int initial_num_players = 5;
    int num_showdown_players = 5;
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
        Card(constants::Suit::Diamond, constants::Rank::Ace),       // Player 1 hole card 1
        Card(constants::Suit::Club, constants::Rank::Eight),        // Player 2 hole card 1
        Card(constants::Suit::Heart, constants::Rank::Nine),        // Player 3 hole card 1
        Card(constants::Suit::Spade, constants::Rank::Nine),        // Player 4 hole card 1
        Card(constants::Suit::Club, constants::Rank::Two),          // Player 0 hole card 1 (Dealer)
        Card(constants::Suit::Spade, constants::Rank::Seven),       // Player 1 hole card 2
        Card(constants::Suit::Spade, constants::Rank::Queen),       // Player 2 hole card 2
        Card(constants::Suit::Diamond, constants::Rank::Three),     // Player 3 hole card 2
        Card(constants::Suit::Spade, constants::Rank::Jack),        // Player 4 hole card 2
        Card(constants::Suit::Heart, constants::Rank::King),        // Player 0 hole card 2 (Dealer)
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
        constants::BLINDS_STRUCTURE.at(initial_num_players).first)
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[2] -
        constants::BLINDS_STRUCTURE.at(initial_num_players).second)
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[3])
    ),
    std::vector<std::pair<constants::Action, unsigned>>(1,
        std::make_pair(constants::Action::All_In, player_chip_counts[4])
    )
    };
    // Build ground truth
    int num_players = 5;  // Remaining players at end of game
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

struct test_sidepots_and_ties_1
{
    // Build TestCase struct
    std::string test_desc = "Test three side pots and a tie...";
    // THE constructor arguments
    int initial_num_players = 10;
    int num_showdown_players = 6;
    std::vector<constants::AI_Type> player_ai_types = {
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
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
        Card(constants::Suit::Diamond, constants::Rank::Five),      // Player 1 hole card 1 (two pair)
        Card(constants::Suit::Club, constants::Rank::Eight),        // Player 2 hole card 1 (fold)
        Card(constants::Suit::Heart, constants::Rank::Nine),        // Player 3 hole card 1 (fold)
        Card(constants::Suit::Spade, constants::Rank::Nine),        // Player 4 hole card 1 (three of a kind)
        Card(constants::Suit::Heart, constants::Rank::Seven),       // Player 5 hole card 1 (full house)
        Card(constants::Suit::Heart, constants::Rank::Ten),         // Player 6 hole card 1 (full house)
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Player 7 hole card 1 (two pair)
        Card(constants::Suit::Heart, constants::Rank::Two),         // Player 8 hole card 1 (two pair)
        Card(constants::Suit::Diamond, constants::Rank::Queen),     // Player 9 hole card 1 (straight)
        Card(constants::Suit::Club, constants::Rank::Two),          // Player 0 hole card 1 (fold) (dealer)
        Card(constants::Suit::Spade, constants::Rank::King),        // Player 1 hole card 2 (two pair)
        Card(constants::Suit::Spade, constants::Rank::Queen),       // Player 2 hole card 2 (fold)
        Card(constants::Suit::Diamond, constants::Rank::Three),     // Player 3 hole card 2 (fold)
        Card(constants::Suit::Spade, constants::Rank::Jack),        // Player 4 hole card 2 (three of a kind)
        Card(constants::Suit::Spade, constants::Rank::Seven),       // Player 5 hole card 2 (full house)
        Card(constants::Suit::Heart, constants::Rank::Jack),        // Player 6 hole card 2 (full house)
        Card(constants::Suit::Club, constants::Rank::King),         // Player 7 hole card 2 (two pair)
        Card(constants::Suit::Heart, constants::Rank::Four),        // Player 8 hole card 2 (two pair)
        Card(constants::Suit::Club, constants::Rank::Ace),          // Player 9 hole card 2 (straight)
        Card(constants::Suit::Heart, constants::Rank::King),        // Player 0 hole card 2 (fold) (dealer)
        Card(),                                                     // Burn card
        Card(constants::Suit::Club, constants::Rank::Ten),          // Flop Cards
        Card(constants::Suit::Diamond, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::Seven),
        Card(),                                                     // Burn card
        Card(constants::Suit::Diamond, constants::Rank::King),      // Turn card
        Card(),                                                     // Burn card
        Card(constants::Suit::Club, constants::Rank::Jack)          // River card
    };
    // Player chip counts
    std::vector<unsigned> player_chip_counts = {
        constants::BIG_BLIND * 50,      // Player 0
        constants::BIG_BLIND * 200,     // Player 1
        constants::BIG_BLIND * 50,      // Player 2
        constants::BIG_BLIND * 75,      // Player 3
        constants::BIG_BLIND * 100,     // Player 4
        constants::BIG_BLIND * 50,      // Player 5
        constants::BIG_BLIND * 25,      // Player 6
        constants::BIG_BLIND * 275,     // Player 7
        constants::BIG_BLIND * 100,     // Player 8
        constants::BIG_BLIND * 75,      // Player 9
    };
    // Scripted actions
    std::vector<std::vector<std::pair<constants::Action, unsigned>>> scripted_actions =
    {
    std::vector<std::pair<constants::Action, unsigned>>({       // Player 0
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 2),
        std::make_pair(constants::Action::Fold, 0),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 1
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 2 -
        constants::SMALL_BLIND),
        std::make_pair(constants::Action::Check, 0),
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 48),
        std::make_pair(constants::Action::Bet, constants::BIG_BLIND * 25),
        std::make_pair(constants::Action::Check, 0),
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 50),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 2
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Check, 0),
        std::make_pair(constants::Action::Fold, 0),
        }),

    std::vector<std::pair<constants::Action, unsigned>>(1,      // Player 3
        std::make_pair(constants::Action::Fold, 0)
    ),
    std::vector<std::pair<constants::Action, unsigned>>({       // Player 4
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Bet, constants::BIG_BLIND),
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 47),
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 25),
        std::make_pair(constants::Action::All_In, constants::BIG_BLIND * 25),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 5
        std::make_pair(constants::Action::Raise, constants::BIG_BLIND * 2),
        std::make_pair(constants::Action::All_In, constants::BIG_BLIND * 48),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 6
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 2),
        std::make_pair(constants::Action::All_In, constants::BIG_BLIND * 23),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 7
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 2),
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 48),
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 25),
        std::make_pair(constants::Action::Re_Raise, constants::BIG_BLIND * 50),
        }),

    std::vector<std::pair<constants::Action, unsigned>>(1,      // Player 8
        std::make_pair(constants::Action::Fold, 0)
    ),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 9
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 2),
        std::make_pair(constants::Action::Call, constants::BIG_BLIND * 48),
        std::make_pair(constants::Action::All_In, constants::BIG_BLIND * 25),
        }),
    };

    // Build ground truth
    int num_players = 10;  // Remaining players at end of game
    std::vector<int> player_order = { 6, 5, 9, 4, 1, 7 };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand6 = {   // Player 6
        Card(constants::Suit::Heart, constants::Rank::Ten),
        Card(constants::Suit::Club, constants::Rank::Ten),
        Card(constants::Suit::Heart, constants::Rank::Jack),
        Card(constants::Suit::Diamond, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::Jack)
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand5 = {   // Player 5
        Card(constants::Suit::Heart, constants::Rank::Seven),       // Player 5 hole cards (full house)
        Card(constants::Suit::Spade, constants::Rank::Seven),
        Card(constants::Suit::Club, constants::Rank::Seven),
        Card(constants::Suit::Diamond, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::Jack)
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand9 = {   // Player 9
        Card(constants::Suit::Club, constants::Rank::Ten),          // Flop Cards
        Card(constants::Suit::Diamond, constants::Rank::Jack),
        Card(constants::Suit::Diamond, constants::Rank::Queen),     // Player 9 hole cards (straight)    
        Card(constants::Suit::Diamond, constants::Rank::King),
        Card(constants::Suit::Club, constants::Rank::Ace),
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand4 = {   // Player 4
        Card(constants::Suit::Club, constants::Rank::Ten),          // Flop Cards
        Card(constants::Suit::Spade, constants::Rank::Jack),
        Card(constants::Suit::Diamond, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::Jack),
        Card(constants::Suit::Diamond, constants::Rank::King),      // Turn card
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand1 = {   // Player 1
        Card(constants::Suit::Club, constants::Rank::Ten),          // Flop Cards
        Card(constants::Suit::Diamond, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::Jack),
        Card(constants::Suit::Spade, constants::Rank::King),
        Card(constants::Suit::Diamond, constants::Rank::King),      // Turn card
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand7 = {   // Player 7
        Card(constants::Suit::Club, constants::Rank::Ten),          // Flop Cards
        Card(constants::Suit::Diamond, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::Jack),
        Card(constants::Suit::Club, constants::Rank::King),
        Card(constants::Suit::Diamond, constants::Rank::King),      // Turn card
    };
    std::vector<std::array<Card, constants::MAX_CARDS_IN_HAND>> best_hands = {
            best_hand6,
            best_hand5,
            best_hand9,
            best_hand4,
            best_hand1,
            best_hand7
    };
    std::vector<constants::HandRank> hand_ranks = {
        constants::HandRank::Full_House,
        constants::HandRank::Full_House,
        constants::HandRank::Straight,
        constants::HandRank::Three_of_a_Kind,
        constants::HandRank::Two_Pair,
        constants::HandRank::Two_Pair
    };
    std::vector<unsigned> chips_bet = {
        1250,   // Player 6
        2500,   // Player 5
        3750,   // Player 9
        5000,   // Player 4
        6250,   // Player 1
        6250    // Player 7
    };
    std::vector<unsigned> chips_won = {
        7700,     // Player 6
        6250,     // Player 5
        5000,     // Player 9
        3750,     // Player 4
        1250,     // Player 1
        1250      // Player 7
    };
};

struct test_sidepots_and_ties_2
{
    // Build TestCase struct
    std::string test_desc = "Test side pots, a tie, and an eliminated player...";
    // THE constructor arguments
    int initial_num_players = 10;
    int num_showdown_players = 6;
    std::vector<constants::AI_Type> player_ai_types = {
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
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
        Card(constants::Suit::Diamond, constants::Rank::King),      // Player 1 hole card 1 (pair)
        Card(constants::Suit::Club, constants::Rank::Eight),        // Player 2 hole card 1 (fold)
        Card(constants::Suit::Heart, constants::Rank::Five),        // Player 3 hole card 1 (two pair)
        Card(constants::Suit::Spade, constants::Rank::Five),        // Player 4 hole card 1 (two pair)
        Card(constants::Suit::Heart, constants::Rank::Seven),       // Player 5 hole card 1 (fold)
        Card(constants::Suit::Club, constants::Rank::Ten),          // Player 6 hole card 1 (flush)
        Card(constants::Suit::Diamond, constants::Rank::Ace),       // Player 7 hole card 1 (pair)
        Card(constants::Suit::Diamond, constants::Rank::Five),      // Player 8 hole card 1 (two pair)
        Card(constants::Suit::Diamond, constants::Rank::Queen),     // Player 9 hole card 1 (fold)
        Card(constants::Suit::Club, constants::Rank::Two),          // Player 0 hole card 1 (fold) (dealer)
        Card(constants::Suit::Spade, constants::Rank::Three),       // Player 1 hole card 2 (pair)
        Card(constants::Suit::Spade, constants::Rank::Queen),       // Player 2 hole card 2 (fold)
        Card(constants::Suit::Diamond, constants::Rank::Six),       // Player 3 hole card 2 (two pair)
        Card(constants::Suit::Spade, constants::Rank::Six),         // Player 4 hole card 2 (two pair)
        Card(constants::Suit::Spade, constants::Rank::Seven),       // Player 5 hole card 2 (fold)
        Card(constants::Suit::Club, constants::Rank::Jack),         // Player 6 hole card 2 (flush)
        Card(constants::Suit::Club, constants::Rank::King),         // Player 7 hole card 2 (pair)
        Card(constants::Suit::Heart, constants::Rank::Six),         // Player 8 hole card 2 (two pair)
        Card(constants::Suit::Club, constants::Rank::Ace),          // Player 9 hole card 2 (fold)
        Card(constants::Suit::Heart, constants::Rank::King),        // Player 0 hole card 2 (fold) (dealer)
        Card(),                                                     // Burn card
        Card(constants::Suit::Club, constants::Rank::Two),          // Flop Cards
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(),                                                     // Burn card
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(),                                                     // Burn card
        Card(constants::Suit::Heart, constants::Rank::Jack)         // River card
    };
    // Player chip counts
    std::vector<unsigned> player_chip_counts = {
        constants::BIG_BLIND * 50,      // Player 0
        constants::BIG_BLIND * 200,     // Player 1
        constants::BIG_BLIND * 50,      // Player 2
        constants::BIG_BLIND * 75,      // Player 3
        constants::BIG_BLIND * 100,     // Player 4
        constants::BIG_BLIND * 50,      // Player 5
        constants::BIG_BLIND * 25,      // Player 6
        constants::BIG_BLIND * 275,     // Player 7
        constants::BIG_BLIND * 100,     // Player 8
        constants::BIG_BLIND * 75,      // Player 9
    };
    // Scripted actions
    std::vector<std::vector<std::pair<constants::Action, unsigned>>> scripted_actions =
    {
    std::vector<std::pair<constants::Action, unsigned>>({       // Player 0
        std::make_pair(constants::Action::Fold, 0),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 1
        std::make_pair(constants::Action::Call, 475),
        std::make_pair(constants::Action::Bet, 1000),
        std::make_pair(constants::Action::Bet, 2000),
        std::make_pair(constants::Action::Call, 1500),
        std::make_pair(constants::Action::Bet, 2000),
        std::make_pair(constants::Action::All_In, 3000),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 2
        std::make_pair(constants::Action::Fold, 0),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 3
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Call, 450),
        std::make_pair(constants::Action::Call, 1000),
        std::make_pair(constants::Action::All_In, 2250),
        }),
    std::vector<std::pair<constants::Action, unsigned>>({       // Player 4
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Call, 450),
        std::make_pair(constants::Action::Call, 1000),
        std::make_pair(constants::Action::All_In, 3500),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 5
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Fold, 0),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 6
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Call, 450),
        std::make_pair(constants::Action::All_In, 750),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 7
        std::make_pair(constants::Action::Raise, 500),
        std::make_pair(constants::Action::Call, 1000),
        std::make_pair(constants::Action::Call, 3500),
        std::make_pair(constants::Action::Raise, 5000),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 8
        std::make_pair(constants::Action::Call, 500),
        std::make_pair(constants::Action::Call, 1000),
        std::make_pair(constants::Action::All_In, 3500),
    }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 9
        std::make_pair(constants::Action::Fold, 0),
        }),
    };

    // Build ground truth
    int num_players = 9;  // Remaining players at end of game
    std::vector<int> player_order = { 6, 3, 4, 8, 7, 1 };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand6 = {   // Player 6 flush
        Card(constants::Suit::Club, constants::Rank::Two),          // Flop Cards
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Club, constants::Rank::Ten),          // Player 6 hole card 1 (flush)
        Card(constants::Suit::Club, constants::Rank::Jack),         // Player 6 hole card 2 (flush)
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand3 = {   // Player 3 Two pair
        Card(constants::Suit::Heart, constants::Rank::Five),        // Player 3 hole card 1 (two pair)
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(constants::Suit::Heart, constants::Rank::Jack)         // River card
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand4 = {   // Player 4
        Card(constants::Suit::Spade, constants::Rank::Five),        // Player 4 hole card 1 (two pair)
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(constants::Suit::Heart, constants::Rank::Jack)         // River card
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand8 = {   // Player 8
        Card(constants::Suit::Diamond, constants::Rank::Five),      // Player 8 hole card 1 (two pair)
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(constants::Suit::Heart, constants::Rank::Jack)         // River card
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand7 = {   // Player 7
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(constants::Suit::Heart, constants::Rank::Jack),         // River card
        Card(constants::Suit::Club, constants::Rank::King),         // Player 7 hole card 2 (pair)
        Card(constants::Suit::Diamond, constants::Rank::Ace),       // Player 7 hole card 1 (pair)
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand1 = {   // Player 1
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(constants::Suit::Heart, constants::Rank::Jack),         // River card
        Card(constants::Suit::Diamond, constants::Rank::King),      // Player 1 hole card 1 (pair)
    };
    std::vector<std::array<Card, constants::MAX_CARDS_IN_HAND>> best_hands = {
            best_hand6,
            best_hand3,
            best_hand4,
            best_hand8,
            best_hand7,
            best_hand1
    };
    std::vector<constants::HandRank> hand_ranks = {
        constants::HandRank::Flush,
        constants::HandRank::Two_Pair,
        constants::HandRank::Two_Pair,
        constants::HandRank::Two_Pair,
        constants::HandRank::Pair,
        constants::HandRank::Pair
    };
    std::vector<unsigned> chips_bet = {
        1250,   // Player 6
        3750,   // Player 3
        5000,   // Player 4
        5000,   // Player 8
        10000,  // Player 7
        10000   // Player 1
    };
    std::vector<unsigned> chips_won = {
        7600,     // Player 6
        4168,     // Player 3
        2500,     // Player 4
        2500,     // Player 8
        10000,    // Player 7
        0         // Player 1
    };
};

struct test_sidepots_and_ties_3
{
    // Build TestCase struct
    std::string test_desc = "Test side pots and two ties...";
    // THE constructor arguments
    int initial_num_players = 10;
    int num_showdown_players = 6;
    std::vector<constants::AI_Type> player_ai_types = {
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
    constants::AI_Type::Scripted,
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
        Card(constants::Suit::Diamond, constants::Rank::King),      // Player 1 hole card 1 (pair)
        Card(constants::Suit::Club, constants::Rank::Eight),        // Player 2 hole card 1 (fold)
        Card(constants::Suit::Heart, constants::Rank::Five),        // Player 3 hole card 1 (two pair)
        Card(constants::Suit::Spade, constants::Rank::Five),        // Player 4 hole card 1 (two pair)
        Card(constants::Suit::Heart, constants::Rank::Seven),       // Player 5 hole card 1 (fold)
        Card(constants::Suit::Club, constants::Rank::Ten),          // Player 6 hole card 1 (flush)
        Card(constants::Suit::Diamond, constants::Rank::Ace),       // Player 7 hole card 1 (pair)
        Card(constants::Suit::Diamond, constants::Rank::Five),      // Player 8 hole card 1 (two pair)
        Card(constants::Suit::Diamond, constants::Rank::Queen),     // Player 9 hole card 1 (fold)
        Card(constants::Suit::Club, constants::Rank::Two),          // Player 0 hole card 1 (fold) (dealer)
        Card(constants::Suit::Heart, constants::Rank::Ace),         // Player 1 hole card 2 (pair)
        Card(constants::Suit::Spade, constants::Rank::Queen),       // Player 2 hole card 2 (fold)
        Card(constants::Suit::Diamond, constants::Rank::Six),       // Player 3 hole card 2 (two pair)
        Card(constants::Suit::Spade, constants::Rank::Six),         // Player 4 hole card 2 (two pair)
        Card(constants::Suit::Spade, constants::Rank::Seven),       // Player 5 hole card 2 (fold)
        Card(constants::Suit::Club, constants::Rank::Jack),         // Player 6 hole card 2 (flush)
        Card(constants::Suit::Club, constants::Rank::King),         // Player 7 hole card 2 (pair)
        Card(constants::Suit::Heart, constants::Rank::Six),         // Player 8 hole card 2 (two pair)
        Card(constants::Suit::Club, constants::Rank::Ace),          // Player 9 hole card 2 (fold)
        Card(constants::Suit::Heart, constants::Rank::King),        // Player 0 hole card 2 (fold) (dealer)
        Card(),                                                     // Burn card
        Card(constants::Suit::Club, constants::Rank::Two),          // Flop Cards
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(),                                                     // Burn card
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(),                                                     // Burn card
        Card(constants::Suit::Heart, constants::Rank::Jack)         // River card
    };
    // Player chip counts
    std::vector<unsigned> player_chip_counts = {
        constants::BIG_BLIND * 50,      // Player 0
        constants::BIG_BLIND * 200,     // Player 1
        constants::BIG_BLIND * 50,      // Player 2
        constants::BIG_BLIND * 75,      // Player 3
        constants::BIG_BLIND * 100,     // Player 4
        constants::BIG_BLIND * 50,      // Player 5
        constants::BIG_BLIND * 25,      // Player 6
        constants::BIG_BLIND * 275,     // Player 7
        constants::BIG_BLIND * 100,     // Player 8
        constants::BIG_BLIND * 75,      // Player 9
    };
    // Scripted actions
    std::vector<std::vector<std::pair<constants::Action, unsigned>>> scripted_actions =
    {
    std::vector<std::pair<constants::Action, unsigned>>({       // Player 0
        std::make_pair(constants::Action::Fold, 0),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 1
        std::make_pair(constants::Action::Call, 475),
        std::make_pair(constants::Action::Bet, 1000),
        std::make_pair(constants::Action::Bet, 2000),
        std::make_pair(constants::Action::Call, 1500),
        std::make_pair(constants::Action::Bet, 2000),
        std::make_pair(constants::Action::All_In, 3000),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 2
        std::make_pair(constants::Action::Fold, 0),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 3
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Call, 450),
        std::make_pair(constants::Action::Call, 1000),
        std::make_pair(constants::Action::All_In, 2250),
        }),
    std::vector<std::pair<constants::Action, unsigned>>({       // Player 4
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Call, 450),
        std::make_pair(constants::Action::Call, 1000),
        std::make_pair(constants::Action::All_In, 3500),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 5
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Fold, 0),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 6
        std::make_pair(constants::Action::Call, constants::BIG_BLIND),
        std::make_pair(constants::Action::Call, 450),
        std::make_pair(constants::Action::All_In, 750),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 7
        std::make_pair(constants::Action::Raise, 500),
        std::make_pair(constants::Action::Call, 1000),
        std::make_pair(constants::Action::Call, 3500),
        std::make_pair(constants::Action::Raise, 5000),
        }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 8
        std::make_pair(constants::Action::Call, 500),
        std::make_pair(constants::Action::Call, 1000),
        std::make_pair(constants::Action::All_In, 3500),
    }),

    std::vector<std::pair<constants::Action, unsigned>>({       // Player 9
        std::make_pair(constants::Action::Fold, 0),
        }),
    };

    // Build ground truth
    int num_players = 10;  // Remaining players at end of game
    std::vector<int> player_order = { 6, 3, 4, 8, 1, 7 };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand6 = {   // Player 6 flush
        Card(constants::Suit::Club, constants::Rank::Two),          // Flop Cards
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Club, constants::Rank::Ten),          // Player 6 hole card 1 (flush)
        Card(constants::Suit::Club, constants::Rank::Jack),         // Player 6 hole card 2 (flush)
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand3 = {   // Player 3 Two pair
        Card(constants::Suit::Heart, constants::Rank::Five),        // Player 3 hole card 1 (two pair)
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(constants::Suit::Heart, constants::Rank::Jack)         // River card
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand4 = {   // Player 4
        Card(constants::Suit::Spade, constants::Rank::Five),        // Player 4 hole card 1 (two pair)
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(constants::Suit::Heart, constants::Rank::Jack)         // River card
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand8 = {   // Player 8
        Card(constants::Suit::Diamond, constants::Rank::Five),      // Player 8 hole card 1 (two pair)
        Card(constants::Suit::Club, constants::Rank::Five),
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(constants::Suit::Heart, constants::Rank::Jack)         // River card
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand7 = {   // Player 7
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(constants::Suit::Heart, constants::Rank::Jack),         // River card
        Card(constants::Suit::Club, constants::Rank::King),         // Player 7 hole card 2 (pair)
        Card(constants::Suit::Diamond, constants::Rank::Ace),       // Player 7 hole card 1 (pair)
    };
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand1 = {   // Player 1
        Card(constants::Suit::Club, constants::Rank::Nine),
        Card(constants::Suit::Diamond, constants::Rank::Nine),      // Turn card
        Card(constants::Suit::Heart, constants::Rank::Jack),         // River card
        Card(constants::Suit::Diamond, constants::Rank::King),      // Player 1 hole card 1 (pair)
        Card(constants::Suit::Heart, constants::Rank::Ace),         // Player 1 hole card 2 (pair)
    };
    std::vector<std::array<Card, constants::MAX_CARDS_IN_HAND>> best_hands = {
            best_hand6,
            best_hand3,
            best_hand4,
            best_hand8,
            best_hand1,
            best_hand7
    };
    std::vector<constants::HandRank> hand_ranks = {
        constants::HandRank::Flush,
        constants::HandRank::Two_Pair,
        constants::HandRank::Two_Pair,
        constants::HandRank::Two_Pair,
        constants::HandRank::Pair,
        constants::HandRank::Pair
    };
    std::vector<unsigned> chips_bet = {
        1250,   // Player 6
        3750,   // Player 3
        5000,   // Player 4
        5000,   // Player 8
        10000,  // Player 7
        10000   // Player 1
    };
    std::vector<unsigned> chips_won = {
        7600,    // Player 6
        4168,    // Player 3
        2500,    // Player 4
        2500,    // Player 8
        5000,    // Player 7
        5000     // Player 1
    };
};
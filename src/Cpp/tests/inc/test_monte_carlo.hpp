#pragma once

/******************************************************************************
* Store test fixtures for Texas Hold 'Em Monte Carlo testing.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <cassert>
#include <chrono>       // For high_resolution_clock
#include <iostream>
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
std::string card_to_string(const Card& c)
{
    std::ostringstream ss;
    ss << c;
    return ss.str();
}

template <typename T>
void execute_test(const T test)
{
    std::vector<constants::AI_Type> player_ai_types;
    for (int i = 0; i < test.num_players; i++)
    {
        player_ai_types.push_back(constants::AI_Type::CheckCall);
    }
    TexasHoldEm the(
        player_ai_types,
        test.num_players,
        test.seed,
        0,
        false
    );
    // Time execution of test
    auto t1 = std::chrono::high_resolution_clock::now();
    the.begin_mc_game(test.starting_hand, test.num_runs);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    // Print output
    std::cout << "\n\n" << test.test_desc << std::endl;
    std::cout << "\t- Number of players:\t\t" << test.num_players << std::endl;
    std::cout << "\t- Starting hand:\t\t";
    for (const auto& card : test.starting_hand)
    {
        std::cout << card_to_string(card) << " ";
    }
    std::cout << std::endl;
    std::cout << "\t- Number games played:\t\t" << the.mc_total_runs << std::endl;
    std::cout << "\t- Number games won:\t\t" << the.mc_run_wins << std::endl;
    std::cout << std::fixed;
    std::cout << std::setprecision(2);
    double prob = static_cast<double>(the.mc_run_wins) /
        static_cast<double>(the.mc_total_runs) * 100;
    std::cout << "\t- Probability of win:\t\t" << prob << "%" << std::endl;
    std::cout << "\t- Execution time:\t\t" << ms_double.count() << " ms\n";
    if (test.min_prob >= 0)
    {
        assert((prob >= test.min_prob) && "Simulated probability too low!");
    }
    if (test.max_prob >= 0)
    {
        assert((prob <= test.max_prob) && "Simulated probability too high!");
    }
}

/* Test Fixtures
******************************************************************************/
struct test_pocket_cards_struct
{
    // Constructor
    test_pocket_cards_struct(
        int num_players,
        std::string test_desc,
        Card c1,
        Card c2,
        size_t num_runs = 1000,
        int seed = 0) :
        num_players(num_players),
        test_desc(test_desc),
        c1(c1),
        c2(c2),
        num_runs(num_runs),
        seed(seed)
    {
        starting_hand.push_back(c1);
        starting_hand.push_back(c2);
    }
    // Data Members
    int num_players = 2;
    std::string test_desc;
    // Hole cards
    Card c1;
    Card c2;
    // Flop cards
    // Turn Card
    // River Card
    size_t num_runs = 1000;
    int seed = 0;
    std::vector<Card> starting_hand;
    double max_prob = -1;
    double min_prob = -1;
};

struct test_flop_4player1
{
    std::string test_desc = "test_flop_4player1...";
    int seed = 0;
    int num_players = 4;
    size_t num_runs = 1000;
    std::vector<Card> starting_hand = {
    // Hole cards
    Card(constants::Suit::Spade, constants::Rank::Ace),
    Card(constants::Suit::Spade, constants::Rank::King),
    // Flop cards
    Card(constants::Suit::Diamond, constants::Rank::Nine),
    Card(constants::Suit::Spade, constants::Rank::Seven),
    Card(constants::Suit::Club, constants::Rank::Jack),
    // Turn Card
    // River Card
    };
    double max_prob = -1;
    double min_prob = -1;
};

struct test_flop_4player2
{
    std::string test_desc = "test_flop_4player2...";
    int seed = 0;
    int num_players = 4;
    size_t num_runs = 1000;
    std::vector<Card> starting_hand = {
    // Hole cards
    Card(constants::Suit::Spade, constants::Rank::Ace),
    Card(constants::Suit::Spade, constants::Rank::King),
    // Flop cards
    Card(constants::Suit::Diamond, constants::Rank::Nine),
    Card(constants::Suit::Spade, constants::Rank::Seven),
    Card(constants::Suit::Club, constants::Rank::King),
    // Turn Card
    // River Card
    };
    double max_prob = -1;
    double min_prob = -1;
};

struct test_royal_flush_10player
{
    std::string test_desc = "test_royal_flush_10player...";
    int seed = 0;
    int num_players = 10;
    size_t num_runs = 1000;
    std::vector<Card> starting_hand = {
    // Hole cards
    Card(constants::Suit::Spade, constants::Rank::Ace),
    Card(constants::Suit::Spade, constants::Rank::King),
    // Flop cards
    Card(constants::Suit::Spade, constants::Rank::Ten),
    Card(constants::Suit::Spade, constants::Rank::Queen),
    Card(constants::Suit::Spade, constants::Rank::Jack),
    // Turn Card
    // River Card
    };
    double max_prob = 100;
    double min_prob = 100;
};

struct test_straight_draw_4player_1000runs
{
    std::string test_desc = "test_straight_draw_4player_1000runs...";
    int seed = 0;
    int num_players = 4;
    size_t num_runs = 1000;
    std::vector<Card> starting_hand = {
    // Hole cards
    Card(constants::Suit::Spade, constants::Rank::Jack),
    Card(constants::Suit::Spade, constants::Rank::Queen),
    // Flop cards
    Card(constants::Suit::Diamond, constants::Rank::Ten),
    Card(constants::Suit::Spade, constants::Rank::Seven),
    Card(constants::Suit::Club, constants::Rank::King),
    // Turn Card
    // River Card
    };
    double max_prob = -1;
    double min_prob = -1;
};

struct test_straight_draw_4player_10000runs
{
    std::string test_desc = "test_straight_draw_4player_10000runs...";
    int seed = 0;
    int num_players = 4;
    size_t num_runs = 10000;
    std::vector<Card> starting_hand = {
    // Hole cards
    Card(constants::Suit::Spade, constants::Rank::Jack),
    Card(constants::Suit::Spade, constants::Rank::Queen),
    // Flop cards
    Card(constants::Suit::Diamond, constants::Rank::Ten),
    Card(constants::Suit::Spade, constants::Rank::Seven),
    Card(constants::Suit::Club, constants::Rank::King),
    // Turn Card
    // River Card
    };
    double max_prob = -1;
    double min_prob = -1;
};
#pragma once

/******************************************************************************
* Declare classes used to store records of Texas Hold 'Em games.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <vector>
// Project headers
#include "constants.hpp"

/* Forward Declarations
******************************************************************************/
struct Card;
class Player;
class Pot;

/* Declarations
******************************************************************************/
struct ShowdownStruct
{
    // Constructors
    ShowdownStruct() {}  // Default constructor needed for serialize
    ShowdownStruct(
        int idx,
        std::array<Card, constants::MAX_CARDS_IN_HAND> sh,
        constants::HandRank hr,
        unsigned chips_bet,
        unsigned chips_won = 0) :
        player_idx(idx),
        best_hand(sh),
        hand_rank(hr),
        total_chips_bet(chips_bet),
        chips_won(chips_won) {}
    // Data Members
    int player_idx;
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand;
    constants::HandRank hand_rank;
    unsigned total_chips_bet;
    unsigned chips_won;
    // Member Functions
    // Const required when using 3-paremeter version of std::sort
    bool operator>(const ShowdownStruct& other) const;
    bool operator==(const ShowdownStruct& other);
    constants::Rank identify_card_groups(
        const std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand,
        int group_size,
        constants::Rank ignore_rank = constants::Rank::No_Card
    ) const;
    std::vector<constants::Rank> identify_kickers(
        const std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand
    ) const;
};

struct GameState
{
    // Data Members
    int random_seed;
    int tournament_number;
    int game_number;
    int initial_num_players;
    int num_players;
    int num_active_players;
    int action_number;
    constants::Round round;
    // Player info
    int player_idx;
    std::vector<constants::Blind> blinds;
    std::vector<constants::Action> last_actions;
    std::vector<constants::AI_Type> ai_types;
    std::vector<std::pair<Card, Card>> hole_cards;
    std::vector<std::array<Card, constants::MAX_CARDS_IN_HAND>> best_hands;
    std::vector<constants::HandRank> hand_ranks;
    std::vector<int> remaining_players;  // Vector bool is broken
    // Before action
    unsigned sum_prev_bets;
    bool legal_to_raise;
    bool raise_active;
    bool all_in_below_min_raise;
    int raise_player_idx;
    unsigned pot_chip_count;
    unsigned chips_to_call;
    unsigned min_bet;
    unsigned max_bet;
    unsigned min_to_raise;
    Card flop_card1;
    Card flop_card2;
    Card flop_card3;
    Card turn_card;
    Card river_card;
    // Vectors and arrays
    std::array<
        std::array<unsigned, constants::MAX_BETTING_ROUNDS>, constants::MAX_PLAYER_COUNT>
        pot_player_bets = { 0 };
    std::vector<unsigned> player_chip_counts;
    std::vector<int> active_player_list;
    std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand;
    size_t num_available_cards;
    std::vector<Card> available_cards;
    size_t num_legal_actions;
    std::vector<constants::Action> legal_actions;
    // After action
    constants::Action player_action;
    unsigned player_bet;
    // After Showdown
    size_t num_showdown_players;
    std::vector<ShowdownStruct> showdown_players;
};

struct GameHistory
{
    int game_number;
    size_t num_states;
    std::vector<GameState> states;
};

struct TournamentHistory
{
    // Constructors
    TournamentHistory() {} // Default constructor for serialize
    TournamentHistory(
        int tourn_num,
        int seed,
        int plyr_cnt) :
        tournament_number(tourn_num),
        random_seed(seed),
        initial_player_count(plyr_cnt) {}
    // Data Members
    int tournament_number;
    int random_seed;
    int initial_player_count;
    std::vector<int> finishing_order;
    std::vector<int> game_eliminated;
    size_t num_games;
    std::vector<GameHistory> games;
};

class RunTournaments
{
    int initial_player_count;
    // Constructor needs AIs
    int num_tournaments;
    std::vector<int> random_seeds;
    std::vector<TournamentHistory> tournaments;
    // Methods to run tournaments in parallel
};

struct PlayerScript
{
    // Constructors
    PlayerScript() {}
    PlayerScript(
        std::vector<Card> test_cards,
        std::vector<unsigned> player_chip_counts,
        std::vector<std::vector<std::pair<constants::Action, unsigned>>>
        scripted_actions) :
        test_cards(test_cards),
        player_chip_counts(player_chip_counts),
        scripted_actions(scripted_actions) {}
    // Data Members
    std::vector<Card> test_cards;
    std::vector<unsigned> player_chip_counts;
    std::vector<
        std::vector<
        std::pair<constants::Action, unsigned>>
        > scripted_actions;
};

struct TestCase
{
    // Constructors
    TestCase() {}
    TestCase(
        std::string test_desc,
        PlayerScript player_script,
        GameState ground_truth) :
        test_desc(test_desc),
        player_script(player_script),
        ground_truth(ground_truth) {}
    // Data Members
    std::string test_desc;
    PlayerScript player_script;
    GameState ground_truth;
    // Member Functions
    void validate_results(GameState& gs);
};


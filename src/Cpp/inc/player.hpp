#pragma once

/******************************************************************************
* Declare classes representing a Texas Hold 'Em player.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <random> // For mt19937 and normal distribution
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"

/* Forward Declarations
******************************************************************************/
struct GameState;

/* Declarations
******************************************************************************/
class PlayerAI
{
public:
    // Constructors
    PlayerAI(constants::AI_Type ai, std::mt19937& rng) : ai(ai), rng(rng) {}
    // Data Members
    const constants::AI_Type ai;
    std::mt19937& rng;
    std::uniform_int_distribution<> uni_dist;
    // Member Functions
    void player_act(GameState& gs);
    static bool legal_act(constants::Action act, GameState& gs);
private:
    // Member Functions
    void random_act(GameState& gs);
    void set_int_dist(int max);
    int dist_max;
};

class Player
{
public:
    // Constructors
    Player(int idx, unsigned chips, constants::AI_Type ai, std::mt19937& rng) :
        player_idx(idx),
        ai_type(ai),
        m_ai(ai, rng),
        m_chip_count(chips) {}
    // Data Members
    const int player_idx;
    constants::AI_Type ai_type;
    constants::Blind blind = constants::Blind::No_Blind;
    constants::Action prev_action = constants::Action::No_Action;
    // Member Functions
    void eliminate_player();
    void fold_player();
    constants::Blind get_blind_status() const;
    std::pair<Card, Card> get_hole_cards();
    std::vector<Card> get_available_cards();
    std::array<Card, constants::MAX_CARDS_IN_HAND> get_best_hand();
    constants::HandRank get_best_hand_rank();
    size_t get_card_count() const;
    unsigned get_chip_count() const;
    unsigned pay_blind(unsigned chips);
    void player_act(GameState& gs);
    bool is_player_active() const;
    bool is_player_eliminated() const;
    void receive_card(const Card& c);
    void return_cards();
    void set_player_active();
    void update_blind_status(constants::Blind b);
    void win_chips(unsigned chips);
private:
    // Data Members
    PlayerAI m_ai;
    Hand m_hand;
    bool m_active = true;
    bool m_eliminated = false;
    unsigned m_chip_count;
    // Member Functions
    unsigned m_push_chips_to_pot(unsigned chips);
};
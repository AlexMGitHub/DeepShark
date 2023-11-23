#pragma once

/******************************************************************************
* Declare classes representing a Texas Hold 'Em player.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <memory>   // For shared_ptr
#include <random>   // For mt19937 and normal distribution
#include <utility>  // For std::pair
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"

/* Forward Declarations
******************************************************************************/
struct GameState;
class TexasHoldEm;

/* AI Declarations
******************************************************************************/
class PlayerAI
{
public:
    // Constructors
    PlayerAI(std::mt19937& rng) : rng(rng) {}
    // Data Members
    std::mt19937& rng;
    // Member Functions
    virtual void player_act(GameState& gs) = 0;
    static bool legal_act(constants::Action act, GameState& gs);
};

class RandomAI : public PlayerAI
{
public:
    // Constructors
    RandomAI(std::mt19937& rng) : PlayerAI(rng) {}
    // Data Members
    const constants::AI_Type ai = constants::AI_Type::Random;
    // Member Functions
    void player_act(GameState& gs) override;
private:
    // Data Members
    int m_distribution_max;
    std::uniform_int_distribution<> m_uniform_dist;
    // Member Functions
    void m_set_distribution_max(int max);
};

class ScriptedAI : public PlayerAI
{
public:
    // Constructors
    ScriptedAI(std::mt19937& rng) : PlayerAI(rng) {}
    // Data Members
    const constants::AI_Type ai = constants::AI_Type::Scripted;
    std::vector<std::pair<constants::Action, unsigned>> scripted_actions;
    // Member Functions
    void player_act(GameState& gs) override;
    void accept_script(std::vector<std::pair<constants::Action, unsigned>> script);
private:
    // Data Members
    int m_action_number = 0;
};

class CheckCallAI : public PlayerAI
{
public:
    // Constructors
    CheckCallAI(std::mt19937& rng) : PlayerAI(rng) {}
    // Data Members
    const constants::AI_Type ai = constants::AI_Type::CheckCall;
    // Member Functions
    void player_act(GameState& gs) override;
};

// class PersonalityAI : public PlayerAI
// {
// public:
//     // Constructors
//     PersonalityAI(std::mt19937& rng) : PlayerAI(rng) {}
//     // Data Members
//     int confidence_level;
//     // Member Functions
//     void assess_confidence(GameState& gs);
// };

/* Player Declarations
******************************************************************************/
class Player
{
public:
    // Constructors
    Player(int idx, unsigned chips, constants::AI_Type ai, std::mt19937& rng) :
        player_idx(idx),
        ai_type(ai),
        m_chip_count(chips)
    {
        m_select_ai(rng);
    }
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
    void pass_script(std::vector<std::pair<constants::Action, unsigned>> script);
private:
    // Friends
    friend TexasHoldEm;
    // Data Members
    std::shared_ptr<PlayerAI> m_ai;
    Hand m_hand;
    bool m_active = true;
    bool m_eliminated = false;
    unsigned m_chip_count;
    // Member Functions
    unsigned m_push_chips_to_pot(unsigned chips);
    void m_select_ai(std::mt19937& rng);
};
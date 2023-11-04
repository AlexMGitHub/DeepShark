/******************************************************************************
* Classes representing a Texas Hold 'Em player.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <cassert>
#include <iostream>
#include <random> // For shuffle(), uniform_int_distribution<>
// Project headers
#include "player.hpp"
#include "constants.hpp"
#include "storage.hpp"
// Using statements
using std::array;
using std::cout;
using std::endl;
using std::vector;
using namespace constants;

/* PlayerAI Method Definitions
******************************************************************************/
void PlayerAI::player_act(GameState& gs)
{
    /**
     * Select the appropriate act method based on the player's AI type.
    */
    switch (ai)
    {
    case AI_Type::Random:
        return random_act(gs);
        break;
    default:
        cout << "Invalid player AI!" << endl;
        exit(-1);
    }
}

void PlayerAI::random_act(GameState& gs)
{
    /**
     * Player AI randomly chooses from the list of legal actions.
     *
     * @param gs is the current game state.
     * @return Modifies the game state with the chosen action and bet amount.
    */
    set_int_dist(static_cast<int>(gs.legal_actions.size() - 1));
    int action_idx = uni_dist(rng);
    Action random_action = gs.legal_actions[action_idx];
    if (random_action == Action::Fold ||
        random_action == Action::Check)
    {
        gs.player_bet = 0;
        // Player checks if possible, otherwise folds
        if (legal_act(Action::Check, gs))
        {
            gs.player_action = Action::Check;
        }
        else
        {
            gs.player_action = Action::Fold;
        }
    }
    else if (random_action == Action::Call)
    {
        gs.player_bet = gs.chips_to_call;
        gs.player_action = Action::Call;
    }
    else if (random_action == Action::Bet ||
        random_action == Action::Raise ||
        random_action == Action::Re_Raise)
    {
        gs.player_action = random_action;
        set_int_dist(gs.max_bet);
        unsigned random_bet = static_cast<unsigned>(uni_dist(rng));
        if (random_bet < (gs.min_to_raise + gs.chips_to_call))
        {
            gs.player_bet = gs.min_to_raise + gs.chips_to_call;
        }
        else
        {
            gs.player_bet = random_bet;
            if (random_bet == gs.max_bet) { gs.player_action = Action::All_In; }
        }
    }
    else if (random_action == Action::All_In)
    {
        gs.player_bet = gs.max_bet;
        gs.player_action = Action::All_In;
    }
}

bool PlayerAI::legal_act(Action act, GameState& gs)
{
    /**
     * Determine if action is legal.
     *
     * @param act is the action to search for.
     * @param gs is the current game state containing the legal actions vector.
     * @return True if the action exists inside the vector of legal actions.
    */
    return std::find(gs.legal_actions.begin(), gs.legal_actions.end(), act)
        != gs.legal_actions.end();
}

void PlayerAI::set_int_dist(int max)
{
    /**
     * Set the new maximum value of a uniform distribution of integers.
     *
     * @param max is the maximum integer value of the uniform distribution.
     * @return Replace the data member with a new uniform distribution.
    */
    if (max != dist_max)
    {
        uni_dist = std::uniform_int_distribution<>(0, max);
        dist_max = max;
    }
}

/* Player Method Definitions
******************************************************************************/
void Player::eliminate_player()
{
    /**
     * Eliminate player from the poker tournament.
    */
    m_eliminated = true;
    m_active = false;
}

void Player::fold_player()
{
    /**
     * Folder player's hand.
    */
    m_active = false;
}

vector<Card> Player::get_available_cards()
{
    return m_hand.available_cards;
}

array<Card, MAX_CARDS_IN_HAND> Player::get_best_hand()
{
    m_hand.determine_best_hand();
    return m_hand.best_hand;
}

HandRank Player::get_best_hand_rank()
{
    return m_hand.hand_rank;
}

Blind Player::get_blind_status() const
{
    return blind;
}

std::pair<Card, Card> Player::get_hole_cards()
{
    return std::make_pair(m_hand.hole_card1, m_hand.hole_card2);
}

size_t Player::get_card_count() const
{
    return m_hand.available_cards.size();
}

unsigned Player::get_chip_count() const
{
    return m_chip_count;
}

unsigned Player::pay_blind(unsigned chips)
{
    return m_push_chips_to_pot(chips);
}

void Player::player_act(GameState& gs)
{
    /**
     * Call player's AI to determine player action.
    */
    m_ai.player_act(gs);
    m_push_chips_to_pot(gs.player_bet);
    prev_action = gs.player_action;
}

bool Player::is_player_eliminated() const
{
    return m_eliminated;
}

bool Player::is_player_active() const
{
    return m_active;
}

void Player::receive_card(const Card& c)
{
    /**
     * Receive hole card from dealer.
    */
    m_hand.add_card(c);
}

void Player::return_cards()
{
    m_hand.clear_hand();
}

void Player::set_player_active()
{
    m_active = true;
}

void Player::update_blind_status(Blind b)
{
    blind = b;
}

void Player::win_chips(unsigned chips)
{
    m_chip_count += chips;
}

/* Private Player Method Definitions
************************************/
unsigned Player::m_push_chips_to_pot(unsigned chips)
{
    /**
     * Push player's bet to the pot.
     *
     * @param chips is the amount of chips the player bet (or blind amount).
     * @return The amount of chips actually pushed to the pot, which could be
     *         less than the parameter chips if the player's stack is
     *         insufficient.
    */
    if (m_chip_count >= chips)
    {
        m_chip_count -= chips;
        return chips;
    }
    unsigned temp = m_chip_count;
    m_chip_count = 0;
    return temp;
}
/******************************************************************************
* Classes representing a Texas Hold 'Em player.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <algorithm> // For sort()
#include <array>
#include <iostream>
#include <random> // For shuffle()
#include <ranges> // For views::drop()
#include <stdexcept>
#include <string>
#include <utility> // For to_underlying()
// Project headers
#include "player.hpp"
// Using statements
using std::array;
using std::cout;
using std::endl;
using std::sort;
using std::to_underlying;
using std::vector;

/* Player Method Definitions
******************************************************************************/
void Player::eliminate_player()
{
    m_eliminated = true;
}

void Player::fold_player()
{
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

Blind Player::get_blind_status()
{
    return blind;
}

size_t Player::get_card_count()
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

unsigned Player::player_act(GameState& gs)
{
    return 0;
}

bool Player::is_player_eliminated()
{
    return m_eliminated;
}

bool Player::is_player_active() const
{
    return m_active;
}

void Player::receive_card(const Card& c)
{
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

/* Private Player Method Definitions */
unsigned Player::m_push_chips_to_pot(unsigned chips)
{
    if (m_chip_count >= chips)
    {
        m_chip_count -= chips;
        return chips;
    }
    unsigned temp = m_chip_count;
    m_chip_count = 0;
    return temp;
}
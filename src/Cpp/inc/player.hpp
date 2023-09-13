#pragma once

/******************************************************************************
 * Declare classes representing a Texas Hold 'Em player.
 *****************************************************************************/

 /* Headers
 ******************************************************************************/
 // C++ standard library
#include <array>
#include <map>
#include <vector>
// Project headers
#include "cards.hpp"

/* Constants and Enums
******************************************************************************/
enum class Action : int
{
    Bet,
    Call,
    Check,
    Fold,
    Raise,
    Re_Raise
};

enum class Blind : int
{
    No_Blind,
    Small,
    Big
};

/* Declarations
******************************************************************************/
class Player
{
public:
    // Data Members
    Blind blind = Blind::No_Blind;
    // Member Functions
    Blind get_blind_status();
    size_t get_card_count();
    unsigned get_chip_count();
    unsigned pay_blind(unsigned chips);
    unsigned player_act();
    bool player_eliminated();
    bool player_is_active();
    void receive_card(const Card& c);
    void return_cards();
    void update_blind_status(Blind b);
    void win_chips(unsigned chips);
private:
    // Data Members
    bool m_active = true;
    bool m_eliminated = false;
    unsigned m_chip_count = 0;
    Hand m_hand;
    // Member Functions
    unsigned m_push_chips_to_pot(unsigned chips);
};
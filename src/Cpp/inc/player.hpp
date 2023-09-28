#pragma once

/******************************************************************************
* Declare classes representing a Texas Hold 'Em player.
******************************************************************************/

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
    No_Action,
    All_In,
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
    Dealer_Button,
    Small_Blind,
    Big_Blind,
    Dealer_Button_and_Small_Blind
};

/* Declarations
******************************************************************************/
class Player
{
public:
    // Constructors
    Player(unsigned chips, int idx) : m_chip_count(chips), player_idx(idx) {}
    // Data Members
    Blind blind = Blind::No_Blind;
    Action prev_action = Action::No_Action;
    const int player_idx;
    // Member Functions
    void eliminate_player();
    void fold_player();
    Blind get_blind_status() const;
    std::array<Card, MAX_CARDS_IN_HAND> get_best_hand();
    std::vector<Card> get_available_cards();
    size_t get_card_count() const;
    unsigned get_chip_count() const;
    unsigned pay_blind(unsigned chips);
    unsigned player_act(GameState& gs);
    bool is_player_active() const;
    bool is_player_eliminated() const;
    void receive_card(const Card& c);
    void return_cards();
    void set_player_active();
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
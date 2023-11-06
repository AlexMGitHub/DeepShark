/******************************************************************************
* Classes representing a poker dealer and poker pot.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <utility> // For to_underlying()
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"
#include "player.hpp"
#include "pot_dealer.hpp"
// Using statements
using std::to_underlying;
using std::vector;
using namespace constants;

/* Pot Method Definitions
******************************************************************************/
void Pot::add_chips(unsigned chips, unsigned player_idx, Round rnd)
{
    /**
     * Add the chips bet by a player to the pot.
     *
     * The chips are stored in a 2D array where the rows represent different
     * players and the columns represent the betting round when the bet was
     * made.  This includes all bets, raises, calls, and blinds - any time
     * chips are placed in the pot.
     *
     * @param chips is the amount of chips to add to the pot.
     * @param player_idx is the row index of the array to add the chips to.
     * @param rnd is the column index of the array to add the chips to.
    */
    int rnd_idx = to_underlying(rnd);
    m_player_bets[player_idx][rnd_idx] += chips;
    m_chip_count += chips;
}

unsigned Pot::get_chip_count()
{
    return m_chip_count;
}

unsigned Pot::get_player_bets_by_round(unsigned player_idx, Round rnd) const
{
    /**
     * Return the total chips bet by the player in the specified betting round.
    */
    int rnd_idx = to_underlying(rnd);
    return m_player_bets[player_idx][rnd_idx];
}

unsigned Pot::get_total_player_bets(int player_idx) const
{
    /**
     * Return the total number of chips bet by the player in the game so far.
    */
    unsigned total = 0;
    for (int i = 0; i < MAX_BETTING_ROUNDS; i++)
    {
        total += m_player_bets[player_idx][i];
    }
    return total;
}

/* Private Pot Method Definitions */
unsigned Pot::m_pay_to_winner(unsigned chips_bet)
{
    /**
     * Pay out chips to the winner of a game.
     *
     * A player can only win a maximum of chips_bet from each player in the
     * game.  This method ensures that side pots are properly distributed so
     * long as the winners are paid in ascending order of chips bet.
     *
     * @param chips_bet is the total number of chips bet by the player during
     *                  the game.
     * @return appropriate amount of chips for winner of side pot or main pot
    */
    unsigned total_chips = 0;
    unsigned plyr_chips;
    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        plyr_chips = get_total_player_bets(i);
        m_clear_players_bets(i);
        if (plyr_chips >= chips_bet)
        {
            m_chip_count -= chips_bet;
            m_player_bets[i][0] = plyr_chips - chips_bet;
            total_chips += chips_bet;
        }
        else
        {
            m_chip_count -= plyr_chips;
            total_chips += plyr_chips;
        }
    }
    return total_chips;
}

void Pot::m_clear_players_bets(int player_idx)
{
    /**
     * Remove all chips bet by a particular player from the pot.
    */
    for (int i = 0; i < MAX_BETTING_ROUNDS; i++)
    {
        m_player_bets[player_idx][i] = 0;
    }
}

/* Dealer Method Definitions
******************************************************************************/
void Dealer::burn_one_card()
{
    m_deck.burn_one_card();
}

void Dealer::clear_cards_from_table()
{
    flop_card1 = Card();
    flop_card2 = Card();
    flop_card3 = Card();
    turn_card = Card();
    river_card = Card();
}

Card Dealer::deal_one_card()
{
    return m_deck.deal_one_card();
}

void Dealer::shuffle_deck()
{
    m_deck.shuffle_deck();
}

void Dealer::deal_to_players(
    vector<Player>& player_list,
    int num_players,
    int button_idx)
{
    /**
     * Deal two hole cards to each player.
     *
     * Deal starts with the first player after the dealer button.
     *
     * @param player_list is a list of all players who entered the tournament.
     * @param num_players is the number of players remaining in the tournament.
     * @param button_idx is the index of the player who has the dealer button.
    */

    int num_cards_dealt = 0;
    int i = static_cast<int>((button_idx + 1) % player_list.size());
    while (num_cards_dealt < (2 * num_players))
    {
        if (!player_list[i].is_player_eliminated())
        {
            player_list[i].receive_card(deal_one_card());
            num_cards_dealt++;
        }
        i = static_cast<int>((i + 1) % player_list.size());
    }
}

void Dealer::deal_flop(vector<Player>& player_list)
{
    /**
     * Deal the flop - three community cards.
     *
     * The dealer must burn a card before dealing the flop.  Each player who
     * remains in the tournament receives a copy of the flop cards so that
     * they can assemble their best poker hand.
    */
    burn_one_card();
    flop_card1 = deal_one_card();
    flop_card2 = deal_one_card();
    flop_card3 = deal_one_card();
    for (auto& player : player_list)
    {
        if (!player.is_player_eliminated() &&
            player.is_player_active())
        {
            player.receive_card(flop_card1);
            player.receive_card(flop_card2);
            player.receive_card(flop_card3);
        }
    }
}

void Dealer::deal_turn(vector<Player>& player_list)
{
    /**
     * Deal the turn - one community card.
     *
     * The dealer must burn a card before dealing the turn.  Each player who
     * remains in the tournament receives a copy of the turn card so that
     * they can assemble their best poker hand.
    */
    burn_one_card();
    turn_card = deal_one_card();
    for (auto& player : player_list)
    {
        if (!player.is_player_eliminated() &&
            player.is_player_active())
        {
            player.receive_card(turn_card);
        }
    }
}

void Dealer::deal_river(vector<Player>& player_list)
{
    /**
     * Deal the river - one community card.
     *
     * The dealer must burn a card before dealing the river.  Each player who
     * remains in the tournament receives a copy of the river card so that
     * they can assemble their best poker hand.
    */
    burn_one_card();
    river_card = deal_one_card();
    for (auto& player : player_list)
    {
        if (!player.is_player_eliminated() &&
            player.is_player_active())
        {
            player.receive_card(river_card);
        }
    }
}

void Dealer::stack_the_deck(vector<Card> cards)
{
    /**
     * Replace cards in deck with cards chosen for use in a test scenario.
    */
    m_deck.stack_the_deck(cards);
}
/******************************************************************************
* Contains functions necessary to implement the Texas Hold 'Em environment.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <algorithm> // For sort()
#include <cassert>
#include <iostream>
#include <random> // For shuffle()
#include <ranges> // For views::drop()
#include <stdexcept>
#include <string>
#include <utility> // For to_underlying()
// Project headers
#include "cards.hpp"
#include "texasholdem.hpp"
#include "storage.hpp"
// Using statements
using std::cout;
using std::endl;
using std::sort;
using std::to_underlying;
using std::vector;

/* Pot Method Definitions
******************************************************************************/
void Pot::add_chips(unsigned chips, unsigned player_idx, Round rnd)
{
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
    int rnd_idx = to_underlying(rnd);
    return m_player_bets[player_idx][rnd_idx];
}

/* Private Pot Method Definitions */
unsigned Pot::m_pay_to_winner(GameState gs)
{
    unsigned winnings = 0;
    int num_players_showdown = gs.showdown_player_idx.size();
    if (num_players_showdown <= 1)
    {
        // The game ended before the showdown and all but one player folded
        winnings = m_chip_count;
        m_chip_count = 0;
        std::fill(&m_player_bets[0][0],
            &m_player_bets[0][0] + sizeof(m_player_bets), 0);
        return winnings;
    }
    // The game made it to the showdown stage with more than one player
    unsigned winner_chips_bet = 0;
    for (int i = 0; i < MAX_BETTING_ROUNDS; i++)
    {
        // Calculate how many chips were bet by the winner
        winner_chips_bet += m_player_bets[gs.winner_idx][i];
        m_player_bets[gs.winner_idx][i] = 0;
    }
    winnings += winner_chips_bet;
    unsigned player_total_chips = 0;
    // Compare the number of chips bet by the winner to the losers to check if
    // there are side pots
    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        player_total_chips = 0;
        for (int j = 0; j < MAX_BETTING_ROUNDS; j++)
        {
            player_total_chips += m_player_bets[i][j];
            m_player_bets[i][j] = 0;
        }
        if (player_total_chips <= winner_chips_bet)
        {
            // Winner bet equal or more chips than loser, so he gets them all
            winnings += player_total_chips;
            m_chip_count -= player_total_chips;
        }
        else
        {
            // Winner did not bet as much as loser, so there must be a side pot
            winnings += winner_chips_bet;
            m_chip_count -= winner_chips_bet;
            m_player_bets[i][j] = player_total_chips - winner_chips_bet;
        }
    }
    return winnings;
}

/* Dealer Method Definitions
******************************************************************************/
void Dealer::deal_to_players(
    vector<Player>& player_list,
    unsigned num_players,
    int button_idx)
{
    // Deal two cards to each player, starting with the first player after the
    // dealer button
    // In heads-up poker this is the big blind, otherwise the small blind
    int num_cards_dealt = 0;
    int i = (button_idx + 1) % player_list.size();
    while (num_cards_dealt < (2 * num_players))
    {
        if (!player_list[i].is_player_eliminated())
        {
            player_list[i].receive_card(deal_one_card());
            num_cards_dealt++;
        }
        i = (button_idx + 1) % player_list.size();
    }
}

void Dealer::deal_flop(vector<Player>& player_list)
{
    burn_one_card();
    Card& flop_card1 = deal_one_card();
    Card& flop_card2 = deal_one_card();
    Card& flop_card3 = deal_one_card();
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
    burn_one_card();
    Card& turn_card = deal_one_card();
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
    burn_one_card();
    Card& river_card = deal_one_card();
    for (auto& player : player_list)
    {
        if (!player.is_player_eliminated() &&
            player.is_player_active())
        {
            player.receive_card(river_card);
        }
    }
}

/* Texas Hold 'Em Method Definitions
******************************************************************************/
void TexasHoldEm::begin_tournament()
{
    if (!m_tournament_started)
    {
        m_tournament_started = true;
        for (int i = 0; i <= initial_num_players; i++)
        {
            full_player_list.push_back(Player(MAX_BUY_IN, i));
            remaining_player_list.push_back(full_player_list[i]);
        }
        m_begin_round();
    }
    else
    {
        cout << "Tournament already in progress!" << endl;
    }
}

void TexasHoldEm::m_begin_round()
{
    // Set all (non-eliminated) players to active, and action to No_Action
    for (auto& player : full_player_list)
    {
        if (!player.is_player_eliminated())
        {
            player.set_player_active();
            player.prev_action = Action::No_Action;
            player.blind = Blind::No_Blind;
        }
    }
    // Identify players with dealer button and blinds
    if (num_players > 2)
    {
        full_player_list[m_button_idx].blind = Blind::Dealer_Button;
        full_player_list[m_sb_idx].blind = Blind::Small_Blind;
    }
    else  // Heads-up play
    {
        full_player_list[m_button_idx].blind = Blind::Dealer_Button_and_Small_Blind;
    }
    full_player_list[m_bb_idx].blind = Blind::Big_Blind;
    // Collect blinds
    unsigned sb = BLINDS_STRUCTURE.at(num_players).first;
    unsigned bb = BLINDS_STRUCTURE.at(num_players).second;
    unsigned sb_pmt = full_player_list[m_sb_idx].pay_blind(sb);
    unsigned bb_pmt = full_player_list[m_bb_idx].pay_blind(bb);
    pot.add_chips(sb_pmt, m_sb_idx);
    pot.add_chips(bb_pmt, m_bb_idx);
    // Deal cards to players
    dealer.shuffle_deck();
    dealer.deal_to_players(full_player_list, num_players, m_button_idx);
    m_pre_flop();
}

bool TexasHoldEm::m_is_betting_over(Round rnd) const
{
    unsigned num_chips_bet;
    unsigned bet;
    bool first_active_player = true;
    for (const auto& player : full_player_list)
    {
        // Make sure player is active and not all-in
        if (player.is_player_active() && (player.get_chip_count() != 0))
        {
            // Store amount first active player has placed into the pot
            if (first_active_player)
            {
                first_active_player = false;
                num_chips_bet = pot.get_player_bets_by_round(
                    player.player_idx,
                    rnd);
            }
            else
            {
                // Betting is over if all active and not all-in players have 
                // bet the same amount, otherwise betting is not over.
                bet = pot.get_player_bets_by_round(
                    player.player_idx,
                    rnd);
                if (bet != num_chips_bet) { return false; }
            }
        }
    }
    if (num_chips_bet == 0) { return false; }  // Betting hasn't started yet
    return true;
}

unsigned TexasHoldEm::m_get_num_active_players() const
{
    // Returns size_t?
    return m_get_vec_active_players().size();
}

vector<unsigned> TexasHoldEm::m_get_player_chip_counts() const
{
    vector<unsigned> chip_counts;
    for (const auto& player : full_player_list)
    {
        chip_counts.push_back(player.get_chip_count());
    }
    return chip_counts;
}

vector<int> TexasHoldEm::m_get_vec_active_players() const
{
    vector<int> active_players;
    for (const auto& player : full_player_list)
    {
        if (player.is_player_active())
        {
            active_players.push_back(player.player_idx);
        }
    }
    return active_players;
}



void TexasHoldEm::m_update_game_state(GameState& gs, RoundData& rd)
{
    // Round info
    gs.player_idx = rd.player_idx;
    gs.action_number = rd.action_number;
    gs.round = rd.round;
    gs.raise_active = rd.raise_active;
    gs.raise_player_idx = rd.raise_player_idx;
    gs.num_active_players = rd.num_active_players;
    gs.pot_chip_count = rd.pot_chip_count;
    gs.legal_actions = rd.legal_actions;
    // Player info
    gs.blind = full_player_list[gs.player_idx].blind;
    gs.player_chip_counts = m_get_player_chip_counts();
    gs.active_player_list = m_get_vec_active_players();
    gs.best_hand = full_player_list[gs.player_idx].get_best_hand();
    gs.available_cards = full_player_list[gs.player_idx].get_available_cards();
}

void TexasHoldEm::m_betting_loop(RoundData rd, GameState gs)
{
    while (!m_is_betting_over(rd.round))
    {
        if (!full_player_list[i].is_player_eliminated() &&
            full_player_list[i].is_player_active() &&
            (full_player_list[i].get_chip_count() > 0))
        {
            // Calculate chip requirements
            rd.pot_chip_count = pot.get_chip_count();
            rd.sum_prev_bets = pot.get_player_bets_by_round(i, rd.round);
            rd.chips_to_call = rd.min_bet - rd.sum_prev_bets;
            rd.max_bet = full_player_list[i].get_chip_count();
            rd.player_idx = i;
            if (rd.chips_to_call > rd.max_bet)
            {
                rd.legal_actions = { Action::All_In, Action::Fold };
                rd.chips_to_call = rd.max_bet;
                rd.min_to_raise = rd.max_bet;
                m_update_game_state(gs, rd);
                rd.player_bet = full_player_list[i].player_act(gs);
                assert((rd.player_bet == rd.max_bet) || (rd.player_bet == 0) &&
                    "Invalid player bet!");
            }
            else
            {
                rd.legal_actions = { Action::Fold };
                if (rd.all_in_below_min_raise && (rd.raise_player_idx == i))
                {
                    rd.legal_to_raise = false;
                }
                if (rd.legal_to_raise)
                {
                    rd.legal_actions.push_back(Action::All_In);
                }
                if (rd.chips_to_call == 0)
                {
                    rd.legal_actions.push_back(Action::Check);
                }
                if (rd.legal_to_raise &&
                    !rd.raise_active &&
                    (rd.max_bet > rd.min_to_raise))
                {
                    rd.legal_actions.push_back(Action::Raise);
                    rd.raise_player_idx = i;
                }
                else if (rd.legal_to_raise &&
                    rd.raise_active &&
                    (rd.max_bet > rd.min_to_raise))
                {
                    rd.legal_actions.push_back(Action::Re_Raise);
                    rd.raise_player_idx = i;
                }
                if (!rd.legal_to_raise && (rd.chips_to_call > 0))
                {
                    rd.legal_actions.push_back(Action::Call);
                    rd.max_bet = rd.chips_to_call;
                }
                m_update_game_state(gs, rd);
                rd.player_bet = full_player_list[i].player_act(gs);
                assert((((rd.player_bet >= rd.chips_to_call) &&
                    (rd.player_bet <= rd.max_bet)) ||
                    (rd.player_bet == 0)) && "Invalid player bet!");
            }
            rd.player_action = gs.player_action;
            if (rd.player_bet == rd.max_bet &&
                (rd.player_bet < rd.chips_to_call + rd.min_to_raise))
            {
                assert((rd.player_action == Action::All_In) &&
                    "Action should be all-in!");
                if (rd.player_bet < rd.chips_to_call)
                {
                    // Player has gone all-in, but doesn't have enough chips 
                    // to cover call
                    // No change to min bet or min raise
                }
                else
                {
                    // Player has gone all-in, but doesn't have enough chips 
                    // to cover the minimum raise
                    // No change to min raise, but it's not legal for the 
                    // player who made the raise to re-raise unless another
                    // player raises first.
                    rd.all_in_below_min_raise = true;
                    rd.min_bet = rd.player_bet + rd.sum_prev_bets;
                }
            }
            else if (rd.player_bet > rd.min_bet)
            {
                // Player has raised
                assert((rd.player_bet >= (rd.chips_to_call + rd.min_to_raise))
                    && "Invalid player raise!");
                assert((rd.player_action == Action::Raise) &&
                    "Action should be raise!");
                rd.min_to_raise = rd.player_bet - rd.chips_to_call;
                rd.min_bet = rd.player_bet + rd.sum_prev_bets;
                rd.raise_active = true;
            }
            else if ((rd.player_bet == 0) && (rd.chips_to_call > 0))
            {
                // Player has folded
                // No change to min bet or min raise
                assert((rd.player_action == Action::Fold) &&
                    "Action should be fold!");
                full_player_list[i].fold_player();
            }
            else if ((rd.player_bet == 0) && (rd.chips_to_call == 0))
            {
                // Player has checked
                // No change to min bet or min raise
                assert((rd.player_action == Action::Check) &&
                    "Action should be check!");
            }
            else if ((rd.player_bet == rd.chips_to_call) &&
                (rd.chips_to_call != 0))
            {
                // Player has called
                // No change to min bet or min raise
                assert((rd.player_action == Action::Call) &&
                    "Action should be call!");
            }
            // Store game state
            game_hist.states.push_back(gs);
            pot.add_chips(rd.player_bet, i, Round::Pre_Flop);
        }
        i = (i + 1) % initial_num_players;
        rd.action_number++;
    }
}

void TexasHoldEm::m_pre_flop()
{
    // The player after the big blind always goes first before the flop
    // This is the Under The Gun (UTG) player for more than 2 players
    // For heads-up poker the dealer/small blind goes first
    GameState gs;
    RoundData rd;

    int i = (m_bb_idx + 1) % initial_num_players;
    rd.min_bet = BLINDS_STRUCTURE.at(num_players).second;
    rd.min_to_raise = BLINDS_STRUCTURE.at(num_players).second;
    rd.num_active_players = m_get_num_active_players();
    rd.round = Round::Pre_Flop;
    rd.legal_to_raise = true;
    rd.raise_active = true;
    rd.raise_player_idx = m_bb_idx;
    rd.all_in_below_min_raise = false;
    rd.action_number = 0;
    rd.pot_chip_count = pot.get_chip_count();
    while (!m_is_betting_over(rd.round))
    {
        if (!full_player_list[i].is_player_eliminated() &&
            full_player_list[i].is_player_active() &&
            (full_player_list[i].get_chip_count() > 0))
        {
            // Calculate chip requirements
            rd.pot_chip_count = pot.get_chip_count();
            rd.sum_prev_bets = pot.get_player_bets_by_round(i, rd.round);
            rd.chips_to_call = rd.min_bet - rd.sum_prev_bets;
            rd.max_bet = full_player_list[i].get_chip_count();
            rd.player_idx = i;
            if (rd.chips_to_call > rd.max_bet)
            {
                rd.legal_actions = { Action::All_In, Action::Fold };
                rd.chips_to_call = rd.max_bet;
                rd.min_to_raise = rd.max_bet;
                m_update_game_state(gs, rd);
                rd.player_bet = full_player_list[i].player_act(gs);
                assert((rd.player_bet == rd.max_bet) || (rd.player_bet == 0) &&
                    "Invalid player bet!");
            }
            else
            {
                rd.legal_actions = { Action::Fold };
                if (rd.all_in_below_min_raise && (rd.raise_player_idx == i))
                {
                    rd.legal_to_raise = false;
                }
                if (rd.legal_to_raise)
                {
                    rd.legal_actions.push_back(Action::All_In);
                }
                if (rd.chips_to_call == 0)
                {
                    rd.legal_actions.push_back(Action::Check);
                }
                if (rd.legal_to_raise &&
                    !rd.raise_active &&
                    (rd.max_bet > rd.min_to_raise))
                {
                    rd.legal_actions.push_back(Action::Raise);
                    rd.raise_player_idx = i;
                }
                else if (rd.legal_to_raise &&
                    rd.raise_active &&
                    (rd.max_bet > rd.min_to_raise))
                {
                    rd.legal_actions.push_back(Action::Re_Raise);
                    rd.raise_player_idx = i;
                }
                if (!rd.legal_to_raise && (rd.chips_to_call > 0))
                {
                    rd.legal_actions.push_back(Action::Call);
                    rd.max_bet = rd.chips_to_call;
                }
                m_update_game_state(gs, rd);
                rd.player_bet = full_player_list[i].player_act(gs);
                assert((((rd.player_bet >= rd.chips_to_call) &&
                    (rd.player_bet <= rd.max_bet)) ||
                    (rd.player_bet == 0)) && "Invalid player bet!");
            }
            rd.player_action = gs.player_action;
            if (rd.player_bet == rd.max_bet &&
                (rd.player_bet < rd.chips_to_call + rd.min_to_raise))
            {
                assert((rd.player_action == Action::All_In) &&
                    "Action should be all-in!");
                if (rd.player_bet < rd.chips_to_call)
                {
                    // Player has gone all-in, but doesn't have enough chips 
                    // to cover call
                    // No change to min bet or min raise
                }
                else
                {
                    // Player has gone all-in, but doesn't have enough chips 
                    // to cover the minimum raise
                    // No change to min raise, but it's not legal for the 
                    // player who made the raise to re-raise unless another
                    // player raises first.
                    rd.all_in_below_min_raise = true;
                    rd.min_bet = rd.player_bet + rd.sum_prev_bets;
                }
            }
            else if (rd.player_bet > rd.min_bet)
            {
                // Player has raised
                assert((rd.player_bet >= (rd.chips_to_call + rd.min_to_raise))
                    && "Invalid player raise!");
                assert((rd.player_action == Action::Raise) &&
                    "Action should be raise!");
                rd.min_to_raise = rd.player_bet - rd.chips_to_call;
                rd.min_bet = rd.player_bet + rd.sum_prev_bets;
                rd.raise_active = true;
            }
            else if ((rd.player_bet == 0) && (rd.chips_to_call > 0))
            {
                // Player has folded
                // No change to min bet or min raise
                assert((rd.player_action == Action::Fold) &&
                    "Action should be fold!");
                full_player_list[i].fold_player();
            }
            else if ((rd.player_bet == 0) && (rd.chips_to_call == 0))
            {
                // Player has checked
                // No change to min bet or min raise
                assert((rd.player_action == Action::Check) &&
                    "Action should be check!");
            }
            else if ((rd.player_bet == rd.chips_to_call) &&
                (rd.chips_to_call != 0))
            {
                // Player has called
                // No change to min bet or min raise
                assert((rd.player_action == Action::Call) &&
                    "Action should be call!");
            }
            // Store game state
            game_hist.states.push_back(gs);
            pot.add_chips(rd.player_bet, i, Round::Pre_Flop);
        }
        i = (i + 1) % initial_num_players;
        rd.action_number++;
    }
    if (m_get_num_active_players() == 1)
    {
        m_end_round();
    }
    else
    {
        m_flop(rd);
    }

    void TexasHoldEm::m_flop(RoundData rd)
    {
        GameState gs;
        rd.round = Round::Flop;
        dealer.deal_flop(full_player_list));
        m_betting_loop(rd, gs);
        if (m_get_num_active_players() == 1)
        {
            m_end_round();
        }
        else
        {
            m_turn(rd);
        }
    }

    void TexasHoldEm::m_turn(RoundData rd)
    {
        GameState gs;
        rd.round = Round::Turn;
        dealer.deal_turn(full_player_list));
        m_betting_loop(rd, gs);
        if (m_get_num_active_players() == 1)
        {
            m_end_round();
        }
        else
        {
            m_river(rd);
        }
    }

    void TexasHoldEm::m_river(RoundData rd)
    {
        GameState gs;
        rd.round = Round::River;
        dealer.deal_river(full_player_list));
        m_betting_loop(rd, gs);
        if (m_get_num_active_players() == 1)
        {
            m_end_round();
        }
        else
        {
            m_showdown(rd);
        }
    }

    void TexasHoldEm::m_showdown(RoundData rd)
    {
        GameState gs;
        rd.round = Round::Showdown;
        array<Card, MAX_CARDS_IN_HAND> best_hand;
        HandRank best_rank;
        bool first_player = true;
        int plyr_idx;
        array<Card, MAX_CARDS_IN_HAND> plyr_best_hand;
        HandRank plyr_best_hand_rank;
        int winner_idx;
        for (auto& player : full_player_list)
        {
            if (!player.is_player_eliminated() && player.is_player_active())
            {
                plyr_idx = player.player_idx;
                plyr_best_hand = player.get_best_hand();
                plyr_best_hand_rank = player.get_best_hand_rank();
                if (first_player)
                {
                    first_player = false;
                    best_hand = plyr_best_hand;
                    best_hand_rank = plyr_best_hand_rank;
                    winner_idx = plyr_idx;
                }
                else
                {
                    if (plyr_best_hand_rank > best_hand_rank)
                    {
                        best_hand = plyr_best_hand;
                        best_hand_rank = plyr_best_hand_rank;
                        winner_idx = plyr_idx;
                    }
                    else if (player_best_hand_rank == best_hand_rank)
                    {
                        for (int i = MAX_CARDS_IN_HAND - 1; i >= 0; i--)
                        {
                            if (best_hand[i].rank < plyr_best_hand[i].rank)
                            {
                                best_hand = plyr_best_hand;
                                best_hand_rank = plyr_best_hand_rank;
                                winner_idx = plyr_idx;
                                break;
                            }
                            else if (best_hand[i].rank == plyr_best_hand[i].rank)
                            {
                                continue;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
                gs.showdown_player_idx.push_back(plyr_idx);
                gs.showdown_hands.push_back(plyr_best_hand);
                gs.showdown_hand_ranks.push_back(plyr_best_hand_rank);
            }
        }
        gs.winner_idx = winner_idx;
        game_hist.states.push_back(gs);
        m_end_round(gs);
    }

    // Need to check if game moves to next phase or if all other players folded
    // Need to decide where actions taken by players will be verified
    // Probably makes most sense to check inside the player class, but will 
    // need to pass the player the current bet/raise amount, and previous
    // player action.  Probably also pass what they've contributed so far?
    // Or maybe pass a list of legal actions
}

void TexasHoldEm::m_end_round(GameState gs)
{
    // All players return their cards
    for (auto& player : full_player_list)
    {
        player.return_cards();
    }
    // Pay the winner of the round
    full_player_list[gs.winner_idx].win_chips(pot.m_pay_to_winner());
    while (pot.m_chip_count > 0)
    {
        // Determine next-strongest hand from showdown players
        // Provide that player's index to the pot
        // Continue paying out side pots until pot is empty
    }

    // Save the player index of the player who had the button this round
    //int btn_player_idx = remaining_player_list[m_button_idx].get().player_idx;
    // Check if players eliminated; clear remaining_player_list and re-populate
    // only with non-eliminated players from full_player_list
    remaining_player_list.clear();
    for (int i = 0; i < full_player_list.size(); i++)
    {
        Player player = full_player_list[i];
        if (!player.is_player_eliminated())
        {
            if (player.get_chip_count() == 0)
            {
                player.eliminate_player();
                num_players--;
            }
            else
            {
                remaining_player_list.push_back(player);
            }
        }
    }
    m_move_blinds();
    m_round_count++;
    m_begin_round();
}

void TexasHoldEm::m_move_blinds()
{
    // Move dealer button to the next eligible (i.e. not eliminated) player
    int j = (m_button_idx + 1) % initial_num_players;
    bool btn_moved = false;
    while (!btn_moved)
    {
        if (!full_player_list[j].is_player_eliminated())
        {
            // The next player at the table hasn't been eliminated and 
            // receives the dealer button
            m_button_idx = j;
            btn_moved = true;
        }
        else
        {
            // The next player at the table has been eliminated, so we
            // have to keep searching for an eligble player
            j = (j + 1) % initial_num_players;
        }
    }
    // Move small blind based on location of dealer button
    if (num_players > 2)
    {
        bool sb_moved = false;
        j = (m_button_idx + 1) % initial_num_players;
        while (!sb_moved)
        {
            if (!full_player_list[j].is_player_eliminated())
            {
                m_sb_idx = j;
                sb_moved = true;
            }
            else
            {
                j = (j + 1) % initial_num_players;
            }
        }
    }
    else
    {
        // Dealer button is small blind for heads-up play
        m_sb_idx = m_button_idx;
    }
    // Move big blind based on location of small blind
    bool bb_moved = false;
    j = (m_sb_idx + 1) % initial_num_players;
    while (!bb_moved)
    {
        if (!full_player_list[j].is_player_eliminated())
        {
            m_bb_idx = j;
            bb_moved = true;
        }
        else
        {
            j = (j + 1) % initial_num_players;
        }
    }
}

// What if player can't afford blind?

// Before the flop, big blind always acts last
// Under the Gun (UTG) player after big blind goes first
// After the flop for the rest of the game the button is the last player to act
// That is, the player before the small blind
// Or in other words, the small blind goes first
// The first person to act after the flop (small blind) receives card first
// On showdown reveal hand order is same as post-flop betting order; small blind first button last

// Heads up poker (2 players)
// Dealer button posts small blind and is first to act pre-flop
// After flop big blind is first to play

// button
// sb
// bb
// utg

// if a player can't afford the big blind, each subsequent player who posts the bb
// has the excess money put in a side pot that the poor player can't win

// Player should always be able to afford the small blind, because the smallest
// betting increment is the SB, so a player that can't afford the SB has zero
// chips and is eliminated

// but the SB increases over time, so that's not necessarily true



// To raise, have to raise by at least BB

// To re-raise, have to raise by at least the last raise amount

// One caveat, if the player is open-raising (first raise, hasn't paid BB)
// then it's double the amount he raised, which would be that amount minus the
// BB since he hasn't paid it yet

// Allowed to re-raise less than the previous raise only if going all-in
// https://www.888poker.com/magazine/strategy/ultimate-guide-re-raising-poker

// the dealer will burn a card before the flop, turn, and river.
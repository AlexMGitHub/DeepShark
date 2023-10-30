/******************************************************************************
* Contains functions necessary to implement the Texas Hold 'Em environment.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <algorithm> // For sort()
#include <cassert>
#include <functional> // For std::greater
#include <iostream>
#include <random> // For shuffle()
#include <ranges> // For views::drop()
#include <stdexcept>
#include <string>
#include <utility> // For to_underlying()
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"
#include "player.hpp"
#include "serialize.hpp"
#include "storage.hpp"
#include "texasholdem.hpp"
// Using statements
using std::cout;
using std::endl;
using std::sort;
using std::to_underlying;
using std::vector;
using namespace constants;

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

unsigned Pot::get_total_player_bets(int player_idx)
{
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
    for (int i = 0; i < MAX_BETTING_ROUNDS; i++)
    {
        m_player_bets[player_idx][i] = 0;
    }
}

// unsigned Pot::m_pay_to_winner(GameState gs, int winner_idx)
// {
//     unsigned winnings = 0;
//     int num_players_showdown = static_cast<int>(gs.showdown_players.size());
//     if (num_players_showdown <= 1)
//     {
//         // The game ended before the showdown and all but one player folded
//         winnings = m_chip_count;
//         m_chip_count = 0;
//         std::fill(&m_player_bets[0][0],
//             &m_player_bets[0][0] + sizeof(m_player_bets), 0);
//         return winnings;
//     }
//     // The game made it to the showdown stage with more than one player
//     unsigned winner_chips_bet = 0;
//     for (int i = 0; i < MAX_BETTING_ROUNDS; i++)
//     {
//         // Calculate how many chips were bet by the winner
//         winner_chips_bet += m_player_bets[winner_idx][i];
//         m_player_bets[winner_idx][i] = 0;
//     }
//     winnings += winner_chips_bet;
//     unsigned player_total_chips = 0;
//     // Compare the number of chips bet by the winner to the losers to check if
//     // there are side pots
//     for (int i = 0; i < MAX_PLAYER_COUNT; i++)
//     {
//         player_total_chips = 0;
//         int j = 0;
//         for (j = 0; j < MAX_BETTING_ROUNDS; j++)
//         {
//             player_total_chips += m_player_bets[i][j];
//             m_player_bets[i][j] = 0;
//         }
//         if (player_total_chips <= winner_chips_bet)
//         {
//             // Winner bet equal or more chips than loser, so he gets them all
//             winnings += player_total_chips;
//             m_chip_count -= player_total_chips;
//         }
//         else
//         {
//             // Winner did not bet as much as loser, so there must be a side pot
//             winnings += winner_chips_bet;
//             m_chip_count -= winner_chips_bet;
//             m_player_bets[i][j] = player_total_chips - winner_chips_bet;
//         }
//     }
//     return winnings;
// }

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
    // Deal two cards to each player, starting with the first player after the
    // dealer button
    // In heads-up poker this is the big blind, otherwise the small blind
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

/* Texas Hold 'Em Method Definitions
******************************************************************************/
void TexasHoldEm::begin_tournament()
{
    if (!m_tournament_started)
    {
        m_tournament_started = true;
        for (int i = 0; i < initial_num_players; i++)
        {
            full_player_list.push_back(Player(i, MAX_BUY_IN, AI_Type::Random, rng));
        }
        tourn_hist.games.clear();
        tourn_hist.tournament_number = tournament_number;
        tourn_hist.initial_player_count = initial_num_players;
        tourn_hist.random_seed = random_seed;
        game_hist.game_number = 0;
        game_hist.states.clear();
        GameState gs;
        gs.random_seed = tourn_hist.random_seed;
        gs.tournament_number = tourn_hist.tournament_number;
        gs.game_number = game_hist.game_number;
        gs.num_players = tourn_hist.initial_player_count;
        for (int i = 0; i < initial_num_players; i++)
        {
            gs.ai_types.push_back(full_player_list[i].ai_type);
            gs.remaining_players.push_back(true);
        }
        m_begin_round(gs);
    }
    else
    {
        cout << "Tournament already in progress!" << endl;
    }
}

void TexasHoldEm::m_begin_round(GameState gs)
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
        full_player_list[m_bb_idx].blind = Blind::Big_Blind;
        int j = (m_bb_idx + 1) % initial_num_players;
        int utg_idx = 0;
        while (1)
        {
            if (full_player_list[j].blind != Blind::No_Blind) { break; }
            if (!full_player_list[j].is_player_eliminated())
            {
                full_player_list[j].blind = UTG_Players[utg_idx++];
            }
            j = (j + 1) % initial_num_players;
        }
    }
    else  // Heads-up play
    {
        full_player_list[m_button_idx].blind = Blind::Dealer_Button_and_Small_Blind;
        full_player_list[m_bb_idx].blind = Blind::Big_Blind;
    }

    // Collect blinds
    unsigned sb = BLINDS_STRUCTURE.at(num_players).first;
    unsigned bb = BLINDS_STRUCTURE.at(num_players).second;
    unsigned sb_pmt = full_player_list[m_sb_idx].pay_blind(sb);
    unsigned bb_pmt = full_player_list[m_bb_idx].pay_blind(bb);
    pot.add_chips(sb_pmt, m_sb_idx, Round::Pre_Flop);
    pot.add_chips(bb_pmt, m_bb_idx, Round::Pre_Flop);
    if ((sb_pmt < sb))
    {
        full_player_list[m_sb_idx].prev_action = Action::All_In;
        gs.all_in_below_min_raise = true;
    }
    else if ((bb_pmt < bb))
    {
        full_player_list[m_bb_idx].prev_action = Action::All_In;
        gs.all_in_below_min_raise = true;
    }
    else
    {
        gs.all_in_below_min_raise = false;
    }
    // Deal cards to players
    dealer.shuffle_deck();
    dealer.deal_to_players(full_player_list, num_players, m_button_idx);
    gs.blinds.clear();
    gs.last_actions.clear();
    gs.hole_cards.clear();
    gs.best_hands.clear();
    gs.hand_ranks.clear();
    for (int i = 0; i < initial_num_players; i++)
    {
        gs.blinds.push_back(full_player_list[i].blind);
        gs.last_actions.push_back(full_player_list[i].prev_action);
        gs.hole_cards.push_back(full_player_list[i].get_hole_cards());
        gs.best_hands.push_back(full_player_list[i].get_best_hand());
        gs.hand_ranks.push_back(HandRank::High_Card);
        gs.remaining_players[i] = !full_player_list[i].is_player_eliminated();
    }
    m_update_game_state(gs);
    //game_hist.states.push_back(gs);
    m_pre_flop(gs);
}

bool TexasHoldEm::m_is_betting_over(Round rnd) const
{
    if (m_get_num_active_players() == 1) { return true; }
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

int TexasHoldEm::m_get_num_active_not_allin_players() const
{
    int num_active_not_allin = 0;
    for (const auto& player : full_player_list)
    {
        if (player.is_player_active() && (player.get_chip_count() != 0))
        {
            num_active_not_allin++;
        }
    }
    return num_active_not_allin;
}

int TexasHoldEm::m_get_num_active_players() const
{
    int num_active = 0;
    auto active_players = m_get_vec_active_players();
    for (const auto& player : active_players)
    {
        if (player) { num_active++; }
    }
    return num_active;
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
            active_players.push_back(true);
        }
        else
        {
            active_players.push_back(false);
        }
    }
    return active_players;
}

void TexasHoldEm::m_update_game_state(GameState& gs)
{
    // Game info
    gs.game_number = game_hist.game_number;
    gs.num_players = num_players;
    // Player info
    for (int i = 0; i < initial_num_players; i++)
    {
        gs.last_actions[i] = full_player_list[i].prev_action;
        gs.best_hands[i] = full_player_list[i].get_best_hand();
        gs.hand_ranks[i] = full_player_list[i].get_best_hand_rank();
    }
    gs.player_chip_counts = m_get_player_chip_counts();
    gs.num_active_players = m_get_num_active_players();
    gs.active_player_list = m_get_vec_active_players();
    gs.best_hand = full_player_list[gs.player_idx].get_best_hand();
    gs.available_cards = full_player_list[gs.player_idx].get_available_cards();
    // Dealer info
    gs.flop_card1 = dealer.flop_card1;
    gs.flop_card2 = dealer.flop_card2;
    gs.flop_card3 = dealer.flop_card3;
    gs.turn_card = dealer.turn_card;
    gs.river_card = dealer.river_card;
    // Pot info
    gs.pot_chip_count = pot.get_chip_count();
    gs.pot_player_bets = pot.m_player_bets;
    // Update vector sizes
    gs.initial_num_players = initial_num_players;
    gs.num_available_cards = gs.available_cards.size();
    gs.num_legal_actions = gs.legal_actions.size();
    gs.num_showdown_players = gs.showdown_players.size();
    gs.num_winners = gs.winners.size();
}

void TexasHoldEm::m_betting_loop(GameState& gs, int starting_plyr)
{
    int i = starting_plyr;
    while (!m_is_betting_over(gs.round))
    {
        if (!full_player_list[i].is_player_eliminated() &&
            full_player_list[i].is_player_active() &&
            (full_player_list[i].get_chip_count() > 0))
        {
            // Calculate chip requirements
            gs.pot_chip_count = pot.get_chip_count();
            gs.sum_prev_bets = pot.get_player_bets_by_round(i, gs.round);
            gs.chips_to_call = gs.min_bet - gs.sum_prev_bets;
            gs.max_bet = full_player_list[i].get_chip_count();
            gs.player_idx = i;
            if (gs.chips_to_call >= gs.max_bet)
            {
                gs.legal_actions = { Action::All_In, Action::Fold };
                gs.chips_to_call = gs.max_bet;
                gs.min_to_raise = gs.max_bet;
                m_update_game_state(gs);
                full_player_list[i].player_act(gs);
                assert(((gs.player_bet == gs.max_bet) || (gs.player_bet == 0))
                    && "Invalid player bet!");
            }
            else
            {
                gs.legal_actions = { Action::Fold };
                if (gs.all_in_below_min_raise && (gs.raise_player_idx == i))
                {
                    gs.legal_to_raise = false;
                }
                else
                {
                    gs.legal_to_raise = true;
                }
                if (gs.legal_to_raise)
                {
                    gs.legal_actions.push_back(Action::All_In);
                }
                if (gs.chips_to_call == 0)
                {
                    gs.legal_actions.push_back(Action::Check);
                }
                else if (gs.chips_to_call > 0)
                {
                    gs.legal_actions.push_back(Action::Call);
                }
                if (gs.legal_to_raise &&
                    !gs.raise_active &&
                    (gs.max_bet > (gs.min_to_raise + gs.chips_to_call))
                    )
                {
                    if (gs.chips_to_call == 0)
                    {
                        gs.legal_actions.push_back(Action::Bet);
                    }
                    else
                    {
                        gs.legal_actions.push_back(Action::Raise);
                        gs.raise_player_idx = i;
                    }
                }
                else if (gs.legal_to_raise &&
                    gs.raise_active &&
                    (gs.max_bet > (gs.min_to_raise + gs.chips_to_call))
                    )
                {
                    if (gs.action_number == 0)
                    {
                        // First action of the game by the UTG player
                        gs.legal_actions.push_back(Action::Raise);
                    }
                    else
                    {
                        gs.legal_actions.push_back(Action::Re_Raise);
                    }
                }
                if (!gs.legal_to_raise && (gs.chips_to_call > 0))
                {
                    gs.max_bet = gs.chips_to_call;
                }
                m_update_game_state(gs);
                full_player_list[i].player_act(gs);
                assert((((gs.player_bet >= gs.chips_to_call) &&
                    (gs.player_bet <= gs.max_bet)) ||
                    (gs.player_bet == 0)) && "Invalid player bet!");
            }
            if ((gs.player_bet == gs.max_bet) &&
                (gs.player_bet < (gs.chips_to_call + gs.min_to_raise)))
            {
                assert((gs.player_action == Action::All_In) &&
                    "Action should be all-in!");
                if (gs.player_bet < gs.chips_to_call)
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
                    gs.all_in_below_min_raise = true;
                    gs.min_bet = gs.player_bet + gs.sum_prev_bets;
                }
            }
            else if (gs.player_bet > gs.min_bet)
            {
                // Player has raised
                assert((gs.player_bet >= (gs.chips_to_call + gs.min_to_raise))
                    && "Invalid player raise!");
                assert((gs.player_action == Action::Raise ||
                    gs.player_action == Action::Re_Raise ||
                    gs.player_action == Action::Bet ||
                    gs.player_action == Action::All_In) &&
                    "Action should be all-in/bet/raise/re-raise!");
                gs.min_to_raise = gs.player_bet - gs.chips_to_call;
                gs.min_bet = gs.player_bet + gs.sum_prev_bets;
                gs.raise_active = true;
            }
            else if ((gs.player_bet == 0) && (gs.chips_to_call > 0))
            {
                // Player has folded
                // No change to min bet or min raise
                assert((gs.player_action == Action::Fold) &&
                    "Action should be fold!");
                full_player_list[i].fold_player();
            }
            else if ((gs.player_bet == 0) && (gs.chips_to_call == 0))
            {
                // Player has checked
                // No change to min bet or min raise
                assert((gs.player_action == Action::Check) &&
                    "Action should be check!");
            }
            else if ((gs.player_bet == gs.chips_to_call) &&
                (gs.chips_to_call != 0))
            {
                // Player has called
                // No change to min bet or min raise
                assert((gs.player_action == Action::Call) &&
                    "Action should be call!");
            }
            // Store game state
            game_hist.states.push_back(gs);
            gs.action_number++;
            pot.add_chips(gs.player_bet, i, gs.round);
        }
        i = (i + 1) % initial_num_players;
    }
}

void TexasHoldEm::m_pre_flop(GameState gs)
{
    // The player after the big blind always goes first before the flop
    // This is the Under The Gun (UTG) player for more than 2 players
    // For heads-up poker the dealer/small blind goes first
    int starting_player = (m_bb_idx + 1) % initial_num_players;
    gs.min_bet = BLINDS_STRUCTURE.at(num_players).second;
    gs.min_to_raise = BLINDS_STRUCTURE.at(num_players).second;
    gs.num_active_players = m_get_num_active_players();
    gs.round = Round::Pre_Flop;
    gs.legal_to_raise = true;
    gs.raise_active = false;
    gs.raise_player_idx = m_bb_idx;
    gs.all_in_below_min_raise = false;
    gs.action_number = 0;
    gs.pot_chip_count = pot.get_chip_count();
    m_betting_loop(gs, starting_player);
    if (m_get_num_active_not_allin_players() == 1)
    {
        m_showdown(gs);
    }
    else
    {
        m_flop(gs);
    }
}

void TexasHoldEm::m_flop(GameState gs)
{
    // After the flop the small blind always go first (player after button)
    gs.round = Round::Flop;
    gs.min_bet = 0;
    gs.min_to_raise = BLINDS_STRUCTURE.at(num_players).second;
    gs.num_active_players = m_get_num_active_players();
    gs.legal_to_raise = true;
    gs.raise_active = false;
    gs.raise_player_idx = m_bb_idx;
    gs.all_in_below_min_raise = false;
    gs.pot_chip_count = pot.get_chip_count();
    dealer.deal_flop(full_player_list);
    m_betting_loop(gs, m_sb_idx);
    if (m_get_num_active_not_allin_players() == 1)
    {
        m_showdown(gs);
    }
    else
    {
        m_turn(gs);
    }
}

void TexasHoldEm::m_turn(GameState gs)
{
    gs.round = Round::Turn;
    gs.min_bet = 0;
    gs.min_to_raise = BLINDS_STRUCTURE.at(num_players).second;
    gs.num_active_players = m_get_num_active_players();
    gs.legal_to_raise = true;
    gs.raise_active = false;
    gs.raise_player_idx = m_bb_idx;
    gs.all_in_below_min_raise = false;
    gs.pot_chip_count = pot.get_chip_count();
    dealer.deal_turn(full_player_list);
    m_betting_loop(gs, m_sb_idx);
    if (m_get_num_active_not_allin_players() == 1)
    {
        m_showdown(gs);
    }
    else
    {
        m_river(gs);
    }
}

void TexasHoldEm::m_river(GameState gs)
{
    gs.round = Round::River;
    gs.min_bet = 0;
    gs.min_to_raise = BLINDS_STRUCTURE.at(num_players).second;
    gs.num_active_players = m_get_num_active_players();
    gs.legal_to_raise = true;
    gs.raise_active = false;
    gs.raise_player_idx = m_bb_idx;
    gs.all_in_below_min_raise = false;
    gs.pot_chip_count = pot.get_chip_count();
    dealer.deal_river(full_player_list);
    m_betting_loop(gs, m_sb_idx);
    m_showdown(gs);
}

void TexasHoldEm::m_showdown(GameState gs)
{
    gs.round = Round::Showdown;
    m_update_game_state(gs);
    for (auto& player : full_player_list)
    {
        if (!player.is_player_eliminated() && player.is_player_active())
        {
            gs.showdown_players.push_back(
                ShowdownStruct(
                    player.player_idx,
                    player.get_best_hand(),
                    player.get_best_hand_rank(),
                    pot.get_total_player_bets(player.player_idx)
                ));
        }
    }
    //game_hist.states.push_back(gs);
    gs.num_showdown_players = gs.showdown_players.size();
    m_end_round(gs);
}

int TexasHoldEm::m_player_closest_to_left_of_dealer(vector<int> tie_list)
{
    int min_distance = MAX_PLAYER_COUNT;
    for (size_t i = 0; i < tie_list.size(); i++)
    {
        for (int dist = 0; dist < MAX_PLAYER_COUNT; dist++)
        {
            if ((m_button_idx + dist) % initial_num_players == tie_list[i])
            {
                if (dist < min_distance)
                {
                    min_distance = dist;
                }
            }
        }
    }
    return min_distance;
}

// Need to check if game moves to next phase or if all other players folded
// Need to decide where actions taken by players will be verified
// Probably makes most sense to check inside the player class, but will 
// need to pass the player the current bet/raise amount, and previous
// player action.  Probably also pass what they've contributed so far?
// Or maybe pass a list of legal actions


// Tied hadn split the pot evenly.  if tehre is a odd chip left, it goes to the
// player closest to the left of the dealer button

void TexasHoldEm::m_end_round(GameState gs)
{
    // Determine winner of round
    if (m_get_num_active_players() == 1)
    {
        // Other players folded before the showdown
        int winner_idx = gs.showdown_players[0].player_idx;
        unsigned player_bet = gs.showdown_players[0].total_chips_bet;
        // Pay the winner of the round
        unsigned winnings = pot.m_pay_to_winner(player_bet);
        gs.showdown_players[0].chips_won = winnings;
        full_player_list[winner_idx].win_chips(winnings);
    }
    else
    {
        // The game went to the showdown, so must determine winner
        // Ties and side pots are possible, so check for both
        // Sort showdown hands from strongest to weakest
        std::sort(gs.showdown_players.begin(), gs.showdown_players.end(),
            std::greater<ShowdownStruct>());
        int tie_count = 1;
        int winner_idx = 0;
        unsigned smallest_bet;
        unsigned winnings;
        unsigned remainder;
        unsigned split_winnings;
        int closest_plyr;
        vector<int> tie_player_indices;
        // This loop will continue until the pot and any side pots have been
        // emptied out and paid to the respective winners
        while (pot.m_chip_count > 0)
        {
            tie_count = 1;
            while (1)
            {
                // Check to see if there is a tie for the strongest hand
                if (gs.showdown_players[winner_idx] ==
                    gs.showdown_players[winner_idx + tie_count])
                {
                    tie_count++;
                }
                else
                {
                    break;
                }
            }
            if (tie_count > 1)
            {
                // There's been a tie, and so we must split the pot or side pot
                for (int i = winner_idx; i < winner_idx + tie_count; i++)
                {
                    if ((i > winner_idx) &&
                        (gs.showdown_players[i].total_chips_bet ==
                            gs.showdown_players[i - 1].total_chips_bet))
                    {
                        // Tied winners bet same amount, so pot has already
                        // been split in the previous loop.
                        break;
                    }
                    // Showdown structs are sorted so that in case of a tie
                    // the player who bet the least amount is first in the vector
                    smallest_bet = gs.showdown_players[i].total_chips_bet;
                    winnings = pot.m_pay_to_winner(smallest_bet);
                    remainder = winnings % tie_count;
                    split_winnings = (winnings - remainder) / tie_count;
                    // Determine player closest to left of dealer button
                    for (int j = winner_idx; j < winner_idx + tie_count; j++)
                    {
                        tie_player_indices.push_back(gs.showdown_players[j].player_idx);
                    }
                    closest_plyr = m_player_closest_to_left_of_dealer(tie_player_indices);
                    tie_player_indices.clear();
                    // Pay each winner their equal portion of the pot or side pot
                    for (int k = winner_idx; k < winner_idx + tie_count; k++)
                    {
                        full_player_list[k].win_chips(split_winnings);
                        gs.showdown_players[k].chips_won += split_winnings;
                    }
                    // Pay the odd chip(s) to the closest to the left of the dealer
                    full_player_list[closest_plyr].win_chips(remainder);
                    gs.showdown_players[closest_plyr].chips_won += remainder;
                }
                winner_idx++;
            }
            else
            {
                // No tie, so no need to split the pot
                int bet = gs.showdown_players[winner_idx].total_chips_bet;
                int winner = gs.showdown_players[winner_idx].player_idx;
                winnings = pot.m_pay_to_winner(bet);
                full_player_list[winner].win_chips(winnings);
                gs.showdown_players[winner_idx].chips_won = winnings;
                winner_idx++;
            }
        }
    }
    // Check if players eliminated, and verify the total number of chips in the
    // tournament hasn't changed.
    unsigned total_chips = 0;
    unsigned player_chip_cnt;
    for (auto& player : full_player_list)
    {
        if (!player.is_player_eliminated())
        {
            player_chip_cnt = player.get_chip_count();
            total_chips += player_chip_cnt;
            if (player_chip_cnt == 0)
            {
                player.eliminate_player();
                num_players--;
            }
        }
    }
    assert((total_chips == MAX_BUY_IN * initial_num_players)
        && "Invalid total chip count!");
    // Update game state and add to game history.  Add game history to
    // Tournament history.
    m_update_game_state(gs);
    game_hist.states.push_back(gs);
    game_hist.num_states = game_hist.states.size();
    tourn_hist.games.push_back(game_hist);
    tourn_hist.num_games = tourn_hist.games.size();
    if (num_players == 1)
    {
        m_tournament_completed = true;
        write_tournamenthistory("test_game.bin", tourn_hist);
        cout << "Write tournament history successful!" << endl;
        return;
    }
    if (tourn_hist.games.size() >= 10)
    {
        tourn_hist.num_games = tourn_hist.games.size();
        write_tournamenthistory("test_game.bin", tourn_hist);
        cout << "Write tournament history successful!" << endl;
        exit(-1);
    }
    // All players return their cards
    for (auto& player : full_player_list)
    {
        player.return_cards();
    }
    dealer.clear_cards_from_table();
    // Move blinds, increment game count, clear game history, and start
    // next game
    m_move_blinds();
    game_hist.game_number++;
    game_hist.states.clear();
    gs.num_showdown_players = 0;
    gs.showdown_players.clear();
    m_begin_round(gs);
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
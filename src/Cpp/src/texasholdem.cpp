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
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"
#include "playback.hpp"
#include "player.hpp"
#include "serialize.hpp"
#include "storage.hpp"
#include "texasholdem.hpp"
// Using statements
using std::cout;
using std::endl;
using std::sort;
using std::vector;
using namespace constants;

/* Texas Hold 'Em Method Definitions
******************************************************************************/
void TexasHoldEm::begin_tournament()
{
    /**
     * Initialize data structures and begin the poker tournament.
    */
    if (m_tournament_started)
    {
        cout << "Tournament already in progress!" << endl;
        return;
    }
    m_tournament_started = true;
    // Initialize players
    for (int i = 0; i < initial_num_players; i++)
    {
        full_player_list.push_back(Player(i, MAX_BUY_IN, AI_Type::Random, rng));
    }
    // Initalize data storage structures
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
    gs.initial_num_players = initial_num_players;
    gs.num_players = tourn_hist.initial_player_count;
    for (int i = 0; i < initial_num_players; i++)
    {
        gs.ai_types.push_back(full_player_list[i].ai_type);
        gs.remaining_players.push_back(true);
    }
    m_begin_game(gs);
}

void TexasHoldEm::m_begin_game(GameState gs)
{
    /**
     * Begin poker game by
    */
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
            if (!full_player_list[j].is_player_eliminated())
            {
                if (full_player_list[j].blind != Blind::No_Blind) { break; }
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
        gs.raise_player_idx = m_sb_idx;
    }
    if ((bb_pmt < bb))
    {
        full_player_list[m_bb_idx].prev_action = Action::All_In;
        gs.all_in_below_min_raise = true;
        gs.raise_player_idx = m_bb_idx;
    }
    if ((sb_pmt == sb) && (bb_pmt == bb))
    {
        gs.all_in_below_min_raise = false;
        gs.raise_player_idx = m_bb_idx;
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
        gs.hand_ranks.push_back(full_player_list[i].get_best_hand_rank());
        gs.remaining_players[i] = !full_player_list[i].is_player_eliminated();
    }
    m_update_game_state(gs);
    m_pre_flop(gs);
}

void TexasHoldEm::m_pre_flop(GameState gs)
{
    /**
     * Start the pre-flop round of betting.
     *
     * The pre-flop is the round of betting that occurs after the dealer has
     * dealt all players their two hole cards.
     *
     * The player after the big blind always goes first before the flop.
     * This is the Under The Gun (UTG) player for more than 3 players and
     * the dealer for three players.
     * For heads-up poker (two players) the dealer/small blind goes first.
    */
    int starting_player = (m_bb_idx + 1) % initial_num_players;
    // Initialize game state for pre-flop
    gs.min_bet = BLINDS_STRUCTURE.at(num_players).second;
    gs.min_to_raise = BLINDS_STRUCTURE.at(num_players).second;
    gs.num_active_players = m_get_num_active_players();
    gs.round = Round::Pre_Flop;
    gs.legal_to_raise = true;
    gs.raise_active = false;
    gs.action_number = 0;
    gs.pot_chip_count = pot.get_chip_count();
    // Begin the betting
    m_betting_loop(gs, starting_player);
    if (m_get_num_active_not_allin_players() <= 1)
    {
        // Either all other players folded or went all-in, deal all cards
        dealer.deal_flop(full_player_list);
        dealer.deal_turn(full_player_list);
        dealer.deal_river(full_player_list);
        m_showdown(gs);  // Determine winner of game
    }
    else
    {
        m_flop(gs);  // Proceed to flop round of betting
    }
}

void TexasHoldEm::m_flop(GameState gs)
{
    /**
     * Start the flop round of betting.
     *
     * The flop is the round of betting that occurs after the dealer has
     * dealt the three flop cards.
     *
     * After the flop the first active player after the button acts first.
     * This will be the small blind if the player is still active.
    */
    gs.round = Round::Flop;
    gs.min_bet = 0;
    gs.min_to_raise = BLINDS_STRUCTURE.at(num_players).second;
    gs.num_active_players = m_get_num_active_players();
    gs.legal_to_raise = true;
    gs.raise_active = false;
    gs.raise_player_idx = -1;
    gs.all_in_below_min_raise = false;
    gs.pot_chip_count = pot.get_chip_count();
    dealer.deal_flop(full_player_list);
    m_betting_loop(gs, m_sb_idx);
    if (m_get_num_active_not_allin_players() <= 1)
    {
        // Either all other players folded or went all-in, deal all cards
        dealer.deal_turn(full_player_list);
        dealer.deal_river(full_player_list);
        m_showdown(gs);  // Determine winner of game
    }
    else
    {
        m_turn(gs);  // Proceed to turn round of betting
    }
}

void TexasHoldEm::m_turn(GameState gs)
{
    /**
     * Start the turn round of betting.
     *
     * The turn is the round of betting that occurs after the dealer has
     * dealt the turn card.
     *
     * After the flop the first active player after the button acts first.
     * This will be the small blind if the player is still active.
    */
    gs.round = Round::Turn;
    gs.min_bet = 0;
    gs.min_to_raise = BLINDS_STRUCTURE.at(num_players).second;
    gs.num_active_players = m_get_num_active_players();
    gs.legal_to_raise = true;
    gs.raise_active = false;
    gs.raise_player_idx = -1;
    gs.all_in_below_min_raise = false;
    gs.pot_chip_count = pot.get_chip_count();
    dealer.deal_turn(full_player_list);
    m_betting_loop(gs, m_sb_idx);
    if (m_get_num_active_not_allin_players() <= 1)
    {
        // Either all other players folded or went all-in, deal all cards
        dealer.deal_river(full_player_list);
        m_showdown(gs);  // Determine winner of game
    }
    else
    {
        m_river(gs);  // Proceed to river round of betting
    }
}

void TexasHoldEm::m_river(GameState gs)
{
    /**
     * Start the river round of betting.
     *
     * The turn is the round of betting that occurs after the dealer has
     * dealt the river card.
     *
     * After the flop the first active player after the button acts first.
     * This will be the small blind if the player is still active.
    */
    gs.round = Round::River;
    gs.min_bet = 0;
    gs.min_to_raise = BLINDS_STRUCTURE.at(num_players).second;
    gs.num_active_players = m_get_num_active_players();
    gs.legal_to_raise = true;
    gs.raise_active = false;
    gs.raise_player_idx = -1;
    gs.all_in_below_min_raise = false;
    gs.pot_chip_count = pot.get_chip_count();
    dealer.deal_river(full_player_list);
    m_betting_loop(gs, m_sb_idx);
    m_showdown(gs);  // Determine winner of game
}

void TexasHoldEm::m_showdown(GameState gs)
{
    /**
     * Store the best hand of each player who made it to the showdown.
     *
     * Also stores the hand rank of each player's hand as well as how many
     * chips each player bet.  The latter will be used to distribute side pots.
    */
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
    gs.num_showdown_players = gs.showdown_players.size();
    game_hist.states.push_back(gs);
    if (debug) { print_state(gs); }
    m_end_game(gs);
}


void TexasHoldEm::m_end_game(GameState gs)
{
    /**
     * Determine winner and start the next game or serialize the tournament.
     *
     * If more than one player remains return all cards to the deck and start
     * the next game.  Otherwise, the tournament is concluded and the game
     * states are serialized and saved to disk.
     *
    */
    gs.round = Round::Game_Result;
    determine_game_winner(gs);
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
                tourn_hist.finishing_order.push_back(player.player_idx);
                tourn_hist.game_eliminated.push_back(game_hist.game_number);
            }
        }
    }
    assert((total_chips == MAX_BUY_IN * initial_num_players)
        && "Invalid total chip count!");
    // Update game state and add to game history.  Add game history to
    // Tournament history.
    m_update_game_state(gs);
    game_hist.states.push_back(gs);
    if (debug) { print_state(gs); }
    game_hist.num_states = game_hist.states.size();
    tourn_hist.games.push_back(game_hist);
    tourn_hist.num_games = tourn_hist.games.size();
    if (num_players == 1)
    {
        m_tournament_completed = true;
        cout << "Tournament is complete!" << endl;
        int winner_idx = gs.showdown_players[0].player_idx;
        tourn_hist.finishing_order.push_back(winner_idx);
        tourn_hist.game_eliminated.push_back(game_hist.game_number);
        write_tournamenthistory("test_game.bin", tourn_hist);
        cout << "Write tournament history successful!" << endl;
        return;
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
    m_begin_game(gs);
}

void TexasHoldEm::determine_game_winner(GameState& gs)
{
    /**
     * Determine the winner of the game and distribute the winnings.
     *
     * The ShowdownStruct contains the logic for determining which hands are
     * strongest.  The ShowdownStructs are sorted in descending order of hand
     * strength.  There is a check for potential ties, and side pots are
     * distributed according to the number of chips bet by each player.
    */
    if (m_get_num_active_players() == 1)
    {
        // Other players folded before the showdown
        int winner_idx = gs.showdown_players[0].player_idx;
        unsigned player_bet = gs.showdown_players[0].total_chips_bet;
        // Pay the winner of the round
        unsigned winnings = pot.m_pay_to_winner(player_bet);
        gs.showdown_players[0].chips_won = winnings;
        full_player_list[winner_idx].win_chips(winnings);
        if (pot.m_chip_count > 0)
        {
            /*
            An edge case that occurs more often during heads-up play.
            The big blind puts one player all-in, but small blind folds
            rather than call the big blind.  All-in player wins, but can
            only take as many chips from each player as he bet.
            If his stack was smaller than the small blind, then the small
            blind player should be returned the difference in chips.  But the
            small blind player already folded, thus leaving chips in the pot.
            */
            unsigned player_bet;
            for (int i = 0; i < initial_num_players; i++)
            {
                // Return remainder of folded players' bet to them
                player_bet = pot.get_total_player_bets(i);
                full_player_list[i].win_chips(player_bet);
                pot.m_clear_players_bets(i);
                pot.m_chip_count -= player_bet;
                if (pot.m_chip_count == 0) { break; }
            }
        }
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
                for (int i = winner_idx; i < tie_count; i++)
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
                    remainder = winnings % (tie_count - i);
                    split_winnings = (winnings - remainder) / (tie_count - i);
                    // Determine player closest to left of dealer button
                    for (int j = winner_idx; j < winner_idx + tie_count - i; j++)
                    {
                        tie_player_indices.push_back(gs.showdown_players[j].player_idx);
                    }
                    closest_plyr = m_player_closest_to_left_of_dealer(tie_player_indices);
                    tie_player_indices.clear();
                    // Pay each winner their equal portion of the pot or side pot
                    // Pay the odd chip(s) to the closest to the left of the dealer
                    int plyr_idx;
                    full_player_list[closest_plyr].win_chips(remainder);
                    for (int k = winner_idx; k < winner_idx + tie_count - i; k++)
                    {
                        gs.showdown_players[k].chips_won += split_winnings;
                        plyr_idx = gs.showdown_players[k].player_idx;
                        full_player_list[plyr_idx].win_chips(split_winnings);
                        if (plyr_idx == closest_plyr)
                        {
                            gs.showdown_players[k].chips_won += remainder;
                        }
                    }
                    winner_idx++;
                }
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
}

void TexasHoldEm::m_betting_loop(GameState& gs, int starting_plyr)
{
    /**
     * Rotate through the active players until the betting round is over.
     *
     * Provides the current player with a list of legal actions and the
     * minimum amount of chips the player must bet to call, and the minimum
     * amount of chips to raise.  Updates the game state and then passes
     * the game state to the player to choose an action.
     *
     * The action chosen by the player and the amount of chips bet by the
     * player are validated to ensure that they comply with the rules, and the
     * game state is stored.
     *
     * The player's bet is not added to the pot until after the game state is
     * stored.  The game state is to contain information that has been updated
     * to the point just prior to the player's action.
     *
     * @param gs is a struct containing the current game state.
     * @param starting_plyr is the index of the player who acts first.
    */
    int plyr_idx = starting_plyr;
    while (!m_is_betting_over(gs))
    {
        if (!full_player_list[plyr_idx].is_player_eliminated() &&
            full_player_list[plyr_idx].is_player_active() &&
            (full_player_list[plyr_idx].get_chip_count() > 0))
        {
            determine_legal_actions(gs, plyr_idx);
            m_update_game_state(gs);
            full_player_list[plyr_idx].player_act(gs);
            validate_player_action(gs, plyr_idx);
            // Store game state
            game_hist.states.push_back(gs);
            if (debug) { print_state(gs); }
            gs.action_number++;
            pot.add_chips(gs.player_bet, plyr_idx, gs.round);
        }
        plyr_idx = (plyr_idx + 1) % initial_num_players;
    }
}

void TexasHoldEm::determine_legal_actions(GameState& gs, int plyr_idx)
{
    /**
     * Determine what actions and betting amounts are legal for the player.
     *
     * Provides the current player with a list of legal actions and the
     * minimum amount of chips the player must bet to call, and the minimum
     * amount of chips to raise.  Updates the game state and then passes
     * the game state to the player to choose an action.
     *
     * @param gs is a struct containing the game state
     * @param plyr_idx is the player index
    */
    // Calculate chip requirements
    gs.pot_chip_count = pot.get_chip_count();
    gs.sum_prev_bets = pot.get_player_bets_by_round(plyr_idx, gs.round);
    gs.chips_to_call = gs.min_bet - gs.sum_prev_bets;
    gs.max_bet = full_player_list[plyr_idx].get_chip_count();
    gs.player_idx = plyr_idx;
    if (gs.chips_to_call >= gs.max_bet)
    {
        gs.legal_actions = { Action::All_In, Action::Fold };
        gs.chips_to_call = gs.max_bet;
        gs.min_to_raise = gs.max_bet;
    }
    else
    {
        gs.legal_actions = { Action::Fold };
        if (gs.all_in_below_min_raise && (gs.raise_player_idx == plyr_idx))
        {
            // No other player has re-raised since the raise that 
            // caused the all-in below the min raise, therefore
            // betting action is not re-opened
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
                // Post-flop first bet
                gs.legal_actions.push_back(Action::Bet);
            }
            else
            {
                // Pre-flop big blind is first bet, so player raises
                gs.legal_actions.push_back(Action::Raise);
            }
        }
        else if (gs.legal_to_raise &&
            gs.raise_active &&
            (gs.max_bet > (gs.min_to_raise + gs.chips_to_call))
            )
        {
            gs.legal_actions.push_back(Action::Re_Raise);
        }
        if (!gs.legal_to_raise && (gs.chips_to_call > 0))
        {
            gs.max_bet = gs.chips_to_call;
        }
    }
}

void TexasHoldEm::validate_player_action(GameState& gs, int plyr_idx)
{
    /**
     * Validate that the player's action and bet complies with the rules.
     *
     * @param gs is a struct containing the game state
     * @param plyr_idx is the player index
    */
    assert(PlayerAI::legal_act(gs.player_action, gs) &&
        "Invalid player action!");
    if ((gs.player_bet == gs.max_bet) &&
        (gs.player_bet != gs.chips_to_call) &&
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
        gs.raise_player_idx = plyr_idx;
    }
    else if ((gs.player_bet == 0) && (gs.chips_to_call > 0))
    {
        // Player has folded
        // No change to min bet or min raise
        assert((gs.player_action == Action::Fold) &&
            "Action should be fold!");
        full_player_list[plyr_idx].fold_player();
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
        assert(((gs.player_action == Action::Call) ||
            (gs.player_action == Action::All_In &&
                gs.player_bet == gs.max_bet)) &&
            "Action should be call!");
    }
}

bool TexasHoldEm::m_is_betting_over(GameState& gs) const
{
    /**
     * Determine if betting round has ended.
     *
     * The betting round ends once all active and not all-in players have
     * acted at least once and the amount of chips bet is equal amongst all
     * active and not all-in players, *and* is equal or greater than the
     * largest all-in bet.
     *
     * This condition is necessary to handle a number of edge cases:
     *
     * 1. At the beginning of every post-flop betting round the chips bet by
     * each active player is equal (zero), so must ensure every player acts
     * at least once before ending betting.
     *
     * 2. If each player checks in a post-flop betting round, the round should
     * end after each player has acted once with a total of zero chips bet.
     *
     * 3. In the pre-flop it is possible that all players will call the big
     * blind, and all active player bets will be equal before the big blind's
     * turn.  But the big blind still must get a chance to act before the round
     * ends.
     *
     * 4. The blinds can potentially put a player all-in immediately at the
     * start of the game, and so they will not act for the rest of the game
     * and should not be added to the minimum count of player actions before
     * the round ends.
     *
     * 5. The active and not all-in players must still bet enough to cover any
     * all-in bet or else go all-in themselves.  They cannot simply call
     * each other's bets and proceed to the next round with a smaller bet.
    */
    // Use static variables to keep track of the start of each betting round
    static int start_action = 0;
    static Round round = Round::Pre_Flop;
    static int adjusted_num_players = MAX_PLAYER_COUNT;
    if (gs.action_number == 0)
    {
        start_action = 0;
        round = Round::Pre_Flop;
        adjusted_num_players = num_players;
    }
    if (gs.round != round)
    {
        round = gs.round;
        start_action = gs.action_number;
    }
    if (m_get_num_active_players() == 1 ||  // All but one player folded
        m_get_num_active_not_allin_players() == 0)  // Every player all-in
    {
        return true;
    }
    // Ensure that betting doesn't end before every player has had a chance to
    // act at least once.
    if (gs.action_number == start_action)
    {
        adjusted_num_players = m_get_num_active_not_allin_players();
    }
    if (gs.action_number < start_action + adjusted_num_players)
    {
        return false;  // Not all players have acted during the round
    }
    unsigned num_chips_bet = 0;
    unsigned bet = 0;
    unsigned all_in_bet = 0;
    bool first_active_player = true;
    for (const auto& player : full_player_list)
    {
        if (player.is_player_active())
        {
            if ((player.get_chip_count() != 0))  // Active and not all-in
            {
                // Store amount first active player has placed into the pot
                if (first_active_player)
                {
                    first_active_player = false;
                    num_chips_bet = pot.get_player_bets_by_round(
                        player.player_idx,
                        gs.round);
                }
                else
                {
                    // Betting is not over if all active and not all-in players
                    // have not bet the same amount
                    bet = pot.get_player_bets_by_round(
                        player.player_idx,
                        gs.round);
                    if (bet != num_chips_bet) { return false; }
                }
            }
            else  // Active, all-in player
            {
                // Keep track of the largest all-in bet made this round
                bet = pot.get_player_bets_by_round(
                    player.player_idx,
                    gs.round);
                if (bet > all_in_bet) { all_in_bet = bet; }
            }
        }
    }
    // At start of round blinds could put players all-in, and if the remaining
    // players haven't either bet at least as much then betting isn't over
    if (num_chips_bet < all_in_bet) { return false; }
    // All players have acted and player bets are equal and sufficient
    return true;
}

void TexasHoldEm::m_move_blinds()
{
    /**
     * Move dealer button to the next eligible (i.e. not eliminated) player.
     *
     * Also moves small blind and big blind.  For heads-up play (two players)
     * the dealer is also the small blind.
    */
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

void TexasHoldEm::m_update_game_state(GameState& gs)
{
    /**
     * Update game state with current game and player information.
    */
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
    gs.remaining_players = m_get_vec_remaining_players();
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
    gs.num_available_cards = gs.available_cards.size();
    gs.num_legal_actions = gs.legal_actions.size();
    gs.num_showdown_players = gs.showdown_players.size();
}

int TexasHoldEm::m_get_num_active_players() const
{
    /**
     * Return the number of players who are still active in the current game.
     *
     * This count does not include players who have folded or who have been
     * eliminated from the tournament, but does include all-in players.
    */
    int num_active = 0;
    for (const auto& player : full_player_list)
    {
        if (player.is_player_active()) { num_active++; }
    }
    return num_active;
}

int TexasHoldEm::m_get_num_active_not_allin_players() const
{
    /**
     * Return the number of players who are active and not all-in.
     *
     * This count does not include players who have folded, who have been
     * eliminated from the tournament, or have gone all-in.
    */
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

int TexasHoldEm::m_player_closest_to_left_of_dealer(vector<int> tie_list)
{
    /**
     * Return index of active player closest to the left of the dealer button.
     *
     * Iterate through the list of tied players and return the index of the
     * player who is closest to the left of the dealer button.  This player
     * will receive any odd chip(s) that cannot be divided evenly amongst the
     * tied players.
    */
    int min_distance = MAX_PLAYER_COUNT;
    int closest_plyr_idx = -1;
    for (size_t i = 0; i < tie_list.size(); i++)
    {
        for (int dist = 0; dist < MAX_PLAYER_COUNT; dist++)
        {
            if ((m_button_idx + dist) % initial_num_players == tie_list[i])
            {
                if (dist < min_distance)
                {
                    min_distance = dist;
                    closest_plyr_idx = tie_list[i];
                }
                break;
            }
        }
    }
    assert((closest_plyr_idx >= 0) && "Closest player is invalid!");
    return closest_plyr_idx;
}

vector<int> TexasHoldEm::m_get_vec_active_players() const
{
    vector<int> active_players;
    for (const auto& player : full_player_list)
    {
        active_players.push_back(player.is_player_active());
    }
    return active_players;
}

vector<int> TexasHoldEm::m_get_vec_remaining_players() const
{
    vector<int> remaining_players;
    for (const auto& player : full_player_list)
    {
        remaining_players.push_back(!player.is_player_eliminated());
    }
    return remaining_players;
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
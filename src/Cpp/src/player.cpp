/******************************************************************************
* Classes representing a Texas Hold 'Em player.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <array>
#include <algorithm>    // For std::find()
#include <cassert>
#include <iostream>
#include <memory>       // For static_pointer_cast
#include <random>       // For shuffle(), uniform_int_distribution<>
#include <utility>      // For std::pair, std::to_underlying()
// Project headers
#include "player.hpp"
#include "constants.hpp"
#include "storage.hpp"
#include "texasholdem.hpp"
// Using statements
using std::array;
using std::cout;
using std::endl;
using std::pair;
using std::static_pointer_cast;
using std::to_underlying;
using std::vector;
using namespace constants;

/* PlayerAI Method Definitions
******************************************************************************/
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

/* RandomAI Method Definitions
******************************************************************************/
void RandomAI::player_act(GameState& gs)
{
    /**
     * Player AI randomly chooses from the list of legal actions.
     *
     * @param gs is the current game state.
     * @return Modifies the game state with the chosen action and bet amount.
    */
    m_set_distribution_max(static_cast<int>(gs.legal_actions.size() - 1));
    int action_idx = m_uniform_dist(rng);
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
        m_set_distribution_max(gs.max_bet);
        unsigned random_bet = static_cast<unsigned>(m_uniform_dist(rng));
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

void RandomAI::m_set_distribution_max(int max)
{
    /**
     * Set the new maximum value of a uniform distribution of integers.
     *
     * @param max is the maximum integer value of the uniform distribution.
     * @return Replace the data member with a new uniform distribution.
    */
    m_uniform_dist = std::uniform_int_distribution<>(0, max);
}

/* ScriptedAI Method Definitions
******************************************************************************/
void ScriptedAI::player_act(GameState& gs)
{
    /**
     * AI performs scripted actions to test a particular scenario.
     *
     * @param gs is the current game state.
     * @return Modifies the game state with the chosen action and bet amount.
    */
    gs.player_action = scripted_actions[m_action_number].first;
    gs.player_bet = scripted_actions[m_action_number].second;
    m_action_number++;
}

/* CheckCallAI Method Definitions
******************************************************************************/
void CheckCallAI::player_act(GameState& gs)
{
    /**
     * AI checks if possible, otherwise calls.
     *
     * Used for Monte Carlo simulations.
     *
     * @param gs is the current game state.
     * @return Modifies the game state with the chosen action and bet amount.
    */
    if (legal_act(Action::Check, gs))
    {
        gs.player_action = Action::Check;
        gs.player_bet = 0;
    }
    else
    {
        gs.player_action = Action::Call;
        gs.player_bet = gs.chips_to_call;
    }
}

/* HeuristicAI Method Definitions
******************************************************************************/
void HeuristicAI::check_or_fold(GameState& gs)
{
    /**
     * Player checks if it is legal, otherwise folds.
    */
    if (legal_act(Action::Check, gs))
    {
        gs.player_action = Action::Check;
        gs.player_bet = 0;
    }
    else
    {
        gs.player_action = Action::Fold;
        gs.player_bet = 0;
    }
}

void HeuristicAI::player_act(GameState& gs)
{
    /**
     * AI uses heuristics to determine player action.
     *
     * @param gs is the current game state.
     * @return Modifies the game state with the chosen action, bet amount, and
     * estimated win probability from a Monte Carlo simulation.
    */
    gs.win_perc[gs.player_idx] = -1;
    if (gs.round == Round::Pre_Flop && gs.action_number < gs.num_players)
    {
        hand_position = assess_hole_cards(gs);
        if (hand_position == Position::Not_Playable)
        {
            check_or_fold(gs);
            return;
        }
    }
    if (!legal_act(Action::All_In, gs))
    {
        // Won't be able to raise/re-raise/all-in
        if (legal_act(Action::Call, gs))
        {
            gs.player_action = Action::Call;
            gs.player_bet = gs.chips_to_call;
        }
        else
        {
            check_or_fold(gs);
        }
        return;
    }
    if (gs.round == Round::Pre_Flop)
    {
        hand_position = assess_hole_cards(gs);
        player_position = assess_player_position(gs);
        int blind_level = gs.game_number / gs.num_games_per_blind_level;
        if (blind_level > 8) { blind_level = 8; }
        unsigned bb_value = BLINDS_STRUCTURE.at(blind_level).second;
        if (gs.chips_to_call <= bb_value &&
            gs.action_number < gs.num_players &&
            !gs.raise_active
            )
        {
            // If have a playable hand and no one else has bet,
            // bet 3 to 5 blinds (plus whatever needed to call).
            // Take into account stack size compared to BB and
            // Hand position (early is strongest hands, late weakest)
            if (bb_value * 6 <= 0.1 * gs.max_bet)
            {
                gs.player_action = Action::Raise;
                gs.player_bet = bb_value * 6;
            }
            else if (bb_value * 6 <= 0.25 * gs.max_bet)
            {
                gs.player_action = Action::Raise;
                if (hand_position == Position::Early)
                {
                    gs.player_bet = 5 * bb_value;
                }
                else if (hand_position == Position::Mid)
                {
                    gs.player_bet = 4 * bb_value;
                }
                else
                {
                    gs.player_bet = 3 * bb_value;
                }
            }
            else if (bb_value * 6 <= 0.5 * gs.max_bet)
            {
                gs.player_action = Action::Raise;
                if (hand_position == Position::Early &&
                    player_position >= Position::Early)
                {
                    gs.player_bet = 4 * bb_value;  // Strong hand good position
                }
                else if (hand_position == Position::Early ||
                    (hand_position == Position::Mid && player_position >= Position::Mid))
                {
                    gs.player_bet = 3 * bb_value;   // Strong or okay hand and 
                }                                   // okay or good position
                else
                {
                    gs.player_bet = 2 * bb_value;  // Weaker hand and position
                }
            }
            else
            {
                // Divide the total number of chips in the game by the number
                // of remaining players to compare to player's chip stack and
                // determine if chip stack is relatively small or large
                unsigned assess_chips = MAX_BUY_IN + MAX_BUY_IN * (gs.initial_num_players - gs.num_players) / gs.num_players;
                if (gs.max_bet <= 0.25 * assess_chips ||
                    gs.chips_to_call >= gs.max_bet
                    )
                {
                    if (hand_position == Position::Early)
                    {
                        gs.player_action = Action::All_In;  // Small stack,
                        gs.player_bet = gs.max_bet;         // Only play best hands and go all-in
                    }
                    else
                    {
                        check_or_fold(gs);
                    }
                }
                else if (gs.max_bet <= 0.80 * assess_chips)
                {
                    if (hand_position == Position::Early &&
                        player_position > Position::Early)
                    {
                        if (gs.max_bet > 3 * bb_value)
                        {
                            gs.player_bet = 3 * bb_value;  // Strong hand good position
                            gs.player_action = Action::Raise;
                        }
                        else
                        {
                            gs.player_action = Action::All_In;
                            gs.player_bet = gs.max_bet;
                        }
                    }
                    else if (hand_position == Position::Early ||
                        (hand_position == Position::Mid && player_position > Position::Mid))
                    {
                        if (gs.max_bet > 2 * bb_value)
                        {
                            gs.player_bet = 2 * bb_value;      // Strong or okay hand and 
                            gs.player_action = Action::Raise;  // okay or good position
                        }
                        else
                        {
                            gs.player_action = Action::All_In;
                            gs.player_bet = gs.max_bet;
                        }
                    }
                    else
                    {
                        if (gs.max_bet > gs.chips_to_call)
                        {
                            if (legal_act(Action::Call, gs))
                            {
                                gs.player_bet = gs.chips_to_call;  // Weaker hand and position
                                gs.player_action = Action::Call;
                            }
                            else
                            {
                                check_or_fold(gs);
                            }
                        }
                        else
                        {
                            check_or_fold(gs);
                        }
                    }
                }
                else  // Chip stack is close to or greater than even split of chips
                {
                    if (hand_position == Position::Early)
                    {
                        if (gs.max_bet > 3 * bb_value)
                        {
                            gs.player_bet = 3 * bb_value;  // Strong hand good position
                            gs.player_action = Action::Raise;
                        }
                        else
                        {
                            gs.player_action = Action::All_In;
                            gs.player_bet = gs.max_bet;
                        }
                    }
                    else if (hand_position == Position::Mid && player_position >= Position::Mid)
                    {
                        if (gs.max_bet > 2 * bb_value)
                        {
                            gs.player_bet = 2 * bb_value;      // Strong or okay hand and 
                            gs.player_action = Action::Raise;  // okay or good position
                        }
                        else
                        {
                            gs.player_action = Action::All_In;
                            gs.player_bet = gs.max_bet;
                        }
                    }
                    else if (hand_position == Position::Late && player_position == Position::Late)
                    {
                        if (gs.max_bet > 2 * bb_value)
                        {
                            gs.player_bet = 2 * bb_value;      // Strong or okay hand and 
                            gs.player_action = Action::Raise;  // okay or good position
                        }
                        else
                        {
                            gs.player_action = Action::All_In;
                            gs.player_bet = gs.max_bet;
                        }
                    }
                    else
                    {
                        if (gs.max_bet > gs.chips_to_call)
                        {
                            if (legal_act(Action::Call, gs))
                            {
                                gs.player_bet = gs.chips_to_call;  // Weaker hand and position
                                gs.player_action = Action::Call;
                            }
                            else
                            {
                                check_or_fold(gs);
                            }
                        }
                        else
                        {
                            check_or_fold(gs);
                        }
                    }
                }
            }
            return;
        }
        else
        {
        // If have a playable hand and someone else has bet/raised,
        // determination of whether to call should take into consideration
        // stack amount and number of active players
        // Odds of calling another player's bet/raise should be
        // 1 - (size of bet/raise / size of stack)
        // Num active players?  hand position?
            if (hand_position == Position::Early &&
                player_position > Position::Early &&
                gs.chips_to_call <= 0.1 * gs.max_bet)
            {
                if (legal_act(Action::Re_Raise, gs))
                {
                    gs.player_action = Action::Re_Raise;
                }
                else
                {
                    gs.player_action = Action::Raise;
                }
                gs.player_bet = gs.chips_to_call + gs.min_to_raise;
            }
            else
            {
                double odds_of_call = 1 - static_cast<double>(gs.chips_to_call) / gs.max_bet;
                double rand_number = m_uniform_dist(rng);
                if (legal_act(Action::Call, gs) &&
                    odds_of_call > rand_number)
                {
                    gs.player_bet = gs.chips_to_call;
                    gs.player_action = Action::Call;
                }
                else
                {
                    check_or_fold(gs);
                }
            }
        }
        return;
    }
    // Post-flop rounds use Monte Carlo simulations to determine next action
    double win_perc = run_mc_sim(gs.num_active_players, gs);
    gs.win_perc[gs.player_idx] = win_perc;
    unsigned bet = compute_bet(win_perc, gs, 1.3);
    double pot_odds = static_cast<double>(gs.chips_to_call) /
        (gs.pot_chip_count + gs.chips_to_call);
    double ratio = 1;
    if (pot_odds == 0)
    {
        ratio = 1;
    }
    else
    {
        ratio = win_perc / pot_odds;
    }
    double rand_num = m_uniform_dist(rng);
    if (gs.chips_to_call == 0)
    {
        // First player to bet this round
        gs.player_action = Action::Bet;
        if (bet < round_bet(0.5 * gs.pot_chip_count))
        {
            gs.player_action = Action::Check;
            gs.player_bet = 0;
        }
        else if (bet < gs.pot_chip_count)
        {
            if (round_bet(0.5 * gs.pot_chip_count) >= gs.min_to_raise &&
                round_bet(0.5 * gs.pot_chip_count) < gs.max_bet)
            {
                gs.player_bet = round_bet(0.5 * gs.pot_chip_count);
            }
            else if (round_bet(0.5 * gs.pot_chip_count) >= gs.max_bet &&
                legal_act(Action::All_In, gs))
            {
                gs.player_action = Action::All_In;
                gs.player_bet = gs.max_bet;
            }
            else
            {
                gs.player_action = Action::Check;
                gs.player_bet = 0;
            }
        }
        else if (bet < round_bet(1.5 * gs.pot_chip_count))
        {
            if (gs.pot_chip_count >= gs.min_to_raise &&
                gs.pot_chip_count < gs.max_bet)
            {
                gs.player_bet = gs.pot_chip_count;
            }
            else if (gs.pot_chip_count >= gs.max_bet &&
                legal_act(Action::All_In, gs))
            {
                gs.player_action = Action::All_In;
                gs.player_bet = gs.max_bet;
            }
            else
            {
                gs.player_action = Action::Check;
                gs.player_bet = 0;
            }
        }
        else if (bet < 2 * gs.pot_chip_count)
        {
            if (round_bet(1.5 * gs.pot_chip_count) >= gs.min_to_raise &&
                round_bet(1.5 * gs.pot_chip_count) < gs.max_bet)
            {
                gs.player_bet = round_bet(1.5 * gs.pot_chip_count);
            }
            else if (round_bet(1.5 * gs.pot_chip_count) >= gs.max_bet &&
                legal_act(Action::All_In, gs))
            {
                gs.player_action = Action::All_In;
                gs.player_bet = gs.max_bet;
            }
            else
            {
                gs.player_action = Action::Check;
                gs.player_bet = 0;
            }
        }
        else
        {
            if (2 * gs.pot_chip_count >= gs.min_to_raise &&
                2 * gs.pot_chip_count < gs.max_bet)
            {
                gs.player_bet = 2 * gs.pot_chip_count;
            }
            else if (2 * gs.pot_chip_count >= gs.max_bet &&
                legal_act(Action::All_In, gs))
            {
                gs.player_action = Action::All_In;
                gs.player_bet = gs.max_bet;
            }
            else
            {
                gs.player_action = Action::Check;
                gs.player_bet = 0;
            }
        }
        return;
    }

    if (gs.chips_to_call >= round_bet(0.5 * gs.max_bet))
    {
        // Chance to fold hand if other player bets aggressively and the 
        // probability of winning the hand isn't high.
        if (win_perc < m_uniform_dist(rng))
        {
            check_or_fold(gs);
            return;
        }
    }

    if (ratio < 1)
    {
        if (ratio * ratio > rand_num)
        {
            if (gs.max_bet > gs.chips_to_call)
            {
                if (legal_act(Action::Call, gs))
                {
                    gs.player_bet = gs.chips_to_call;  // Weaker hand and position
                    gs.player_action = Action::Call;
                }
                else
                {
                    check_or_fold(gs);
                }
            }
            else
            {
                check_or_fold(gs);
            }
        }
        else
        {
            check_or_fold(gs);
        }
    }
    else if (ratio >= 1)
    {
        if (gs.chips_to_call >= gs.max_bet && legal_act(Action::All_In, gs))
        {
            gs.player_action = Action::All_In;
            gs.player_bet = gs.max_bet;
        }
        else if (ratio < 1.2)
        {
            gs.player_action = Action::Call;
            gs.player_bet = gs.chips_to_call;
        }
        else if (ratio >= 1.2)
        {
            if (bet < round_bet(0.5 * gs.pot_chip_count))
            {
                gs.player_action = Action::Call;
                gs.player_bet = gs.chips_to_call;
            }
            else
            {
                if (bet >= 2 * gs.pot_chip_count &&
                    2 * gs.pot_chip_count >=
                    (gs.chips_to_call + gs.min_to_raise) &&
                    2 * gs.pot_chip_count < gs.max_bet)
                {
                    gs.player_bet = 2 * gs.pot_chip_count;
                }
                else if (bet >= round_bet(1.5 * gs.pot_chip_count) &&
                    round_bet(1.5 * gs.pot_chip_count) >=
                    (gs.chips_to_call + gs.min_to_raise) &&
                    round_bet(1.5 * gs.pot_chip_count) < gs.max_bet)
                {
                    gs.player_bet = round_bet(1.5 * gs.pot_chip_count);
                }
                else if (bet >= gs.pot_chip_count &&
                    gs.pot_chip_count >=
                    (gs.chips_to_call + gs.min_to_raise) &&
                    gs.pot_chip_count < gs.max_bet)
                {
                    gs.player_bet = gs.pot_chip_count;
                }

                else if (bet >= round_bet(0.5 * gs.pot_chip_count) &&
                    round_bet(0.5 * gs.pot_chip_count) >=
                    (gs.chips_to_call + gs.min_to_raise) &&
                    round_bet(0.5 * gs.pot_chip_count) < gs.max_bet)
                {
                    gs.player_bet = round_bet(0.5 * gs.pot_chip_count);
                }
                else
                {
                    if (gs.chips_to_call < gs.max_bet)
                    {
                        gs.player_action = Action::Call;
                        gs.player_bet = gs.chips_to_call;
                    }
                    else
                    {
                        check_or_fold(gs);
                    }
                    return;
                }
                if (legal_act(Action::Raise, gs))
                {
                    gs.player_action = Action::Raise;
                }
                else if (legal_act(Action::Re_Raise, gs))
                {
                    gs.player_action = Action::Re_Raise;
                }
            }
        }
    }

    // if (gs.chips_to_call <= bb_value)
    // {
    //     // Raise
    //     // Call 1 BB, bet 4 BB + 1 BB for every "limper"
    //     // BB will increase in value over time, compare with stack



    // }
    // else
    // {
    //     /*
    //     how many active players remaining in game?
    //     How many of these active players have called the current raise?
    //     How many players still need to act after me?
    //     How strong is my hand?
    //     how strong is my position?
    //     How much does it cost to call?
    //     How big is my stack?


    //     */

    // }

}



Position HeuristicAI::assess_hole_cards(GameState& gs)
{
    /**
     * Assess strength of player's hole cards.
     *
     * Return whether the cards are playable in the early position (strongest
     * hands), mid position, late position (weakest hands), or if the cards are
     * unplayable.
    */
    Card hole_card1 = gs.hole_cards[gs.player_idx].first;
    Card hole_card2 = gs.hole_cards[gs.player_idx].second;
    HoleCards cards_suit = HoleCards::Unsuited;
    if (hole_card1.suit == hole_card2.suit)
    {
        cards_suit = HoleCards::Suited;
    }
    auto starting_hand = std::make_tuple(
        hole_card1.rank,
        hole_card2.rank,
        cards_suit
    );
// Map key assumes higher-ranked card is the first card of the tuple
// Check to see which hole card has higher rank
    if (hole_card1.rank < hole_card2.rank)
    {
        starting_hand = std::make_tuple(
            hole_card2.rank,
            hole_card1.rank,
            cards_suit);
    }
    // Position hand_position;
    if (play_style == PlayStyle::TAG)  // Tight aggressive
    {
        if (gs.num_players >= 7)
        {
            // Full Ring Table
            if (FULL_RING_TAG_STARTING_HANDS.contains(starting_hand))
            {
                return FULL_RING_TAG_STARTING_HANDS.at(starting_hand);
            }
        }
        else
        {
            // Short Handed Table
            if (SHORT_HANDED_TAG_STARTING_HANDS.contains(starting_hand))
            {
                return SHORT_HANDED_TAG_STARTING_HANDS.at(starting_hand);
            }
        }
    }
    else if (play_style == PlayStyle::LAG)  // Loose aggressive
    {
        if (LAG_STARTING_HANDS.contains(starting_hand))
        {
            return LAG_STARTING_HANDS.at(starting_hand);
        }
    }
    return Position::Not_Playable;
}


Position HeuristicAI::assess_player_position(GameState& gs)
{
    /**
     * Assess strength of player's position at the table.
     *
     * The actual position of the player at the table doesn't matter.  The
     * important information is how many players are still active at the time
     * of the decision.  Being in a late position at the table simply means
     * you know with more certainty how many players are staying active and
     * whether they raised or just called.
     *
     * The Monte Carlo simulation and pot odds calculation takes into account
     * all of this information, and so the player position is mostly useful
     * to help determine what action to take pre-flop when no one has a
     * complete 5-card poker hand yet.
    */
    if (gs.num_players == 2)  // Heads-up play
    {
        if (gs.round == Round::Pre_Flop)
        {
            if (gs.blinds[gs.player_idx] == Blind::Big_Blind)
            {
                return Position::Late;  // BB acts last pre-flop
            }
            else
            {
                return Position::Early;
            }
        }
        else
        {
            if (gs.blinds[gs.player_idx] == Blind::Big_Blind)
            {
                return Position::Early;  // BB acts first post-flop
            }
            else
            {
                return Position::Late;
            }
        }
    }
    else
    {
        int current_pos_idx = to_underlying(gs.blinds[gs.player_idx]);
        if (gs.round == Round::Pre_Flop)
        {
            int table_position = current_pos_idx - to_underlying(Blind::UTG);
            table_position = table_position % gs.num_players;
            if (table_position < gs.num_players / 3)
            {
                return Position::Early;  // UTG acts first pre-flop
            }
            else if (table_position >= gs.num_players - (gs.num_players / 3))
            {
                return Position::Late;
            }
            else
            {
                return Position::Mid;
            }
        }
        else
        {
            int table_position = current_pos_idx - to_underlying(Blind::Small_Blind);
            table_position = table_position % gs.num_players;
            if (table_position < gs.num_players / 3)
            {
                return Position::Early;  // Small blind acts first post-flop
            }
            else if (table_position >= gs.num_players - (gs.num_players / 3))
            {
                return Position::Late;
            }
            else
            {
                return Position::Mid;
            }
        }
    }
}

double HeuristicAI::run_mc_sim(int sim_num_players, GameState& gs, int num_runs)
{
    /**
     * Run Monte Carlo simulation to determine player's probability of winning.
    */
    vector<AI_Type> player_ai_types;
    for (int i = 0; i < sim_num_players; i++)
    {
        player_ai_types.push_back(constants::AI_Type::CheckCall);
    }
    TexasHoldEm the(
        player_ai_types,
        sim_num_players,
        gs.random_seed,
        0,
        false
    );
    vector<Card> starting_hand = create_starting_hand(gs);
    the.begin_mc_game(starting_hand, num_runs);
    double win_perc = static_cast<double>(the.mc_run_wins) /
        static_cast<double>(the.mc_total_runs);
    return win_perc;
}

vector<Card> HeuristicAI::create_starting_hand(GameState& gs)
{
    /**
     * Create a vector of player's hole cards and common cards for MC sim.
    */
    vector<Card> starting_hand;
    if (gs.round >= Round::Pre_Flop)
    {
        starting_hand.push_back(gs.hole_cards[gs.player_idx].first);
        starting_hand.push_back(gs.hole_cards[gs.player_idx].second);
    }
    if (gs.round >= Round::Flop)
    {
        starting_hand.push_back(gs.flop_card1);
        starting_hand.push_back(gs.flop_card2);
        starting_hand.push_back(gs.flop_card3);
    }
    if (gs.round >= Round::Turn)
    {
        starting_hand.push_back(gs.turn_card);
    }
    if (gs.round >= Round::River)
    {
        starting_hand.push_back(gs.river_card);
    }
    return starting_hand;
}

unsigned HeuristicAI::compute_bet(double win_perc, GameState& gs, double ratio)
{
    /**
     * Calculate bet amount based on pot odds and estimated win probability.
     *
     * @param win_perc the probability of winning based on a Monte Carlo sim.
     * @param gs is the current game state.
     * @param ratio is the ratio of win percentage to pot odds.
     * @return player bet rounded to the nearest multiple of the small blind.
    */
    if (win_perc == 1) { return gs.max_bet; }
    double d_bet = (win_perc / ratio * gs.pot_chip_count) / (1 - win_perc / ratio);
    unsigned bet = SMALL_BLIND * static_cast<unsigned>(std::lroundl(d_bet / SMALL_BLIND));
    return bet;
}

unsigned HeuristicAI::round_bet(double bet)
{
    /**
     * Convert double to unsigned and round to nearest multiple of small blind.
    */
    return SMALL_BLIND * static_cast<unsigned>(std::lroundl(bet / SMALL_BLIND));
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
    gs.player_action = Action::No_Action;
    m_ai->player_act(gs);
    assert(gs.player_action != Action::No_Action &&
        "No action was taken by player!");
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

void Player::pass_script(vector<pair<Action, unsigned>> script)
{
    static_pointer_cast<ScriptedAI>(m_ai)->scripted_actions = script;
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

void Player::m_select_ai(std::mt19937& rng)
{
    /**
     * Select the appropriate act method based on the player's AI type.
    */
    switch (ai_type)
    {
    case AI_Type::Random:
        m_ai = make_shared<RandomAI>(rng);
        break;
    case AI_Type::Scripted:
        m_ai = make_shared<ScriptedAI>(rng);
        break;
    case AI_Type::CheckCall:
        m_ai = make_shared<CheckCallAI>(rng);
        break;
    case AI_Type::Heuristic_TAG:
        m_ai = make_shared<HeuristicAI>(rng, PlayStyle::TAG);
        break;
    case AI_Type::Heuristic_LAG:
        m_ai = make_shared<HeuristicAI>(rng, PlayStyle::LAG);
        break;
    default:
        cout << "Invalid player AI!" << endl;
        exit(-1);
    }
}

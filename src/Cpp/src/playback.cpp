/******************************************************************************
* Define functions to playback recorded game states.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <deque>
#include <iostream>
#include <string>
#include <vector>
// Project headers
#include "constants.hpp"
#include "cards.hpp"
#include "playback.hpp"
#include "storage.hpp"
#include "tabulate/tabulate.hpp"
// Using statements
using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::to_underlying;
using std::vector;
using namespace constants;
using namespace tabulate;

/* Overloaded to_string() Functions
******************************************************************************/
string to_string(const Card& c)
{
    std::ostringstream ss;
    ss << c;
    return ss.str();
}

string to_string(const Round& rnd)
{
    switch (rnd)
    {
    case Round::Pre_Flop:
        return "Pre-Flop";
        break;
    case Round::Flop:
        return "Flop";
        break;
    case Round::Turn:
        return "Turn";
        break;
    case Round::River:
        return "River";
        break;
    case Round::Showdown:
        return "Showdown";
        break;
    case Round::Game_Result:
        return "Game Result";
        break;
    default:
        cout << "Invalid round!" << endl;
        exit(-1);
    }
}

string to_string(const HandRank& hr)
{
    switch (hr)
    {
    case HandRank::High_Card:
        return "High Card";
        break;
    case HandRank::Pair:
        return "Pair";
        break;
    case HandRank::Two_Pair:
        return "Two Pair";
        break;
    case HandRank::Three_of_a_Kind:
        return "Three of a Kind";
        break;
    case HandRank::Straight:
        return "Straight";
        break;
    case HandRank::Flush:
        return "Flush";
        break;
    case HandRank::Full_House:
        return "Full House";
        break;
    case HandRank::Four_of_a_Kind:
        return "Four of a Kind";
        break;
    case HandRank::Straight_Flush:
        return "Straight Flush";
        break;
    case HandRank::Royal_Flush:
        return "Royal Flush";
        break;
    default:
        cout << "Invalid hand rank!" << endl;
        exit(-1);
    }
}

string to_string(const Blind& blind)
{
    switch (blind)
    {
    case Blind::No_Blind:
        return "";
        break;
    case Blind::Dealer_Button:
        return "Dealer Button";
        break;
    case Blind::Small_Blind:
        return "Small Blind";
        break;
    case Blind::Big_Blind:
        return "Big Blind";
        break;
    case Blind::Dealer_Button_and_Small_Blind:
        return "Dealer & Small Blind";
        break;
    case Blind::UTG:
        return "UTG";
        break;
    case Blind::UTG_1:
        return "UTG + 1";
        break;
    case Blind::UTG_2:
        return "UTG + 2";
        break;
    case Blind::UTG_3:
        return "UTG + 3";
        break;
    case Blind::UTG_4:
        return "UTG + 4";
        break;
    case Blind::UTG_5:
        return "UTG + 5";
        break;
    case Blind::UTG_6:
        return "UTG + 6";
        break;
    default:
        cout << "Invalid blind!" << endl;
        exit(-1);
    }
}

string to_string(const Action& action)
{
    switch (action)
    {
    case Action::No_Action:
        return "";
        break;
    case Action::All_In:
        return "All-In";
        break;
    case Action::Bet:
        return "Bet";
        break;
    case Action::Call:
        return "Call";
        break;
    case Action::Check:
        return "Check";
        break;
    case Action::Fold:
        return "Fold";
        break;
    case Action::Raise:
        return "Raise";
        break;
    case Action::Re_Raise:
        return "Re-Raise";
        break;
    default:
        cout << "Invalid action!" << endl;
        exit(-1);
    }
}

string to_string(const constants::AI_Type& ai)
{
    switch (ai)
    {
    case AI_Type::Random:
        return "Random";
        break;
    case AI_Type::Scripted:
        return "Scripted";
        break;
    default:
        cout << "Invalid AI type for string conversion!" << endl;
        exit(-1);
    }
}

string to_string(const std::pair<Card, Card> hole_cards)
{
    return to_string(hole_cards.first) + " " + to_string(hole_cards.second);
}

string to_string(const std::array<Card, constants::MAX_CARDS_IN_HAND> best_hand)
{
    return
        to_string(best_hand[0]) + " " +
        to_string(best_hand[1]) + " " +
        to_string(best_hand[2]) + " " +
        to_string(best_hand[3]) + " " +
        to_string(best_hand[4]);
}

/* Playback Definitions
******************************************************************************/
string calc_pot_per_round(const GameState& gs, const Round rd)
{
    /**
     * Calculate the total number of chips bet in a specified betting round.
    */
    unsigned total = 0;
    for (int i = 0; i < MAX_PLAYER_COUNT; i++)
    {
        total += gs.pot_player_bets[i][to_underlying(rd)];
    }
    return to_string(total);
}

string calc_pot_per_player(const GameState& gs, const int plyr_idx)
{
    /**
     * Calculate total number of chips bet by a player over all betting rounds.
    */
    unsigned total = 0;
    for (int i = 0; i < MAX_BETTING_ROUNDS; i++)
    {
        total += gs.pot_player_bets[plyr_idx][i];
    }
    return to_string(total);
}

void build_table(Table& player_table, const GameState& gs)
{
    /**
     * Build the structure of a table to represent the poker game.
    */
    player_table.format()
        .multi_byte_characters(true)  // Need to align card suit symbols
        .font_align(FontAlign::center);
    player_table.add_row({
        "Tournament Number",
        "Game Number",
        "Action Number",
        "Current Round",
        "Active Players",
        "Remaining Players",
        "Random Seed",
        "",
        ""
        });
    player_table.add_row({
        to_string(gs.tournament_number),
        to_string(gs.game_number),
        to_string(gs.action_number),
        to_string(gs.round),
        to_string(gs.num_active_players),
        to_string(gs.num_players),
        to_string(gs.random_seed),
        "",
        ""
        });
    player_table.add_row({ "", "", "", "", "", "", "", "", "" });
    player_table.add_row({
        "Flop",
        "Turn",
        "River",
        "Pot Pre-Flop",
        "Pot Flop",
        "Pot Turn",
        "Pot River",
        "Pot Total",
        ""
        });
    player_table.add_row({
        to_string(gs.flop_card1) + " " +
        to_string(gs.flop_card2) + " " +
        to_string(gs.flop_card3),
        to_string(gs.turn_card),
        to_string(gs.river_card),
        calc_pot_per_round(gs, Round::Pre_Flop),
        calc_pot_per_round(gs, Round::Flop),
        calc_pot_per_round(gs, Round::Turn),
        calc_pot_per_round(gs, Round::River),
        to_string(gs.pot_chip_count),
        ""
        });
    player_table.add_row({ "", "", "", "", "", "", "", "", "" });
    player_table.add_row({
        "Player",
        "Blind",
        "Last Action",
        "Chips Bet",
        "Chip Count",
        "AI",
        "Hole Cards",
        "Best Hand",
        "Hand Rank"
        });
    for (int i = 0; i < gs.initial_num_players; i++)
    {
        if (gs.remaining_players[i])  // If player not eliminated from tourney
        {
            if (i == gs.player_idx &&
                gs.round != Round::Showdown &&
                gs.round != Round::Game_Result)
            {
                // Game state records current player's chips bet and chip count
                // prior to the action, and so must add or subtract the bet
                player_table.add_row({
                to_string(i),
                to_string(gs.blinds[i]),
                to_string(gs.player_action),
                to_string(stoi(calc_pot_per_player(gs, i)) + gs.player_bet),
                to_string(gs.player_chip_counts[i] - gs.player_bet),
                to_string(gs.ai_types[i]),
                to_string(gs.hole_cards[i]),
                to_string(gs.best_hands[i]),
                to_string(gs.hand_ranks[i])
                    });
            }
            else
            {
                // Non-current players can use game state values directly
                player_table.add_row({
                    to_string(i),
                    to_string(gs.blinds[i]),
                    to_string(gs.last_actions[i]),
                    calc_pot_per_player(gs, i),
                    to_string(gs.player_chip_counts[i]),
                    to_string(gs.ai_types[i]),
                    to_string(gs.hole_cards[i]),
                    to_string(gs.best_hands[i]),
                    to_string(gs.hand_ranks[i])
                    });
            }
        }
        else  // Player eliminated from tournament
        {
            player_table.add_row({
                "Eliminated",
                "",
                "",
                "",
                "",
                "",
                "",
                "",
                ""
                });
        }
    }
}

void format_table(Table& player_table, const GameState& gs)
{
    /**
     * Format the table used to represent the poker game.
    */
    // Color header rows of table
    player_table[0].format()
        .font_color(Color::cyan)
        .font_style({ FontStyle::bold });
    player_table[3].format()
        .font_color(Color::cyan)
        .font_style({ FontStyle::bold });
    player_table[6].format()
        .font_color(Color::cyan)
        .font_style({ FontStyle::bold });
    // Highlight game number at the start of a new game
    if (gs.action_number == 0)
    {
        player_table[1][1].format()
            .font_color(Color::green);
    }
    // Highlight current round and common cards dealt during round
    player_table[1][3].format()
        .font_color(Color::green);
    switch (gs.round)
    {
    case Round::Flop:
        player_table[4][0].format()
            .font_color(Color::green);
        break;
    case Round::Turn:
        player_table[4][1].format()
            .font_color(Color::green);
        break;
    case Round::River:
        player_table[4][2].format()
            .font_color(Color::green);
        break;
    default:
        break;
    }
    // Grey out folded players
    for (int i = 0; i < gs.initial_num_players; i++)
    {
        if (!gs.active_player_list[i])
        {
            player_table[7 + i].format()
                .font_color(Color::grey)
                .font_style({ FontStyle::italic });
        }
    }
    // Highlight players
    if (gs.round != Round::Showdown &&
        gs.round != Round::Game_Result)
    {
        // Highlight the current player in yellow
        player_table[7 + gs.player_idx].format()
            .font_color(Color::yellow)
            .font_style({ FontStyle::bold });
    }
    else if (gs.round == Round::Showdown)
    {
        // Highlight showdown players' hands in red
        for (const auto& plyr : gs.showdown_players)
        {
            player_table[7 + plyr.player_idx][7].format()
                .font_color(Color::red)
                .font_style({ FontStyle::bold });
            player_table[7 + plyr.player_idx][8].format()
                .font_color(Color::red)
                .font_style({ FontStyle::bold });
        }
    }
    else  // Game results
    {
        // Highlight game winners in magenta
        for (const auto& plyr : gs.showdown_players)
        {
            if (plyr.chips_won > 0)
            {
                player_table[7 + plyr.player_idx].format()
                    .font_color(Color::magenta)
                    .font_style({ FontStyle::bold });
            }
        }
    }
}

void print_table(const GameState& gs)
{
    /**
     * Build, format, and print the table representing the poker game.
    */
    Table player_table;
    build_table(player_table, gs);
    format_table(player_table, gs);
    cout << player_table << endl;
}

void print_console_output(const GameState& gs)
{
    /**
     * Print console messages describing the poker game.
    */
    static std::deque<string> console_output(MAX_CONSOLE_LINES, "");
    static vector<int> remaining_players(gs.initial_num_players, true);
    static Round rnd = Round::Pre_Flop;
    string output;
    if (gs.game_number == 0 && gs.action_number == 0)
    {
        // Print game number and dealer message for first game of tournament
        output = "- Game #" + to_string(gs.game_number) +
            ": Dealer deals cards to players.";
        console_output.push_back(output);
    }
    if (gs.round != rnd)
    {
        rnd = gs.round;
        if (gs.round != Round::Showdown &&
            gs.round != Round::Game_Result)
        {
            // Comment on the changing of the betting round
            if (gs.round == Round::Pre_Flop)
            {
                output = "- Game #" + to_string(gs.game_number) +
                    ": Dealer deals cards to players.";
                console_output.push_back(output);
            }
            else
            {
                output = "- Dealer deals the " + to_string(gs.round) + ".";
                console_output.push_back(output);
            }
        }
        else if (gs.round == Round::Showdown)
        {
            // Describe which players enter the showdown round
            output = "- Players ";
            for (const auto& plyr : gs.showdown_players)
            {
                output += to_string(plyr.player_idx) + ", ";
            }
            output += "enter the Showdown.";
            console_output.push_back(output);
        }
        else  // Game_Results
        {
            for (const auto& plyr : gs.showdown_players)
            {
                if (plyr.chips_won > 0)  // Print winning player(s)
                {
                    output = "- Player " + to_string(plyr.player_idx) +
                        " won $" + to_string(plyr.chips_won) + " with a " +
                        to_string(plyr.hand_rank) + "!";
                    console_output.push_back(output);
                }
            }
            if (gs.remaining_players != remaining_players)
            {
                output = "- Players ";  // Print eliminated players
                for (int i = 0; i < gs.initial_num_players; i++)
                {
                    if (gs.remaining_players[i] != remaining_players[i])
                    {
                        output += to_string(i) + ", ";
                    }
                }
                output += "are eliminated from the tournament!";
                console_output.push_back(output);
                remaining_players = gs.remaining_players;
                if (gs.num_players == 1)
                {
                    int winner = gs.showdown_players[0].player_idx;
                    output = "- Player " + to_string(winner) +
                        " won the tournament!";
                    console_output.push_back(output);
                }
            }
        }
    }
    // Describe player action
    if (gs.round != Round::Showdown &&
        gs.round != Round::Game_Result)
    {
        output = "- Player " + to_string(gs.player_idx);
        if (gs.player_action == Action::Check
            || gs.player_action == Action::Fold)
        {
            output += " " + to_string(gs.player_action) + "s.";
        }
        else if (gs.player_action == Action::Call)
        {
            output += " calls $" + to_string(gs.chips_to_call) + ".";
        }
        else if (gs.player_action == Action::All_In)
        {
            output += " goes " + to_string(gs.player_action) + " with $" +
                to_string(gs.player_bet) + "!";
        }
        else if (gs.player_action == Action::Bet)
        {
            output += " " + to_string(gs.player_action) + "s $" +
                to_string(gs.player_bet) + "!";
        }
        else
        {
            output += " calls $" + to_string(gs.chips_to_call) + " and " +
                to_string(gs.player_action) + "s by $" +
                to_string(gs.player_bet - gs.chips_to_call) + "!";
        }
        console_output.push_back(output);
    }
    while (console_output.size() > MAX_CONSOLE_LINES)
    {
        console_output.pop_front();  // Delete old console messages
    }
    for (const auto& line : console_output)
    {
        cout << line << endl;
    }
}

void print_state(const GameState& gs)
{
    /**
     * Clear the terminal and then print the current poker game state.
    */
    system("clear");
    print_table(gs);
    print_console_output(gs);
}

void playback_tournament(TournamentHistory& th)
{
    /**
     * Play back the entire poker tournament to terminal one state at a time.
    */
    for (const auto& game : th.games)
    {
        for (const auto& state : game.states)
        {
            print_state(state);
            cout << "\nPress enter to continue: ";
            std::cin.get();
        }
    }
}
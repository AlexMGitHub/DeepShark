/******************************************************************************
* Create C shared library to interface between Python and Cpp code.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <algorithm>                // For std::copy()
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>                  // For std::to_underlying()
#include <vector>
// Project headers
#include "deepshark.hpp"
#include "player.hpp"
#include "serialize.hpp"
#include "texasholdem.hpp"
// Using statements
using std::cout;
using std::endl;
using std::string;
using std::to_underlying;
using std::vector;
using namespace constants;

/* C Interface Functions
******************************************************************************/
int add_ints(int x, int y)
{
    return x + y;
}

std::mutex thread_count_mutex;
std::condition_variable thread_cv;
unsigned thread_count = 0;
unsigned max_threads;

void run_tournament_thread(int thread_index, tourn_data data)
{
    TexasHoldEm the(
        data.player_ai_types,
        data.num_players,
        data.seed,
        thread_index,  // Tournament #
        data.debug,
        data.num_games_per_blind_level,
        data.tourn_directory
    );
    the.begin_tournament();
    std::unique_lock<std::mutex> lck(thread_count_mutex);
    thread_count--;
    //cout << "Tournament " << thread_index << " completed!" << endl;
    lck.unlock();
    thread_cv.notify_one();
}

int run_tournaments(
    int num_players,
    int player_ai_types[],
    int num_games_per_blind_level,
    char tourn_directory[],
    int seeds[],
    unsigned num_processors,
    int num_tournaments,
    bool debug
)
{
    // Multithreading options
    const unsigned processor_count = std::thread::hardware_concurrency();
    if (num_processors > processor_count / 2 - 2)
    {
        return -1;  // Too many processors being requested
    }
    max_threads = num_processors;
    vector<std::thread> threads(num_tournaments);
    // Begin tournaments
    std::unique_lock<std::mutex> lck(thread_count_mutex);
    for (int i = 0; i < num_tournaments; i++)
    {
        // THE constructor arguments
        try {
            tourn_data data(
                num_players,
                player_ai_types,
                num_games_per_blind_level,
                seeds[i],
                tourn_directory,
                debug
            );
            // Create thread
            threads[i] = std::thread(run_tournament_thread, i, data);
        }
        catch (const std::invalid_argument& e) {
            return -1;  // Invalid AI type
        }
        thread_count++;
        if (thread_count == max_threads)
        {
            thread_cv.wait(lck, [] { return thread_count < max_threads; });
        }
    }
    lck.unlock();
    for (auto& th : threads)
    {
        th.join();
    }

    return 0;
}

void get_tournament_summary(struct tournament_summary* summary, char filename[])
{
    string filename_str = filename;
    TournamentHistory th = read_tournamenthistory(filename_str, true);
    summary->tournament_number = th.tournament_number;
    summary->random_seed = th.random_seed;
    summary->initial_player_count = th.initial_player_count;

    for (int i = 0; i < th.initial_player_count; i++)
    {
        *(summary->player_ai_types + i) = to_underlying(th.player_ai_types[i]);
        *(summary->finishing_order + i) = th.finishing_order[i];
        *(summary->game_eliminated + i) = th.game_eliminated[i];
    }
    summary->num_games = th.num_games;
}

void write_nn_vector_data(char read_filename[], char write_filename[])
{
    string read_filename_str = read_filename;
    string write_filename_str = write_filename;
    write_nn_vector_data(read_filename_str, write_filename_str);
}

// int main()
// {
//     int player_ai_types[] = { 3,4,3,4,3,4,3,4,3,4 };
//     int seeds[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19 };
//     char directory[] = "deepshark";
//     run_tournaments(
//         10,
//         player_ai_types,
//         20,
//         directory,
//         seeds,
//         4,
//         20,
//         false
//     );
//     return 0;
// }
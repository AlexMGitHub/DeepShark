/******************************************************************************
* Test functionality of multithreading tournaments in parallel.
******************************************************************************/

/* Headers
******************************************************************************/
// C++ standard library
#include <condition_variable>
#include <chrono>       // For high_resolution_clock
#include <filesystem>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
// Project headers
#include "cards.hpp"
#include "constants.hpp"
#include "playback.hpp"
#include "player.hpp"
#include "serialize.hpp"
#include "texasholdem.hpp"
// Using statements
using std::cout;
using std::endl;
using std::vector;
using namespace constants;

/* Playback Functional Tests
******************************************************************************/
std::mutex thread_count_mutex;
std::condition_variable thread_cv;
unsigned thread_count = 0;
unsigned max_threads;

struct the_data
{
    the_data(
        int num_players,
        bool debug,
        int num_games_per_blind_level,
        std::string tourn_directory) :
        num_players(num_players),
        debug(debug),
        num_games_per_blind_level(num_games_per_blind_level),
        tourn_directory(tourn_directory)
    {
        for (int i = 0; i < num_players; i += 2)
        {
            player_ai_types.push_back(AI_Type::Heuristic_TAG);
            player_ai_types.push_back(AI_Type::Heuristic_LAG);
            // player_ai_types.push_back(AI_Type::Random);
            // player_ai_types.push_back(AI_Type::Random);
        }
    }
    int num_players;
    bool debug;
    int num_games_per_blind_level;
    std::string tourn_directory;
    vector<AI_Type> player_ai_types;
};

void run_tournament(int thread_index, the_data data)
{
    TexasHoldEm the(
        data.player_ai_types,
        data.num_players,
        thread_index,  // Seed
        thread_index,  // Tournament #
        data.debug,
        data.num_games_per_blind_level,
        data.tourn_directory
    );
    the.begin_tournament();
    std::unique_lock<std::mutex> lck(thread_count_mutex);
    thread_count--;
    cout << "Tournament " << thread_index << " completed!" << endl;
    lck.unlock();
    thread_cv.notify_one();
}

int main()
{
    // Multithreading options
    int num_tourns = 20;
    const unsigned processor_count = std::thread::hardware_concurrency();
    max_threads = processor_count / 2 - 4;
    cout << processor_count << endl;
    cout << max_threads << endl;
    vector<std::thread> threads(num_tourns);
    // THE constructor arguments
    int num_players = 10;
    vector<AI_Type> player_ai_types;
    bool debug = false;
    int num_games_per_blind_level = 20;
    std::string tourn_directory = "multithread_testing";
    the_data data(
        num_players,
        debug,
        num_games_per_blind_level,
        tourn_directory
    );
    // Instantiate THE object and begin tournament
    std::unique_lock<std::mutex> lck(thread_count_mutex);
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_tourns; i++)
    {
        threads[i] = std::thread(run_tournament, i, data);
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
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    std::cout << "\n\t- Execution time of " << max_threads << " cores:\t\t" <<
        ms_double.count() << " ms\n";

    // Deserialize and playback recorded game
    //TournamentHistory th = read_tournamenthistory(the.filename);
    // Uncomment to playback existing recorded game
    // cout << std::filesystem::current_path() << endl;
    // TournamentHistory th = read_tournamenthistory("recorded_games/2023-12-24/tourn_0__2023-12-24__22-07-10.bin");
    //playback_tournament(th);
    return 0;
}
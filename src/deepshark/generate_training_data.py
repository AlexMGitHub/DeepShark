"""Script to run C++ game environment and generate training data."""
# Imports
###############################################################################
# Standard system imports
import ctypes
# Related third party imports
# Local application/library specific imports

# Initialize and test C shared library
###############################################################################
# Location of C shared library
c_lib = ctypes.CDLL("src/Cpp/lib/lib_deepshark.so")
# %% Set data types of test C function's arguments and return value
add_ints = c_lib.add_ints
add_ints.argtypes = [ctypes.c_int, ctypes.c_int]
add_ints.restype = ctypes.c_int  # C function returns integer
# Test C function
result = add_ints(4, 5)
print(result)

# Initialize C function to run Texas Hold 'Em tournaments
###############################################################################
run_tournaments = c_lib.run_tournaments
run_tournaments.argtypes = [
    ctypes.c_int,
    ctypes.POINTER(ctypes.c_int),
    ctypes.c_int,
    ctypes.POINTER(ctypes.c_char),
    ctypes.POINTER(ctypes.c_int),
    ctypes.c_uint,
    ctypes.c_int,
    ctypes.c_bool,
]
run_tournaments.restype = ctypes.c_int

# Configure tournament parameters
###############################################################################
# Number of players in tournament
players = 10
num_players = ctypes.c_int(players)
# Player AI types
player_ai_types = (ctypes.c_int * players)()
for idx, x in enumerate(player_ai_types):
    if idx % 2 == 0:
        player_ai_types[idx] = 0  # Random
    else:
        player_ai_types[idx] = 6  # NN
    # if idx in [0, 3, 6]:
    #     player_ai_types[idx] = 3  # TAG
    # elif idx in [1, 4, 7]:
    #     player_ai_types[idx] = 4  # LAG
    # elif idx in [2, 5, 8]:
    #     player_ai_types[idx] = 5  # MTAG
    # else:
    #     player_ai_types[idx] = 3  # TAG
    # if idx == players - 1:
    #     player_ai_types[idx] = 5  # MTAG
    # else:
    #     player_ai_types[idx] = 0  # Random
    # player_ai_types[idx] = 5  # MTAG

# Number of games before increasing blind amount
num_games_per_blind_level = ctypes.c_int(20)
# Name of directory to store game results in
directory = "nn_random"
b_directory = directory.encode("utf-8")  # create byte objects from the strings
tourn_directory = ctypes.c_char_p(b_directory)
# Number of tournaments to run
num_tourns = 100
num_tournaments = ctypes.c_int(num_tourns)
# Generate unique seed for each tournament
seeds = (ctypes.c_int * num_tourns)()  # Weird syntax that creates array
for idx, x in enumerate(seeds):
    seeds[idx] = idx
# Number of physical processors to run simulations on
num_processors = ctypes.c_uint(4)
# Debug should be false - can omit argument as default argument is false
debug = ctypes.c_bool(0)

# Run tournaments and generate data
###############################################################################
run_tournaments(
    num_players,
    player_ai_types,
    num_games_per_blind_level,
    tourn_directory,
    seeds,
    num_processors,
    num_tournaments,
    debug
)

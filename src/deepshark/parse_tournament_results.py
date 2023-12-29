"""."""
# Imports
###############################################################################
# Standard system imports
import ctypes
from os import listdir
from os.path import isfile, join
# Related third party imports
import numpy as np
# Local application/library specific imports


# Classes used to contain data
###############################################################################
class TournamentSummary(ctypes.Structure):
    """Creates a tournament_summary struct."""

    _fields_ = [('tournament_number', ctypes.c_int),
                ('random_seed', ctypes.c_int),
                ('initial_player_count', ctypes.c_int),
                ('finishing_order', ctypes.POINTER(ctypes.c_int)),
                ('game_eliminated', ctypes.POINTER(ctypes.c_int)),
                ('num_games', ctypes.c_size_t)]


class PythonTournamentSummary:
    """Creates a tournament summary for parsing in Python."""

    def __init__(self, th, finishing_order, game_eliminated):
        self.tournament_number = th.tournament_number
        self.random_seed = th.random_seed
        self.initial_player_count = th.initial_player_count
        self.finishing_order = finishing_order
        self.game_eliminated = game_eliminated
        self.num_games = th.num_games


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

# Initalize C function to get tournament results
###############################################################################
tourn_summary = c_lib.get_tournament_summary
tourn_summary.argtypes = [ctypes.POINTER(
    TournamentSummary), ctypes.POINTER(ctypes.c_char)]
tourn_summary.restype = None

# Specify folder containing recorded games, parse and store tournament results
###############################################################################
tourn_directory = "recorded_games/2023-12-28/python/"  # Must update this field
tourn_files = [join(tourn_directory, f) for f in listdir(
    tourn_directory) if isfile(join(tourn_directory, f))]

summary_list = list()
for tourn in tourn_files:
    # Encode filename as a pointer to array of chars
    b_fn = tourn.encode("utf-8")  # create byte objects from the strings
    filename = ctypes.c_char_p(b_fn)
    # Instantiate struct to contain summary results
    th = TournamentSummary()
    th_ptr = ctypes.pointer(th)
    # Create NumPy arrays to store finishing order and game eliminated arrays
    finishing_order_arr = np.zeros(10, dtype=np.intc)
    finishing_order_ptr = finishing_order_arr.ctypes.data_as(
        ctypes.POINTER(ctypes.c_int))
    th.finishing_order = finishing_order_ptr

    game_eliminated_arr = np.zeros(10, dtype=np.intc)
    game_eliminated_ptr = game_eliminated_arr.ctypes.data_as(
        ctypes.POINTER(ctypes.c_int))
    th.game_eliminated = game_eliminated_ptr
    # Pass pointer to struct and filename to C function
    tourn_summary(th_ptr, filename)
    # Transfer results to Python class and append to list
    pth = PythonTournamentSummary(th, finishing_order_arr, game_eliminated_arr)
    summary_list.append(pth)

# Post-process tournament results
###############################################################################
lag_count = 0
tag_count = 0

for summ in summary_list:
    if summ.finishing_order[summ.initial_player_count-1] % 2 == 0:
        tag_count += 1
    else:
        lag_count += 1

print(f"LAG count = {lag_count}")
print(f"TAG count = {tag_count}")

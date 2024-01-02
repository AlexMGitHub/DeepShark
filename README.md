# DeepShark

<p align="center">
<img src="docs/img/DeepShark.jpg" title="DeepShark" alt="DeepShark" width="600"/>
</p>

 A project to apply deep reinforcement learning to the game of Texas hold 'em poker.

The name of the project, DeepShark, is a compounding of the phrases "deep learning" and "card shark."  Deep learning refers to machine learning methods that utilize artificial neural network architectures, while a "shark" is slang for a highly skilled poker player.  I would like to train an ANN to play poker well, hence DeepShark.  The DeepShark image above was generated using Bing's AI image creator.

### Development Environment
DeepShark was developed using Windows Subsystem for Linux (WSL) with an Ubuntu image.  The reported g++ compiler version used for the C++ code is:

`g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0`

There is some non-portable code (e.g. system("clear") to clear the terminal) that will not run in a non-Linux environment.

I wrote the Python code using Python 3.11.5.  A *requirements.txt* file is included for convenience.

## Description of the Poker Environment

The Texas Hold 'Em poker environment runs a poker tournament with up to 10 players at the table.  The tournament has a blinds schedule that increases the size of the blinds after a user-specified number of games.  This replicates the effect of time-based blinds schedules that are used in real poker tournaments to prevent tournaments from dragging on too long.

I created a struct that stores the current game state, and this game state is passed along from function to function and modified as the players progress through the various betting rounds.  The game state structs for a particular game are stored in another game history struct, and when the game is over the game history struct is stored in a tournament history struct.  In this way the environment stores every state of every game played during the tournament.  I wrote a serialization routine to write the contents of the tournament history to a binary file and save it to disk.

Texas Hold 'Em is a relatively simple game, but there are three areas that were tricky to implement:

1.  Determining each player's best five-card poker hand from the available cards

2.  Determining when the betting round has ended, particularly if betting can continue after a player has gone all-in

3.  Determining the winner of the game and accounting for ties and side pots

I wrote both unit tests and functional tests that helped me catch many bugs in the above functionality.  I also wrote a user interface using @p-ranav's [tabulate](https://github.com/p-ranav/tabulate) table maker library.  The user interface allowed me to watch each game play out action-by-action, which was useful for debugging the game environment code.  It's also useful for playing back deserialized games that have been recorded to disk.

### Implementing the Game Environment in C++
I wrote the Texas Hold 'Em game loop in C++.  My previous deep reinforcement project [Checkers-MCTS](https://github.com/AlexMGitHub/Checkers-MCTS) was written entirely in Python, and the slow performance of the checkers game environment became an issue when generating training data.  I experimented with using the `ctypes` Python library to run C functions from Python scripts for my [PyCEM](https://github.com/AlexMGitHub/PyCEM) FDTD simulator, which provided the speed of the C language with the convenience of writing the user interface in Python.

The PyCEM experiment with `ctypes` was a success, but I ran into issues getting C to do what I wanted it to do.  I appreciated the performance and control provided by the C language, but I was frustrated by its quirks and lack of high-level language features.  "Modern" C++ appeared to be the best of both worlds: the performance of C with a standard library that provides many high-level language features.  The `ctypes` Python library is not compatible with C++, but because C++ is essentially a superset of C it is possible to work around this limitation.

I compiled a shared library of the poker game loop that was written using C++ code, but I created C wrapper functions to interface with `ctypes`.  This is done by using the `extern "C"` specification when declaring the wrapper functions in the C++ header file.  This specification prevents the C++ compiler from engaging in "name mangling" which is used in C++ to support function overloading.  The wrapper functions must only use "Plain Old Data" (POD) - that is, basic C data types - as the return type and argument types in the declaration.  However, the bodies of these wrapper functions can use C++ code.

The performance of the game environment in C++ is impressive.  I wrote a player AI that runs a 1000-game Monte Carlo simulation every time it makes a decision post-flop.  Even so, a 10-player tournament with hundreds of games only takes seconds to run.  I also used multithreading in C++ to run multiple tournaments in parallel.  With four parallel threads it only takes about three minutes to simulate 100 poker tournaments.

### Initial "Dumb" AI Players
I needed to be able to test the poker environment to ensure that the game rules were properly implemented.  I did this by writing "dumb" (i.e. hard-coded) AI player classes to play out poker games.  The first "AI" that I created was a purely random player who would randomly choose from the list of legal actions.  If the chosen action was a bet or raise, the AI would randomly choose a value between the minimum bet and the maximum value of its chip stack.

This very simple AI allowed me to test out my user interface, serialization/deserialization code, and catch some bugs in the poker environment.  But once the code seemed functional I added another "scripted" AI class that accepts a pre-planned list of actions to take.  I used these scripted AI players to perform functional testing of the code by devising a few scripted scenarios that involved complicated end-game logic such as multiple side pots and ties.  This testing allowed me to catch a few subtle bugs in the poker code which led to improper payouts when there were multiple winners.

I next wanted to write an AI class that would play somewhat intelligently but still maintain a stochastic nature.  This AI could play out longer and more realistic poker tournaments and possibly even be used to generate initial training data for a neural network.  I wanted this AI to have a rough idea of the strength of its current hand, but not get too bogged down into calculating complicated drawing odds and outs for a particular hand.

I called this class of dumb AIs the "heuristic" AI, as I used some basic poker rules of thumb to guide its decision making.  I combined these rules of thumb with a Monte Carlo simulation to assess the strength of the player's current position.  I found that math for calculating poker odds quickly gets complicated [[1]](#references), and a Monte Carlo simulation will get me a good enough answer to make reasonable decisions.  I only run Monte Carlo simulations during post-flop rounds, as pre-flop I judge the strength of the player's hand by the player's position at the table and whether the player's hole cards are contained in a list of strong starting hands.  I obtained the lists for a tight aggressive player (TAG) from [[2]](#reference), and added additional strong hands to the list for a loose aggressive player (LAG).  The TAG and LAG AI have identical play styles; the only difference is that the LAG is willing to play a larger number of starting hands than the TAG.

The Monte Carlo simulation adds a stochastic element to the AI's decision making as I used the resulting expected win probability in combination with pot odds to determine the player's action.  The pot odds are calculated as the ratio of the chips required to call the bet divided by the total amount of chips in the pot assuming that you call.  If the expected win percentage is larger than the pot odds, then it is worth calling as in the long-run you are expected to win more money than you would lose.  The pot odds are the ratio of the amount of money the player is risking by calling the bet to the amount of money the player could potentially win.  Intuitively, if the pot odds are small then we are risking a relatively small amount of money to win a lot of money.  The smaller the pot odds (as a percentage) and the larger the probability of winning, the stronger the player's position.  

For situations where the pot odds and win probability are nearly equal, I used a uniform distribution to add further stochasticity to the player's decision making.  For example, if the ratio of the probability of winning to the pot odds is greater than a uniform random number between 0 and 1, then the player calls.  Otherwise the player checks or folds.  This prevents the AI from always folding to big bluffs, but still gives a probability of folding proportionate to how weak the hand is.  The expected result is a player AI that will typically make reasonable decisions based on the strength of its current hand, but can unpredictably take actions such as bluffing on a weak hand, calling another player's bluff, etc.

### Comparing Play Styles

Poker playing styles tend to fall along a two-axis spectrum: players can be "tight" or "loose", and "aggressive" or "passive."  A loose play style describes a player who plays a wide range of starting hands.  A tight player plays a narrower range of strong starting hands.  Passive players tend to often check or call and rarely bet or raise large amounts, whereas aggressive players are the opposite.  

I initially created two play styles for the heuristic AI: tight-aggressive (TAG) and loose-aggressive (LAG).  Passive play is considered to be a poor play style used by beginners - if you have a good hand it is almost always better to bet aggressively and force the other players to make hard choices.

I compared the TAG and LAG players in more detail in the Jupyter notebook `heuristic_tournament_results.ipynb`.  In summary, the LAG players were much more likley to be eliminated early on in the tournament compared to the TAG players, but surprisingly a LAG player won the tournament more often than a TAG player.

Out of 100 10-player tournaments with 5 LAG players and 5 TAG players in each, the LAGs won 52 of the tournaments and the TAGs won 48 tournaments:

<p align="center">
<img src="docs/img/lag_vs_tag.png" title="LAG vs. TAG" alt="LAG vs. TAG" width="552"/>
</p>

The horizontal axis of the above plot is labeled "Result."  A result of 10 means 10th place - being eliminated first.  A result of 1 means 1st place - winning the tournament.

This indicated to me that the TAG player might be too tight.  As the number of players at the table dwindles, a wider array of starting hands become viable.  In addition, the blinds schedule increases the size of the blinds as the tournament continues, meaning that the price of folding viable starting hands becomes more prohibitive.

To test my theory, I created a third play style called "Modified Tight-Aggressive" (MTAG) that played the same starting hands as the TAG player up until there were 3 or fewer players left in the tournament.  At that point the MTAG uses the same list of starting hands as the LAG.

<p align="center">
<img src="docs/img/play_style_comparison.png" title="Play Style Comparison Plot" alt="Play Style Comparison Plot" width="552"/>
</p>

The pattern seems to hold with all three play styles in one tournament.  The LAG players still tend to be eliminated early in the tournament, and yet the LAGs won exactly as many tournaments as the MTAGs.  The TAGs and MTAGs had very similar early-tournament performance, but the MTAG took home more tournament wins than the TAG.

Overall, the MTAG appears to be a good compromise between the too-tight TAGs and the too-loose LAGs.

## Deep Reinforcement Learning

### Considerations for Neural Network Architecture
One point of consideration is whether the neural network architecture ought to be some sort of recurrent neural network that remembers previous states.

Without memory, the neural network will essentially just be learning how strong a particular poker hand is.  This functionality could be easily reproduced by a look-up table.  An ANN with memory of previous states could identify player behavior patterns.  As mentioned previously, poker playing styles fall along a two-axis spectrum of "tight" or "loose" and "aggressive" or "passive."  The resulting four quadrants describe play styles that could be identified and exploited by an opponent.  For instance, a loose and aggressive play style describes a player who plays a wide range of starting hands and bets aggressively - which means that they are often bluffing.  An observant opponent might notice this tendency to play weak hands and bluff, and be more likely to call their raises.  Similarly, a tight and aggressive player will only play a narrow range of hands and will bet aggressively when he has good cards.  An observant player would be less likely to call their bets as they probably aren't bluffing, and may be able to steal blinds if the player is too tight.

### Mapping the Game State to a Neural Network Input

*   Legal Actions
    *   Vector of length 7 for 7 actions, 1 if action is legal 0 otherwise
*   Hole Cards
    *   Suit is one hot encoded (length 4)
    *   Rank is one hot encoded (length 13)
    *   All 0s for no suit/rank
*   Flop Cards
*   Turn Card
*   River Card
*   Chips to call
    *   All chips represented as fraction of total chips at table
    *   Divide by (initial_num_players * MAX_BUY_IN)
*   Minimum Bet
*   Maximum Bet (AKA chip count)
*   Amount in pot total
*   Amount in pot per player
*   Position (blinds)
*   Player hand rank
*   Players remaining in tournament
*   Active (unfolded) players
*   Big blind amount?
*   Player chip stack counts
*   Current betting round?

Outputs/ground truth:
*   Chosen player action
*   Bet amount
*   Reward/Q-value

Reward is calculated as the delta between the initial player chip count at the start of the game (before blinds) and the final player chip count, divided by total chips in the tournament to normalize (initial_num_players * MAX_BUY_IN).  Losing money will result in a negative reward, winning money in a positive reward, and folding without betting (or being a blind) will have a reward of zero.  This reward is applied to every state associated with that player during that game.

Outputs: value head outputing Q-value (above calculation) and policy head outputing probability of each legal move

### Converting the C++ Game State to a Neural Network Input



## Acknowledgements

Thanks to @p-ranav for use of his [tabulate](https://github.com/p-ranav/tabulate) table maker library.  I've included copies of his licenses in my repository along with the single header file version of his code.

## References

1.  “Texas Hold’em Poker probabilities,” randomprobabilities.net.
http://randomprobabilities.net/texas-holdem.php (accessed Jan. 02, 2024).

2.  “Texas Hold’em Guide | A Beginners Guide To Texas Hold’em,” www.thepokerbank.com. https://www.thepokerbank.com/guide/

3.  “Strategy: Blind structure - how the blinds increase,” PokerStrategy.com, 2024. https://www.pokerstrategy.com/strategy/live-poker/how-blinds-increase-structure/ (accessed Jan. 02, 2024).

‌
## Appendices

### Appendix A: Abbreviated No Limit Texas Hold 'Em Poker Rules

Texas Hold 'Em is a popular poker variant that has community cards that all players at the table use to complete a poker hand.  Each player is dealt two secret "hole" cards to begin, and the rest of the cards are dealt face up on the table as the game proceeds.  At the start of each game, two players at the table must make mandatory minimum bets called "blinds."  There is a "small blind" and a "big blind," and these blinds rotate around the table after each game.

There are up to five community cards dealt during the different phases of the game.  The first phase of the game is the pre-Flop, where the players are dealt their two hole cards and betting begins.  Assuming that at least two players at the table don't fold, the next phase of the game is the Flop where three community cards are dealt.  Betting commences, and then the third phase of the game is called the Turn where a single community card is dealt.  The fourth and final betting round is called the River, where the fifth and final community card is dealt.  If at least two players are still active after the River, the game goes to the Showdown where each player must show their cards and the winner is determined based on the best 5-card poker hand.

The following is a partial list of rules that are specific to the No Limit Texas Hold 'Em poker, some of which were tricky to implement:

#### Blinds and Order of Play
*   A dealer button is assigned to a player to keep track of the order of play
    *   The dealer button is rotated clockwise around the table at the start of each new game.
    *   The small blind is the player directly to the left (clockwise) of the dealer button
    *   The big blind is the player directly to the left (clockwise) of the small blind

*   Blinds are mandatory bets that these players must make at the start of each game
    *   The big blind is twice the value of the small blind
    *   Blinds are considered the first bet of the game, and so the next bet during the pre-flop is actually a raise
    *   The maximum buy-in is the maximum number of chips that the player is allowed to bring to the table, and it is calculated as 100 times the initial big blind value
    *   The value of the blinds increase as the poker tournament continues.  The structure of this increase depends on how fast you want the game to go, and I used this suggested structure from [3] to increase the blind value after a user-defined number of games played
    *   If a player cannot afford the blind they are automatically put all-in

*   The dealer deals hole cards starting with the first player to his left - the small blind.  
    *   The small blind is also the first player to act after the flop
    *   Before the flop, the big blind always acts last - which means the Under the Gun (UTG) player always goes first.  UTG is the player to the left (clockwise) of the big blind
    *   For heads-up play (two players) the dealer button is also the small blind
        *   The dealer never deals to himself first, and so the big blind receives the first hole card and will be the first to act post-flop
        *   Pre-flop the big blind still acts last, meaning the dealer/small blind acts first.

#### Raising
*   Raises do not have a maximum size in No Limit Hold 'Em (a player can go all-in)
    *   The minimum raise is the previous raise size in the current betting round.  If no raise has occurred yet the minimum raise size is the value of the big blind
    *   If a player raises less than the minimum raise (by going all-in) the betting action is not re-opened
        *   This means that the player who made the previous bet and any player that called that previous bet in that betting round can call or fold but cannot re-raise
        *   A player who has not yet called the previous bet is allowed to re-raise on his turn after the player goes all-in

#### Side Pots
*   If there are N players at the poker table, there could theoretically be the main pot and as many as N - 1 side pots.
    *   Side pots occur when a player goes all-in and betting continues amongst the other players
        *   This could occur because a player could not meet the minimum raise or big blind
        *   It could also occur if the all-in player's stack is sufficient to meet the minimum raise, but other players have sufficient chips to re-raise
*   A player can only win a maximum of the amount that they bet from each player who called them
*   If the all-in player loses the Showdown the winner takes the side pot(s)
*   But if the all-in player wins the Showdown, he is only eligible to win the side pot(s) he has contributed to - not the main pot or any side pots that took place after he went all-in

#### Ties and Multiple Winners
*   If there are N players at the poker table, there could theoretically be an N-way tie (e.g. community cards are a Royal Flush and no one folds before the River)
*   In the case of a tie, the pot is split evenly amongst the winners.  If the pot is not evenly divisible then the remaining chip(s) are given to the player closest to the left (clockwise) of the dealer.
*   It's possible for there to be multiple ties splitting multiple side pots, and each pot must be split amongst the winners

## Appendix B: Heuristics for Playing Poker

*   Play only pocket pairs or suited connectors, and some only in certain positions per chart
*   Full ring of 10 players (7 - 10 players)
    *   Early position: AA, KK, QQ, JJ, AKsuited
    *   Mid: AQsuited, TT, AK
    *   Late: AJsuited, KQsuited, AQ, 99
*   Short handed (6 players or less)
    *   Early: AA KK QQ JJ AKsuited AQsuited AK
    *   Mid: AQ AJsuited, KQsuited, TT, 99
    *   Late: ATsuited AJ KJsuited 88
*   The above is for tight play.  Add more hands for loose play


*   Betting strategy
    *   Bet 4 BB pre-flop, plus 1 BB for every "limper" (player who calls BB) before you
    *   bet 75% to 100% of pot size after flop


*   Size of stack affects strategy:
    *   Small stack is less than 25 blinds
        *   Play tight, stick to the best hands only
    *   Mid stack is 25 to 80 blinds
        *   Don't play small pairs or suited connectors as much
    *   big stack is greater than 80 blinds
        *   Play wider range of hands, consider raising more (5 BB pre flop for instance)

*   Gambits
    *   Float play
        *   Meant as a counter to Cbetting
        *   Check opponent's Cbet with the intention of betting or raising on the turn
        *   Must be heads-up with opponent (1v1)
        *   Should have position over opponent (i.e. they act first)
        *   Have a good hand and/or room to improve
    *   Check raise
        *   check after flop, then raise 3 to 3.5 times bet of player who bets after you
        *   Should be done if first after flop or no one has bet before you
        *   Should be done with a good (but not great) hand or on a straight/flush draw - goal is to make opponent fold rather than let them see the next street
    *   Bluffing
        *   Should be done rarely, and only when situation is right
        *   Your last action should have been strong
        *   Your opponent's last action shoudl have been week
        *   You should have position over your opponent (i.e. play after them)
        *   Better to bluff small pots than large ones to reduce risk of losing too much money
        *   Bluffing on the flop is typically better than bluffing on the river, unless you have played strong all the way to that point

    *   Cbet (continuation bet)
        *   Make a raise pre-flop, and then bet again on flop regardless of cards
        *   Bet 75 to 100% of the pot
        *   Hand doesn't have to be great, but should have potential
        *   Definitely Cbet if you flopped a strong hand
        *   Can Cbet almost every time

*   Pot odds
    *   Calculating pot odds can be easy, but probably easier to run a monte carlo simulation and use the win rate rather than calculating the odds of drawing a particular scenario
    *   Deciding whether to call or not:
        *   Estimate the odds of winning the hand using a MC simulation
        *   Compare the size of the bet to the size of the pot:
            *   Divide bet size by total size of pot (pot + bet)
        *   If probability of winning is higher than pot odds then call, otherwise fold

*   Positioning (Pre-flop)
    *   Early position (worse to better)
        *   UTG (third after flop, first pre-flop)
        *   UTG + 1
        *   UTG + 2
    *   Mid position (worse to better)
        *   UTG + 3
        *   UTG + 4
        *   UTG + 5
        *   UTG + 6
    *   Late position (worse to best)
        *   Dealer button (last after flop, third-to-last pre-flop)
        *   Small blind (first after flop, second-to-last pre-flop)
        *   Big Blind (second after flop, last pre-flop)
        
*   Positioning (Post-flop)
    *   Early position (worst to better)
        *   Small blind (first after flop, second-to-last pre-flop)
        *   Big Blind (second after flop, last pre-flop)
        *   UTG (third after flop, first pre-flop)
        *   UTG + 1
    *   Mid position (best to worst)
        *   UTG + 4
        *   UTG + 3
        *   UTG + 2
    *   Late position (best to worst)
        *   Dealer button (last after flop, third-to-last pre-flop)
        *   UTG + 6
        *   UTG + 5
    *   These positions will change strength as players are eliminated or fold
    *   There is also relative positioning to consider, as a cbet after the flop can effectively start action at a new position
    *   Determine positioning by counting how many players between current position and player who raised/bet, and how many players after current position but before position of betting player.

*   Overall heuristic strategy:
    *   Pre-flop
        *   Use positioning and stack size to determine list of acceptable hands to play
        *   Raise 4BB + 1 BB per limper if opportunity to first raise, so a 5BB bet minimum (1 BB call + 4 BB raise)
            *   If better hand and stronger position maybe have normal distribution?
        *   Call/reraise based on hand strength and position.  Maybe have a list of good cards and positions and then a RNG to make the decision?
    *   Post-flop
        *   Calculating raise
            *   Reverse pot odds decision-making process and assume a ratio of 1 for win % / pot odds
            *   bet = (win% * pot size) / (1 - win%)
            *   If bet is less than 75% of pot size then check
            *   If bet is more than 100% of pot size then use normal distribution centered at 100% of pot size with 1 SD = (bet size - pot size) / 3
            *   Round to nearest BB and make sure chip stack is suffficient!
            *   Also make sure it exceeds min raise
        *   Deciding to call or fold:
            *   Calculate pot odds and run MC simulation for chance to win
            *   Divide chance to win by pot odds
            *   If ratio is >= 1, then call
            *   If ratio is < 1, then run a uniform RNG between 0 and 1
                *   If the random number is <= ratio then call, otherwise fold

## Appendix C: Notes
I received an error when trying to run the C shared library from Python with Clib:

`libstdc++.so.6: version GLIBCXX_3.4.30' not found`

I found a [forum post](https://askubuntu.com/questions/1418016/glibcxx-3-4-30-not-found-in-conda-environment) that indicated that the lib file needed to be linked into the Conda environment.  The command below worked for me:

`ln -sf /usr/lib/x86_64-linux-gnu/libstdc++.so.6 /home/{$user}/miniconda3/envs/${env}/lib/libstdc++.so.6`

Where:

`${user}` is the Ubuntu username
`${env}` is the Conda environment name
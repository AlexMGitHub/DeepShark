# DeepShark
 A project to apply deep reinforcement learning to the game of Texas hold 'em poker.

The name of the project, DeepShark, is a compounding of the phrases "deep learning" and "card shark."  Deep learning refers to machine learning methods that utilize artificial neural network architectures, while a "shark" is slang for a highly skilled poker player.  I would like to train an ANN to play poker well, hence DeepShark.

### Description of Poker Environment

### Initial "Dumb" AI Players
I needed to be able to test the poker environment to ensure that the game rules were properly implemented.  I did this by writing "dumb" (i.e. hard-coded) AI player classes to play out poker games.  The first "AI" that I created was a purely random player who would randomly choose from the list of legal actions.  If the chosen action was a bet or raise, the AI would randomly choose a value between the minimum bet and the maximum value of its chip stack.

This very simple AI allowed me to test out my user interface, serialization/deserialization code, and catch some bugs in the poker environment.  But once the code seemed functional I added another "scripted" AI class that accepts a pre-planned list of actions to take.  I used these scripted AI players to perform functional testing of the code by devising a few scripted scenarios that involved complicated end-game logic such as multiple side pots and ties.  This testing allowed me to catch a few subtle bugs in the poker code which led to improper payouts when there were multiple winners.

I next wanted to write an AI class that would play somewhat intelligently but still maintain a stochastic nature.  This AI could play out longer and more realistic poker tournaments and possibly even be used to generate initial training data for a neural network.  I wanted this AI to have a rough idea of the strength of its current hand, but not get too bogged down into calculating complicated drawing odds and outs for a particular hand.

I used some of the formulas and tables in [1] to generate approximate probabilities of winning the game based on the current hand without considering any draws or number of outs.  The math quickly gets complicated, and so I only generated look-up tables for a few specific scenarios and used ballpark-correct approximations for the calculations.  I applied a stochastic element to the AI's decision making by using a Gaussian distribution to randomly generate numbers.  The Gaussian distribution's average and standard deviation are determined both by the strength of the current hand and by the AI's play style.  The random number is compared to a threshold that is determined by the AI's play style.  The resulting action will depend upon whether the random number exceeded the threshold or not.  The expected result is a player AI that will typically make reasonable decisions based on the strength of its current hand, but can unpredictably take actions such as bluffing on a weak hand, slow playing a strong hand, calling another player's bluff, etc.

### Considerations for Neural Network Architecture
One point of consideration is whether the neural network architecture ought to be some sort of recurrent neural network or other configuration that remembers previous states.

Without memory, the neural network will essentially just be learning how strong a particular poker hand is.  This functionality could be easily reproduced by a look-up table.  An ANN with memory of previous states could identify player behavior patterns.  Poker playing styles tend to fall along a two-axis spectrum: players can be "tight" or "loose" and "aggressive" or "passive."  The resulting four quadrants describe play styles that could be identified and exploited by an opponent.  For instance, a loose and aggressive play style describes a player who plays a wide range of starting hands and bets aggressively - which means that they are often bluffing.  An observant opponent might notice this tendency to play weak hands and bluff, and be more likely to call their raises.  Similarly, a tight and aggressive player will only play a narrow range of hands and will bet aggressively when he has good cards.  An observant player would be less likely to call their bets as they probably aren't bluffing.

### Development Environment
DeepShark was developed using Windows Subsystem for Linux (WSL) with an Ubuntu image.  The reported g++ compiler version used for the C++ code is:

`g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0`

There is some non-portable code (e.g. system("clear") to clear the terminal) that will not run in a non-Linux environment.

## Abbreviated No Limit Texas Hold 'Em Poker Rules

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
    *   The value of the blinds increase as the poker tournament continues.  The structure of this increase depends on how fast you want the game to go, and I used this suggested structure from [PokerStrategy](https://www.pokerstrategy.com/strategy/live-poker/how-blinds-increase-structure/) to increase the blind value every time a player is eliminated from the tournament
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

## References and Acknowledgements

[1] http://randomprobabilities.net/texas-holdem.php

Thanks to @p-ranav for use of his [tabulate](https://github.com/p-ranav/tabulate) table maker library.  I've included copies of his licenses in my repository along with the single header file version of his code.
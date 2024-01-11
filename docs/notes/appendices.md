
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

### Appendix B: Heuristics for Playing Poker

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

### Appendix C: Mapping the C++ Game State to a Neural Network Input

Inputs (60 inputs):
*   Legal Actions (7 inputs)
    *   Vector of length 7 corresponding to 7 potential actions
    *   1 if action is legal, 0 otherwise
*   Hole Cards (10 inputs)
    *   Four suits are one-hot encoded, length 4
    *   Rank is ordinal encoded (normalized by dividing by 13)
    *   All 0s for no suit/rank
    *   5 inputs per card, two cards = 10 inputs
*   Flop Cards (3 cards * 5 = 15 inputs)
*   Turn Card (5 inputs)
*   River Card (5 inputs)
*   Chips to call (1 input)
    *   All chips represented as fraction of total chips at table
    *   Divide by (initial_num_players * MAX_BUY_IN)
*   Maximum Bet (AKA chip count) (1 input)
*   Amount in pot total (1 input)
*   Amount in pot from player (1 inputs)
*   Position (blinds) (11 inputs)
*   Player hand rank (1 input)
*   Active (unfolded) players (1 inputs)
*   Players remaining in tournament (1 inputs)

Outputs/ground truth (8 outputs):
*   Q-values/rewards of each action (7 outputs)
    *   +1 for increase chip count, -1 for decrease, +0 for same
    *   Illegal actions masked with -2 so never chosen by argmax()
*   Bet amount, scaled by stack size (1 output)

### Appendix D: Notes

I received an error when trying to run the C shared library from Python with Clib:

`libstdc++.so.6: version GLIBCXX_3.4.30' not found`

I found a [forum post](https://askubuntu.com/questions/1418016/glibcxx-3-4-30-not-found-in-conda-environment) that indicated that the lib file needed to be linked into the Conda environment.  The command below worked for me:

`ln -sf /usr/lib/x86_64-linux-gnu/libstdc++.so.6 /home/{$user}/miniconda3/envs/${env}/lib/libstdc++.so.6`

Where:

`${user}` is the Ubuntu username
`${env}` is the Conda environment name

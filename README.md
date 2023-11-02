# DeepShark
 A project to apply deep reinforcement learning to the game of Texas hold 'em poker.

The name of the project, DeepShark, is a compounding of the phrases "Deep learning" and "card Shark."  Deep learning refers to machine learning methods that utilize artificial neural network architectures, while a "shark" is slang for a highly skilled poker player.  I would like to train an ANN to play poker well, hence DeepShark.

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

    



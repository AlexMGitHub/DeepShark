/******************************************************************************
 * Define classes used to represent playing cards.
 *****************************************************************************/

 /* Headers
 ******************************************************************************/
 // C++ standard library

#include <algorithm> // For sort()
#include <cassert>
#include <iostream>
#include <random> // For shuffle()
#include <ranges> // For views::drop()
#include <stdexcept>
#include <string>
#include <utility> // For to_underlying()
// Project headers
#include "cards.hpp"
// Using statements
using std::cout;
using std::endl;
using std::sort;
using std::to_underlying;
using std::vector;

/* Card Method Definitions
******************************************************************************/
bool Card::operator==(const Card& other)
{
    return  rank == other.rank &&
        suit == other.suit;
}

/* Deck Method Definitions
******************************************************************************/
void Deck::m_build_deck()
{
    size_t size = 0;
    for (const auto& i : Card_Suits)
    {
        for (const auto& j : Card_Ranks)
        {
            m_cards[size++] = Card(i, j);
        }
    }
    assert((m_cards.size() == NUMBER_CARDS_IN_DECK) &&
        "Invalid  number of cards in deck!");
}

void Deck::burn_one_card()
{
    m_top_card_idx++;
}

Card& Deck::deal_one_card()
{
    return m_cards[m_top_card_idx++];
}

void Deck::print_cards()
{
    for (const auto& i : m_cards)
    {
        std::cout << std::to_underlying(i.suit) << std::to_underlying(i.rank)
            << std::endl;
    }
}

void Deck::shuffle_deck()
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(m_cards.begin(), m_cards.end(), g);
    std::cout << "Deck shuffled!" << std::endl;
}

/* Hand Method Definitions
******************************************************************************/
bool Hand::card_sorter(const Card& lhs, const Card& rhs)
{
    return lhs.rank < rhs.rank;
}

bool Hand::reverse_card_sorter(const Card& lhs, const Card& rhs)
{
    return lhs.rank > rhs.rank;
}

void Hand::sort_cards()
{
    std::sort(m_available_cards.begin(), m_available_cards.end(),
        Hand::card_sorter);
}

bool Hand::is_one_pair()
{
    high_hand.clear();
    vector<Card> remaining_cards;
    Rank pair_rank = Rank::No_Card;
    std::map<Rank, int> count;
    for (const auto& card : m_available_cards)
    {
        count[card.rank]++;
    }
    for (const auto& [key, value] : count)
    {
        if (value == 2)
        {
            pair_rank = key;
        }
    }
    if (pair_rank == Rank::No_Card)
    {
        return false;
    }
    for (const auto& card : m_available_cards)
    {
        if (card.rank == pair_rank)
        {
            high_hand.push_back(card);
        }
        else
        {
            remaining_cards.push_back(card);
        }
    }
    std::sort(remaining_cards.begin(), remaining_cards.end(), Hand::card_sorter);
    high_hand.insert(high_hand.end(), remaining_cards.end() - 3, remaining_cards.end());
    assert((high_hand.size() == MAX_CARDS_IN_HAND) && "Invalid  number of cards in hand!");
    return true;
}

bool Hand::is_two_pair()
{
    high_hand.clear();
    vector<Rank> pairs;
    Card high_card;
    std::map<Rank, int> count;
    for (const auto& card : m_available_cards)
    {
        count[card.rank]++;
    }
    for (const auto& [key, value] : count)
    {
        if (value == 2)
        {
            pairs.push_back(key);
        }
    }
    if (pairs.size() < 2)
    {
        return false;
    }
    if (pairs.size() == 3)
    {
        // Potentially could be 3 pairs, so choose two highest-ranked pairs
        std::sort(pairs.begin(), pairs.end());
        pairs.erase(pairs.begin());
    }
    for (const auto& card : m_available_cards)
    {
        if (card.rank == pairs[0] || card.rank == pairs[1])
        {
            high_hand.push_back(card);
        }
        else if (card.rank > high_card.rank)
        {
            high_card = card;
        }
    }
    high_hand.push_back(high_card);
    assert((high_hand.size() == MAX_CARDS_IN_HAND) && "Invalid  number of cards in hand!");
    return true;
}

bool Hand::is_three_of_a_kind()
{
    high_hand.clear();
    bool three = false;
    Rank three_rank = Rank::No_Card;
    std::map<Rank, int> count;
    for (const auto& card : m_available_cards)
    {
        count[card.rank]++;
    }
    for (const auto& [key, value] : count)
    {
        if (value == 3)
        {
            three = true;
            three_rank = key;
        }
    }
    if (!three)
    {
        return false;
    }
    Card high_card;
    Card next_highest_card;
    for (const auto& card : m_available_cards)
    {
        if (card.rank == three_rank)
        {
            high_hand.push_back(card);
        }
        else if (card.rank > high_card.rank)
        {
            next_highest_card = high_card;
            high_card = card;
        }
        else if (card.rank > next_highest_card.rank)
        {
            next_highest_card = card;
        }
    }
    high_hand.push_back(high_card);
    high_hand.push_back(next_highest_card);
    assert((high_hand.size() == MAX_CARDS_IN_HAND) && "Invalid  number of cards in hand!");
    return true;
}

bool Hand::is_straight()
{
    return is_straight(m_available_cards);
}

bool Hand::is_straight(std::vector<Card> hand)
{
    straight_hand.clear();
    std::sort(hand.begin(), hand.end(), Hand::card_sorter);
    int consecutive_cards = 1;
    int current_card = 0;
    for (std::size_t i = 0, e = hand.size(); i != e; i++)
    {
        current_card = to_underlying(hand[i].rank);
        straight_hand.push_back(hand[i]);
        for (const auto& j : hand | std::views::drop(i + 1))
        {
            if (to_underlying(j.rank) == current_card + consecutive_cards)
            {

                straight_hand.push_back(j);
                consecutive_cards++;
            }
            else
            {
                straight_hand.clear();
                break;
            }
        }
        if (consecutive_cards >= 5)
        {
            std::vector<Card> sub_hand = { straight_hand.end() - MAX_CARDS_IN_HAND, straight_hand.end() };
            straight_hand = sub_hand;
            return true;
        }
        else
        {
            consecutive_cards = 1;
        }
    }
    return false;
}

bool Hand::is_flush()
{
    d.clear();
    h.clear();
    c.clear();
    s.clear();
    flush_hand.clear();
    for (const auto& i : m_available_cards)
    {
        switch (i.suit)
        {
        case Suit::Diamond:
            d.push_back(i);
            break;
        case Suit::Heart:
            h.push_back(i);
            break;
        case Suit::Club:
            c.push_back(i);
            break;
        case Suit::Spade:
            s.push_back(i);
            break;
        default:
            throw std::invalid_argument("Received invalid No_Card suit.");
        }
    }
    if (d.size() >= 5 || h.size() >= 5 || c.size() >= 5 || s.size() >= 5)
    {
        // Choose 5 highest ranked cards of the same suit
        if (d.size() >= 5)
        {
            std::sort(d.begin(), d.end(), Hand::card_sorter);
            flush_hand = { d.end() - 5, d.end() };
        }
        else if (h.size() >= 5)
        {
            std::sort(h.begin(), h.end(), Hand::card_sorter);
            flush_hand = { h.end() - 5, h.end() };
        }
        else if (c.size() >= 5)
        {
            std::sort(c.begin(), c.end(), Hand::card_sorter);
            flush_hand = { c.end() - 5, c.end() };
        }
        else if (s.size() >= 5)
        {
            std::sort(s.begin(), s.end(), Hand::card_sorter);
            flush_hand = { s.end() - 5, s.end() };
        }
    }
    else
    {
        return false;
    }
    sort(flush_hand.begin(), flush_hand.end(), Hand::reverse_card_sorter);
    for (size_t i = 0; i < MAX_CARDS_IN_HAND; i++)
    {
        best_hand[i] = flush_hand[i];
    }
    sort(flush_hand.begin(), flush_hand.end(), Hand::card_sorter);
    sort(best_hand.begin(), best_hand.end(), Hand::card_sorter);
    return true;
}

bool Hand::is_full_house()
{
    high_hand.clear();
    bool three = false;
    Rank three_rank = Rank::No_Card;
    bool pair = false;
    Rank pair_rank = Rank::No_Card;
    std::map<Rank, int> count;
    for (const auto& card : m_available_cards)
    {
        count[card.rank]++;
    }
    for (const auto& [key, value] : count)
    {
        if (value == 3)
        {
            three = true;
            three_rank = key;
        }
        else if (value == 2)
        {
            pair = true;
            // Could potentially be two pairs, choose highest rank pair for full house
            if (key > pair_rank)
            {
                pair_rank = key;
            }
        }
    }
    if (!(pair && three))
    {
        return false;
    }
    for (const auto& card : m_available_cards)
    {
        if (card.rank == three_rank || card.rank == pair_rank)
        {
            high_hand.push_back(card);
        }
    }
    return true;
}

bool Hand::is_four_of_a_kind()
{
    high_hand.clear();
    std::map<Rank, int> count;
    for (const auto& card : m_available_cards)
    {
        count[card.rank]++;
    }
    for (const auto& [key, value] : count)
    {
        if (value == 4)
        {
            Card high_card;
            for (const auto& card : m_available_cards)
            {
                if (card.rank == key)
                {
                    high_hand.push_back(card);
                }
                else if (card.rank > high_card.rank)
                {
                    high_card = card;
                }
            }
            high_hand.push_back(high_card);
            assert((high_hand.size() == MAX_CARDS_IN_HAND) && "Invalid  number of cards in hand!");
            return true;
        }
        assert((value <= 4) && "Cannot have more than 4 of a kind!");
    }
    return false;
}

bool Hand::is_straight_flush()
{
    std::vector<Card> new_flush_hand;
    if (d.size() > 5 || h.size() > 5 || c.size() > 5 || s.size() > 5)
    {
        if (d.size() > 5)
        {
            new_flush_hand = d;
        }
        else if (h.size() >= 5)
        {
            new_flush_hand = h;
        }
        else if (c.size() >= 5)
        {
            new_flush_hand = c;
        }
        else if (s.size() >= 5)
        {
            new_flush_hand = s;
        }
    }
    else
    {
        return is_straight(flush_hand); // Only five cards of same suit
    }

    // Have either 6 or 7 cards of the same suit

    // If have 7 cards of same suit:
    // Check middle 5 cards to see if they could be a straight flush
    sort(new_flush_hand.begin(), new_flush_hand.end(), Hand::card_sorter);
    if (new_flush_hand.size() == 7)
    {
        std::vector<Card> sub_hand = { new_flush_hand.begin() + 1, new_flush_hand.end() - 1 };
        if (is_straight(sub_hand) && !is_straight(flush_hand))
        {
            flush_hand = sub_hand;
            return is_straight(flush_hand);
        }
    }

    // If have 6 cards of same suit or middle 5 cards weren't a flush:
    // Check first 5 cards to see if they could be a straight flush
    std::vector<Card> sub_hand2 = { new_flush_hand.begin(), new_flush_hand.begin() + 5 };
    if (is_straight(sub_hand2) && !is_straight(flush_hand))
    {
        flush_hand = sub_hand2;
        return is_straight(flush_hand);
    }
    return is_straight(flush_hand);
}

void Hand::add_card(const Card& c)
{
    m_available_cards.push_back(c);
}

void Hand::clear_hand()
{
    m_available_cards.clear();
    d.clear();
    h.clear();
    c.clear();
    s.clear();
    flush_hand.clear();
    straight_hand.clear();
    best_hand.fill(Card(Suit::No_Card, Rank::No_Card));
    hand_rank = HandRank::High_Card;
}

void Hand::determine_best_hand()
{
    assert((m_available_cards.size() >= MAX_CARDS_IN_HAND) &&
        "Too few cards in deck to determine best hand!");
    sort_cards();
    // print_hand();
    if (is_flush())
    {
        hand_rank = HandRank::Flush;
        if (is_straight_flush())
        {
            hand_rank = HandRank::Straight_Flush;
            if (flush_hand[MAX_CARDS_IN_HAND - 1].rank == Rank::Ace)
            {
                hand_rank = HandRank::Royal_Flush;
            }
        }
        for (size_t i = 0; i < MAX_CARDS_IN_HAND; i++)
        {
            best_hand[i] = flush_hand[i];
        }
    }
    else if (is_four_of_a_kind())
    {
        hand_rank = HandRank::Four_of_a_Kind;
        for (size_t i = 0; i < MAX_CARDS_IN_HAND; i++)
        {
            best_hand[i] = high_hand[i];
        }
    }
    else if (is_full_house())
    {
        hand_rank = HandRank::Full_House;
        for (size_t i = 0; i < MAX_CARDS_IN_HAND; i++)
        {
            best_hand[i] = high_hand[i];
        }
    }
    else if (hand_rank == HandRank::Flush)
    {
        return;
    }
    else if (is_straight())
    {
        hand_rank = HandRank::Straight;
        for (size_t i = 0; i < MAX_CARDS_IN_HAND; i++)
        {
            best_hand[i] = straight_hand[i];
        }
    }
    else if (is_three_of_a_kind())
    {
        hand_rank = HandRank::Three_of_a_Kind;
        for (size_t i = 0; i < MAX_CARDS_IN_HAND; i++)
        {
            best_hand[i] = high_hand[i];
        }
    }
    else if (is_two_pair())
    {
        hand_rank = HandRank::Two_Pair;
        for (size_t i = 0; i < MAX_CARDS_IN_HAND; i++)
        {
            best_hand[i] = high_hand[i];
        }
    }
    else if (is_one_pair())
    {
        hand_rank = HandRank::Pair;
        for (size_t i = 0; i < MAX_CARDS_IN_HAND; i++)
        {
            best_hand[i] = high_hand[i];
        }
    }
    else
    {
        hand_rank = HandRank::High_Card;
        size_t offset = m_available_cards.size() - MAX_CARDS_IN_HAND;
        for (size_t i = 0; i < MAX_CARDS_IN_HAND; i++)
        {
            best_hand[i] = m_available_cards[i + offset];
        }
    }
    std::sort(best_hand.begin(), best_hand.end(), Hand::card_sorter);
}

void Hand::print_best_hand()
{
    if (best_hand.size() == 0)
    {
        cout << "No cards" << endl;
        return;
    }

    for (const auto& j : best_hand)
    {
        cout << std::to_underlying(j.rank) << std::to_underlying(j.suit) << " ";
    }
    cout << endl;
}

void Hand::print_hand()
{
    if (m_available_cards.size() == 0)
    {
        cout << "No cards" << endl;
        return;
    }

    for (const auto& j : m_available_cards)
    {
        cout << std::to_underlying(j.rank) << std::to_underlying(j.suit) << " ";
    }
    cout << endl;
}

void Hand::print_hand_rank()
{
    std::string rank_str;
    switch (hand_rank)
    {
    case HandRank::High_Card:
        rank_str = "High Card";
        break;
    case HandRank::Pair:
        rank_str = "Pair";
        break;
    case HandRank::Two_Pair:
        rank_str = "Two Pairs";
        break;
    case HandRank::Three_of_a_Kind:
        rank_str = "Three of a Kind";
        break;
    case HandRank::Straight:
        rank_str = "Straight";
        break;
    case HandRank::Flush:
        rank_str = "Flush";
        break;
    case HandRank::Full_House:
        rank_str = "Full House";
        break;
    case HandRank::Four_of_a_Kind:
        rank_str = "Four of a Kind";
        break;
    case HandRank::Straight_Flush:
        rank_str = "Straight Flush";
        break;
    case HandRank::Royal_Flush:
        rank_str = "Royal Flush";
        break;
    }
    cout << "Hand Rank: " << rank_str << endl;
}
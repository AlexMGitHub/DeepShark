/******************************************************************************
 * Contains functions necessary to implement the Texas Hold 'Em environment.
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
#include "texasholdem.hpp"
// Using statements
using std::cout;
using std::endl;
using std::sort;
using std::to_underlying;
using std::vector;

/* Pot Method Definitions
******************************************************************************/
void Pot::add_chips(unsigned chips)
{
    m_chip_count += chips;
}

unsigned Pot::get_chip_count()
{
    return m_chip_count;
}

/* Private Pot Method Definitions */
unsigned Pot::m_pay_to_winner()
{
    unsigned temp = m_chip_count;
    m_chip_count = 0;
    return temp;
}

/* Dealer Method Definitions
******************************************************************************/
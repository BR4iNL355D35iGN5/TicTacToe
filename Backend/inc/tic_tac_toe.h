/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "board_table.h"
#include "player.h"

namespace bd
{
namespace tictactoe
{
namespace backend
{

class TicTacToe
{
public:
    TicTacToe();
    ~TicTacToe();

private:
    BoardTable m_boardTable;
    Player m_player[2];
    quint16 m_round;
};

} // end: namespace backend
} // end: namespace tictactoe
} // end: namespace bd

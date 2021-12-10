/**
  @file
  @author Alexander Wittrock
*/
#include "board_table.h"

namespace bd
{
namespace tictactoe
{
namespace backend
{

///////////////////////////////////////////////////////////////////////////////
BoardTable::BoardTable()
{
    reset();
}

///////////////////////////////////////////////////////////////////////////////
bool BoardTable::getValue(quint8 row, quint8 column, Value& value) const
{
    if(row >= NUM_ROWS || column >= NUM_COLUMNS)
    {
        return false;
    }

    value = m_values[row][column];

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool BoardTable::isCircle(quint8 row, quint8 column) const
{
    Value value;
    if(!getValue(row, column, value))
    {
        return false;
    }

    return value == Value::Circle;
}

///////////////////////////////////////////////////////////////////////////////
bool BoardTable::isCross(quint8 row, quint8 column) const
{
    Value value;
    if(!getValue(row, column, value))
    {
        return false;
    }

    return value == Value::Cross;
}

///////////////////////////////////////////////////////////////////////////////
bool BoardTable::isEmpty(quint8 row, quint8 column) const
{
    Value value;
    if(!getValue(row, column, value))
    {
        return false;
    }

    return value == Value::Empty;
}

///////////////////////////////////////////////////////////////////////////////
void BoardTable::reset()
{
    for(int y = 0; y < NUM_ROWS; ++y)
    {
        for(int x = 0; x < NUM_COLUMNS; ++x)
        {
            m_values[y][x] = Value::Empty;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool BoardTable::setValue(quint8 row, quint8 column, const Value& value)
{
    if(row >= NUM_ROWS || column >= NUM_COLUMNS)
    {
        return false;
    }

    if(m_values[row][column] != Value::Empty)
    {
        return false;
    }

    m_values[row][column] = value;
    return true;
}

} // end: namespace backend
} // end: namespace tictactoe
} // end: namespace bd

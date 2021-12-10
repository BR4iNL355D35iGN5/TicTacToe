/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include <qglobal.h>

namespace bd
{
namespace tictactoe
{
namespace backend
{

class BoardTable
{
public:
    enum class Value
    {
        Empty,
        Circle,
        Cross
    };

    BoardTable();

    bool getValue(quint8 row, quint8 column, Value& value) const;
    bool isCircle(quint8 row, quint8 column) const;
    bool isCross(quint8 row, quint8 column) const;
    bool isEmpty(quint8 row, quint8 column) const;
    void reset();
    bool setValue(quint8 row, quint8 column, const Value& value);

    static const quint8 NUM_COLUMNS = 3;
    static const quint8 NUM_ROWS =3;

private:
    Value m_values[3][3];
};

} // end: namespace backend
} // end: namespace tictactoe
} // end: namespace bd

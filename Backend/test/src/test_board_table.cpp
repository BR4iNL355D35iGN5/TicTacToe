/**
  @file
  @author Alexander Wittrock
*/
#include "board_table.h"

#include <QtTest/QtTest>

using namespace bd::tictactoe::backend;

class TestBoardTable : public QObject
{
    Q_OBJECT

private slots:
    void checkInitialization();
    void checkSetValue();

private:
    BoardTable m_boardTable;
};

///////////////////////////////////////////////////////////////////////////////
void TestBoardTable::checkInitialization()
{
    for(int y = 0; y < BoardTable::NUM_ROWS; ++y)
    {
        for(int x = 0; x < BoardTable::NUM_COLUMNS; ++x)
        {
            QVERIFY(m_boardTable.isEmpty(y, x));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void TestBoardTable::checkSetValue()
{
    for(int y = 0; y < BoardTable::NUM_ROWS; ++y)
    {
        for(int x = 0; x < BoardTable::NUM_COLUMNS; ++x)
        {
            QVERIFY(m_boardTable.setValue(y, x, BoardTable::Value::Cross));
        }
    }

    QVERIFY(m_boardTable.setValue(BoardTable::NUM_ROWS,
                                  BoardTable::NUM_COLUMNS,
                                  BoardTable::Value::Circle) == false);
    QVERIFY(m_boardTable.setValue(BoardTable::NUM_ROWS,
                                  0,
                                  BoardTable::Value::Circle) == false);
    QVERIFY(m_boardTable.setValue(0,
                                  BoardTable::NUM_COLUMNS,
                                  BoardTable::Value::Circle) == false);

    for(int y = 0; y < BoardTable::NUM_ROWS; ++y)
    {
        for(int x = 0; x < BoardTable::NUM_COLUMNS; ++x)
        {
            QVERIFY(m_boardTable.setValue(y, x, BoardTable::Value::Circle) == false);
        }
    }
}

QTEST_MAIN(TestBoardTable)
#include "test_board_table.moc"

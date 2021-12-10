/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "board_table.h"
#include "highscores.h"

#include <QByteArray>
#include <QString>

namespace bd
{
namespace tictactoe
{
namespace backend
{

class Player
{
public:
    Player();
    ~Player();

    quint32 getScore() const;
    quint32 getHighscore() const;
    HighscoreEntry getHighscoreEntry() const;
    void increaseScore();
    bool initialize();
    bool isCpu() const;
    void reset();

private:
    bool m_ai;
    QString m_authentificationToken;
    QByteArray m_avatar;
    quint32 m_highscore;
    QString m_password;
    quint32 m_score;
    QString m_user;
    BoardTable::Value m_value;
};

} // end: namespace backend
} // end: namespace tictactoe
} // end: namespace bd

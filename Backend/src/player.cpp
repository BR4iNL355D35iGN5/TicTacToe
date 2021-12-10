/**
  @file
  @author Alexander Wittrock
*/
#include "player.h"

namespace bd
{
namespace tictactoe
{
namespace backend
{

///////////////////////////////////////////////////////////////////////////////
Player::Player() :
    m_ai(false),
    m_authentificationToken(),
    m_avatar(),
    m_highscore(0),
    m_password(),
    m_score(0),
    m_user()
{
}

///////////////////////////////////////////////////////////////////////////////
quint32 Player::getHighscore() const
{
    return m_highscore;
}

///////////////////////////////////////////////////////////////////////////////
HighscoreEntry Player::getHighscoreEntry() const
{
    HighscoreEntry highscoreEntry;
    highscoreEntry.avatar = m_avatar;
    highscoreEntry.score = m_highscore;
    highscoreEntry.user = m_user;

    return highscoreEntry;
}

///////////////////////////////////////////////////////////////////////////////
quint32 Player::getScore() const
{
    return m_score;
}

///////////////////////////////////////////////////////////////////////////////
void Player::increaseScore()
{
    ++m_score;

    if(m_score > m_highscore)
    {
        m_highscore = m_score;
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Player::isCpu() const
{
    return m_ai;
}

///////////////////////////////////////////////////////////////////////////////
void Player::reset()
{
    m_ai = false;
    m_authentificationToken = {};
    m_avatar = {};
    m_highscore = 0;
    m_password = {};
    m_score = 0;
    m_user = {};
}

} // end: namespace backend
} // end: namespace tictactoe
} // end: namespace bd

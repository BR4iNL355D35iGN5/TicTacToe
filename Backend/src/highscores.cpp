/**
  @file
  @author Alexander Wittrock
*/
#include "highscores.h"

#include <QDataStream>
#include <QFile>

namespace bd
{
namespace tictactoe
{
namespace backend
{

///////////////////////////////////////////////////////////////////////////////
QDataStream &operator>>(QDataStream &in, HighscoreEntry& entry){
    in >> entry.avatar >> entry.score >> entry.user;
    return in;
}

///////////////////////////////////////////////////////////////////////////////
QDataStream &operator<<(QDataStream &out, const HighscoreEntry& entry){
    out << entry.avatar << entry.score << entry.user;
    return out;
}

///////////////////////////////////////////////////////////////////////////////
Highscores::Highscores(const QString& filePathName) :
    m_filePathName(filePathName),
    m_highscoreList()
{
    if(!m_filePathName.isEmpty())
    {
        load();
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Highscores::getEntry(qsizetype index, HighscoreEntry& highscoreEntry) const
{
    if(index >= m_highscoreList.count())
    {
        return false;
    }

    highscoreEntry = m_highscoreList[index];

    return true;
}

///////////////////////////////////////////////////////////////////////////////
const QList<HighscoreEntry>& Highscores::getHighscoreList() const
{
    return m_highscoreList;
}

///////////////////////////////////////////////////////////////////////////////
qsizetype Highscores::getNumEntries() const
{
    return m_highscoreList.count();
}

///////////////////////////////////////////////////////////////////////////////
bool Highscores::insertEntry(const HighscoreEntry& highscoreEntry)
{
    ///\todo check limit, sort entries and if in range insert the new entry
    m_highscoreList.push_back(highscoreEntry);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Highscores::load()
{
    if(!QFile::exists(m_filePathName))
    {
        return false;
    }

    QFile file(m_filePathName);
    if(!file.open(QIODevice::ReadOnly))
    {
        qCritical("Highscores::load() Could not open %s",
                  qPrintable(m_filePathName));
        return false;
    }

    m_highscoreList.clear();

    QDataStream dataStream(&file);
    while(!dataStream.atEnd())
    {
        HighscoreEntry entry;
        dataStream >> entry;
        m_highscoreList.push_back(entry);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Highscores::save(const QString& filePathName)
{
    if(!filePathName.isEmpty())
    {
        m_filePathName = filePathName;
    }

    QFile file(m_filePathName);
    if(!file.open(QIODevice::WriteOnly))
    {
        qCritical("Highscores::load() Could not open %s",
                  qPrintable(m_filePathName));
        return false;
    }

    QDataStream dataStream(&file);
    for(const auto& entry : m_highscoreList)
    {
        dataStream << entry;
    }

    if(!file.flush())
    {
        return false;
    }

    return true;
}

} // end: namespace backend
} // end: namespace tictactoe
} // end: namespace bd


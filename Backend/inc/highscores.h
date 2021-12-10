/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include <QByteArray>
#include <QList>
#include <QString>

namespace bd
{
namespace tictactoe
{
namespace backend
{

struct HighscoreEntry
{
    QByteArray avatar;
    quint32 score;
    QString user;
};

class Highscores
{
public:
    Highscores(const QString& filePathName = {});

    bool getEntry(qsizetype index, HighscoreEntry& highscoreEntry) const;
    const QList<HighscoreEntry>& getHighscoreList() const;
    qsizetype getNumEntries() const;
    bool insertEntry(const HighscoreEntry& highscoreEntry);
    bool load();
    bool save(const QString& filePathName = {});

private:
    static const quint8 MAX_ENTRIES = 100;

    QString m_filePathName;
    QList<HighscoreEntry> m_highscoreList;
};

} // end: namespace backend
} // end: namespace tictactoe
} // end: namespace bd

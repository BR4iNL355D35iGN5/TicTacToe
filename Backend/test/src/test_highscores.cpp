/**
  @file
  @author Alexander Wittrock
*/
#include "highscores.h"

#include <QtTest/QtTest>

using namespace bd::tictactoe::backend;

class TestHighscores : public QObject
{
    Q_OBJECT

private slots:
    void checkInitialization();
    void save();
    void load();

private:
    static const QString FILE_NAME_PATH;

    Highscores m_highscores;
    qsizetype m_numSavedEntries;
};

const QString TestHighscores::FILE_NAME_PATH = "./highscores.dat";

///////////////////////////////////////////////////////////////////////////////
void TestHighscores::checkInitialization()
{
    QVERIFY(m_highscores.getNumEntries() == 0);

    HighscoreEntry entry;
    entry.avatar = {};
    entry.user = "BR4iNL355";
    entry.score = 77;
    QVERIFY(m_highscores.insertEntry(entry));
    QVERIFY(m_highscores.getNumEntries() == 1);
}

///////////////////////////////////////////////////////////////////////////////
void TestHighscores::save()
{
    m_numSavedEntries = m_highscores.getNumEntries();
    QVERIFY(m_highscores.save(FILE_NAME_PATH));
}

///////////////////////////////////////////////////////////////////////////////
void TestHighscores::load()
{
    QVERIFY(m_highscores.load());
    QCOMPARE(m_numSavedEntries, m_highscores.getNumEntries());
    HighscoreEntry entry;
    QVERIFY(m_highscores.getEntry(0, entry));
    QCOMPARE(entry.avatar, QByteArray());
    QCOMPARE(entry.score, 77);
    QCOMPARE(entry.user, "BR4iNL355");
}

QTEST_MAIN(TestHighscores)
#include "test_highscores.moc"

#pragma once

#include <QtNetwork/QNetworkAccessManager>

class HttpRequest : public QObject
{
    Q_OBJECT
public:
    explicit HttpRequest(QObject *parent = nullptr);

    void send(const QString& request);

signals:

private:
    QNetworkAccessManager m_manager;
};

#if 0
EDIT LAMBDA SLOT: if not use lambda SIGNAL SLOT.

Discribe one slot in your .h file for example:

private slots:
    void managerFinished(QNetworkReply *reply);
in .cpp constructor replace lambda to

QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
    this, SLOT(managerFinished(QNetworkReply*)));
now in your slot:

void MainWindow::managerFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();

    qDebug() << answer;
}
#endif

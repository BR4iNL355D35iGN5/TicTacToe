#include "httprequest.h"

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

HttpRequest::HttpRequest(QObject *parent)
    : QObject{parent}
{
    QObject::connect(&m_manager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {
            if (reply->error()) {
                qDebug() << reply->errorString();
                return;
            }

            QString answer = reply->readAll();

            qDebug() << answer;
        }
    );
}

void HttpRequest::send(const QString& request)
{
    QNetworkRequest networkRequest;
    networkRequest.setUrl(QUrl(request));
    m_manager.get(networkRequest);
}


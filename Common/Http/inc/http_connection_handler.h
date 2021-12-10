/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "http_listener.h"

#include <QTcpSocket>
#include <QThread>
#include <QTimer>

namespace bd
{
namespace http
{

class HttpRequest;

class HTTP_API HttpConnectionHandler : public QObject
{
    Q_OBJECT

public:
    HttpConnectionHandler(const HttpListener& httpListener);
    virtual ~HttpConnectionHandler();

    bool isBusy() const;
    void setBusy();

public slots:
    void handleConnection(const bd::http::tSocketDescriptor socketDescriptor);

private slots:
    void disconnected();
    void read();
    void readTimeout();
    void threadDone();

private:
    bool m_busy;
    std::shared_ptr<HttpRequest> m_currentHttpRequest;
    const HttpListener& m_httpListener;
    std::shared_ptr<QTcpSocket> m_tcpSocket;
    QThread m_thread;
    QTimer m_timer;
};

} // end: namespace http
} // end: namespace bd

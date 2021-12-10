/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "http_listener.h"

#include <QMutex>
#include <QTimer>

namespace bd
{
namespace http
{

class HttpConnectionHandler;

class HTTP_API HttpConnectionHandlerPool : public QObject
{
    Q_OBJECT

public:
    HttpConnectionHandlerPool(const HttpListener& httpListener);
    virtual ~HttpConnectionHandlerPool();

    void clear();
    std::shared_ptr<HttpConnectionHandler> getConnectionHandler();

private slots:
    void cleanup();

private:
    QList<std::shared_ptr<HttpConnectionHandler>> m_connectionHandlers;
    const HttpListener& m_httpListener;
    QMutex m_mutex;
    QTimer m_timer;
};

} // end: namespace http
} // end: namespace bd

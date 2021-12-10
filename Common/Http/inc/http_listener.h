/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "http_configuration.h"

#include <QTcpServer>

namespace bd
{
namespace http
{

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    typedef qintptr tSocketDescriptor;
#else
    typedef int tSocketDescriptor;
#endif

#ifdef QT_NO_SSL
  #define QSslConfiguration QObject
#endif

class HttpConnectionHandlerPool;
class HttpRequestHandler;

class HTTP_API HttpListener : public QObject
{
    Q_OBJECT

public:
    HttpListener();
    virtual ~HttpListener();

    const HttpConfiguration& getConfiguration() const;

    const std::shared_ptr<HttpRequestHandler>& getHttpRequestHandler() const;

    bool isListening() const;

    bool startListening(const std::shared_ptr<HttpRequestHandler>& httpRequestHandler,
                        std::shared_ptr<HttpConfiguration> httpConfiguration = nullptr);

    void stopListening();

signals:
    void handleConnection(bd::http::tSocketDescriptor socketDescriptor);

protected:
    void incomingConnection(tSocketDescriptor socketDescriptor);

private:
    HttpConfiguration m_httpConfiguration;
    std::shared_ptr<HttpConnectionHandlerPool> m_httpConnectionHandlerPool;
    std::shared_ptr<HttpRequestHandler> m_httpRequestHandler;
    QTcpServer m_tcpServer;
};

} // end: namespace http
} // end: namespace bd

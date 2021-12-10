/**
  @file
  @author Alexander Wittrock
*/
#include "http_listener.h"
#include "http_connection_handler_pool.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpServer>
#include <QTcpSocket>

namespace bd
{
namespace http
{

///////////////////////////////////////////////////////////////////////////////
HttpListener::HttpListener() :
    QObject(),
    m_httpConfiguration(),
    m_httpConnectionHandlerPool(std::make_shared<HttpConnectionHandlerPool>(*this)),
    m_httpRequestHandler(nullptr)
{
    qRegisterMetaType<tSocketDescriptor>("tSocketDescriptor");
}

///////////////////////////////////////////////////////////////////////////////
HttpListener::~HttpListener()
{
    stopListening();
    qDebug("HttpListener: destroyed");
}

///////////////////////////////////////////////////////////////////////////////
const HttpConfiguration& HttpListener::getConfiguration() const
{
    return m_httpConfiguration;
}

///////////////////////////////////////////////////////////////////////////////
const std::shared_ptr<HttpRequestHandler>& HttpListener::getHttpRequestHandler() const
{
    return m_httpRequestHandler;
}

///////////////////////////////////////////////////////////////////////////////
void HttpListener::incomingConnection(tSocketDescriptor socketDescriptor)
{
    qDebug("HttpListener: New connection");

    auto freeHandler = m_httpConnectionHandlerPool->getConnectionHandler();
    if(freeHandler)
    {
/*
        QMetaObject::invokeMethod(freeHandler.get(),
                                  "handleConnection",
                                  Qt::QueuedConnection,
                                  Q_ARG(tSocketDescriptor, socketDescriptor));
*/
    }
    else
    {
        qDebug("HttpListener: Too many incoming connections");
        std::unique_ptr<QTcpSocket> tcpSocket = std::make_unique<QTcpSocket>(this);
        tcpSocket->setSocketDescriptor(socketDescriptor);
        connect(tcpSocket.get(), SIGNAL(disconnected()), tcpSocket.get(), SLOT(deleteLater()));
        tcpSocket->write("HTTP/1.1 503 too many connections\r\nConnection: "
                         "close\r\n\r\nToo many connections\r\n");
        tcpSocket->disconnectFromHost();
    }
}

///////////////////////////////////////////////////////////////////////////////
bool HttpListener::isListening() const
{
    return m_tcpServer.isListening();
}

///////////////////////////////////////////////////////////////////////////////
bool HttpListener::startListening(const std::shared_ptr<HttpRequestHandler>& httpRequestHandler,
                                  std::shared_ptr<HttpConfiguration> httpConfiguration)
{
    if(!httpRequestHandler)
    {
        qCritical("HttpListener: Invalid HTTP request handler");
        return false;
    }

    m_httpRequestHandler = httpRequestHandler;

    if(!httpConfiguration)
    {
        QJsonObject jsonObject;
        jsonObject["CleanupInterval"] = 1000;
        jsonObject["MaxMultiPartSize"] = 1000000;
        jsonObject["MaxParallelConnections"] = 100;
        jsonObject["MaxRequestSize"] = 16000;
        jsonObject["MinParallelConnections"] = 1;
        jsonObject["ReadTimeout"]= 10000;
        jsonObject["Host"] = "127.0.0.1";
        jsonObject["Port"] = 8080;
        httpConfiguration = std::make_shared<HttpConfiguration>(jsonObject);
    }

    if(!httpConfiguration->isObject())
    {
        qCritical("HttpListener: Invalid HTTP configuration");
        return false;
    }

    m_httpConfiguration = *httpConfiguration;
    const auto host = m_httpConfiguration.getHost();
    const auto port = m_httpConfiguration.getPort();

    m_tcpServer.listen(host.isEmpty() ? QHostAddress::Any : QHostAddress(host),
                        port);
    if(!m_tcpServer.isListening())
    {
        qCritical("HttpListener: Cannot bind on port %i: %s",
                  port, qPrintable(m_tcpServer.errorString()));
        return false;
    }
    else
    {
        qDebug("HttpListener: Listening on port %i", port);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void HttpListener::stopListening()
{
    m_tcpServer.close();
    m_httpConnectionHandlerPool->clear();
    qDebug("HttpListener: stopped listening");
}

} // end: namespace http
} // end: namespace bd

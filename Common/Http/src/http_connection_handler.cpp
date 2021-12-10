/**
  @file
  @author Alexander Wittrock
*/
#include "http_connection_handler.h"
#include "http_listener.h"
#include "http_request.h"
#include "http_request_handler.h"
#include "http_response.h"

#include <QJsonDocument>
#include <QJsonObject>

namespace bd
{
namespace http
{

///////////////////////////////////////////////////////////////////////////////
HttpConnectionHandler::HttpConnectionHandler(const HttpListener& httpListener) :
    m_busy(false),
    m_currentHttpRequest(),
    m_httpListener(httpListener),
    m_tcpSocket(nullptr),
    m_thread(),
    m_timer()
{
    m_thread.start();
    qDebug("HttpConnectionHandler (%p): thread started",
           static_cast<void*>(this));
    moveToThread(&m_thread);
    m_timer.moveToThread(&m_thread);
    m_timer.setSingleShot(true);

    ///\todo create socket

    connect(m_tcpSocket.get(), SIGNAL(readyRead()), SLOT(read()));
    connect(m_tcpSocket.get(), SIGNAL(disconnected()), SLOT(disconnected()));
    connect(&m_timer, SIGNAL(timeout()), SLOT(readTimeout()));
    connect(&m_thread, SIGNAL(finished()), this, SLOT(threadDone()));
}

///////////////////////////////////////////////////////////////////////////////
HttpConnectionHandler::~HttpConnectionHandler()
{
    m_thread.quit();
    m_thread.wait();
    m_thread.deleteLater();
    qDebug("HttpConnectionHandler (%p): destroyed", static_cast<void*>(this));
}

///////////////////////////////////////////////////////////////////////////////
bool HttpConnectionHandler::isBusy() const
{
    return m_busy;
}

///////////////////////////////////////////////////////////////////////////////
void HttpConnectionHandler::setBusy()
{
    m_busy = true;
}

///////////////////////////////////////////////////////////////////////////////
void HttpConnectionHandler::handleConnection(tSocketDescriptor socketDescriptor)
{
    qDebug("HttpConnectionHandler (%p): handle new connection",
           static_cast<void*>(this));
    m_busy = true;
    Q_ASSERT(!m_tcpSocket->isOpen());

    m_tcpSocket->connectToHost("", 0);
    m_tcpSocket->abort();

    if(!m_tcpSocket->setSocketDescriptor(socketDescriptor))
    {
        qCritical("HttpConnectionHandler (%p): cannot initialize socket: %s",
                  static_cast<void*>(this), qPrintable(m_tcpSocket->errorString()));
        return;
    }

/*
    #ifndef QT_NO_SSL
        // Switch on encryption, if SSL is configured
        if(sslConfiguration)
        {
            qDebug("HttpConnectionHandler (%p): Starting encryption", static_cast<void*>(this));
            (static_cast<QSslSocket*>(socket))->startServerEncryption();
        }
    #endif
*/

    m_timer.start(m_httpListener.getConfiguration().getReadTimeout());
    m_currentHttpRequest.reset();
}

///////////////////////////////////////////////////////////////////////////////
void HttpConnectionHandler::disconnected()
{
    qDebug("HttpConnectionHandler (%p): disconnected", static_cast<void*>(this));
    m_tcpSocket->close();
    m_timer.stop();
    m_busy = false;
}

///////////////////////////////////////////////////////////////////////////////
void HttpConnectionHandler::read()
{
    while(m_tcpSocket->bytesAvailable())
    {
        qDebug("HttpConnectionHandler (%p): read input",static_cast<void*>(this));

        if(!m_currentHttpRequest)
        {
            m_currentHttpRequest = std::make_unique<HttpRequest>(m_httpListener);
        }

        while(m_tcpSocket->bytesAvailable() &&
              m_currentHttpRequest->getStatus() != HttpRequest::Status::complete &&
              m_currentHttpRequest->getStatus() != HttpRequest::Status::abort)
        {
            m_currentHttpRequest->readFromSocket(m_tcpSocket);

            if(m_currentHttpRequest->getStatus() == HttpRequest::Status::waitForBody)
            {
                m_timer.start(m_httpListener.getConfiguration().getReadTimeout());
            }
        }

        if(m_currentHttpRequest->getStatus() == HttpRequest::Status::abort)
        {
            m_tcpSocket->write("HTTP/1.1 413 entity too large\r\n"
                               "Connection: close\r\n\r\n413 Entity too large\r\n");
            while(m_tcpSocket->bytesToWrite()) m_tcpSocket->waitForBytesWritten();
            m_tcpSocket->disconnectFromHost();
            m_currentHttpRequest.reset();
            return;
        }

        if(m_currentHttpRequest->getStatus() == HttpRequest::Status::complete)
        {
            m_timer.stop();
            qDebug("HttpConnectionHandler (%p): received request",static_cast<void*>(this));

            // Copy the Connection:close header to the response
            HttpResponse response(m_tcpSocket);
            bool closeConnection =
                QString::compare(m_currentHttpRequest->getHeader("Connection"),
                                 "close", Qt::CaseInsensitive) == 0;
            if(closeConnection)
            {
                response.setHeader("Connection", "close");
            }
            else
            {
                bool http1_0 = QString::compare(
                    m_currentHttpRequest->getVersion(),
                    "HTTP/1.0",
                    Qt::CaseInsensitive) == 0;
                if(http1_0)
                {
                    closeConnection = true;
                    response.setHeader("Connection", "close");
                }
            }

            try
            {
                m_httpListener.getHttpRequestHandler()->process(
                    m_currentHttpRequest, response);
            }
            catch(...)
            {
                qCritical("HttpConnectionHandler (%p): "
                          "An unhandled exception occured in the request handler",
                          static_cast<void*>(this));
            }

            if(!response.hasSentLastPart())
            {
                response.write(QByteArray(), true);
            }

            qDebug("HttpConnectionHandler (%p): finished request",
                   static_cast<void*>(this));

            if(!closeConnection)
            {
                const auto closeResponse =
                    QString::compare(response.getHeaders().value("Connection"),
                                     "close",Qt::CaseInsensitive) == 0;
                if(closeResponse)
                {
                    closeConnection = true;
                }
                else
                {
                    const auto hasContentLength =
                        response.getHeaders().contains("Content-Length");
                    if (!hasContentLength)
                    {
                        bool hasChunkedMode =
                            QString::compare(
                                response.getHeaders().value("Transfer-Encoding"),
                                "chunked", Qt::CaseInsensitive) == 0;
                        if(!hasChunkedMode)
                        {
                            closeConnection = true;
                        }
                    }
                }
            }

            if(closeConnection)
            {
                while(m_tcpSocket->bytesToWrite())
                {
                    m_tcpSocket->waitForBytesWritten();
                }
                m_tcpSocket->disconnectFromHost();
            }
            else
            {              
                m_timer.start(m_httpListener.getConfiguration().getReadTimeout());
            }

            m_currentHttpRequest.reset();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void HttpConnectionHandler::readTimeout()
{
    qDebug("HttpConnectionHandler (%p): read timeout occured",
           static_cast<void*>(this));
    while(m_tcpSocket->bytesToWrite()) m_tcpSocket->waitForBytesWritten();
    m_tcpSocket->disconnectFromHost();
    m_currentHttpRequest.reset();
}

///////////////////////////////////////////////////////////////////////////////
void HttpConnectionHandler::threadDone()
{
    m_timer.stop();
    m_tcpSocket->close();
    m_tcpSocket->reset();
    qDebug("HttpConnectionHandler (%p): thread stopped",
           static_cast<void*>(this));
}

} // end: namespace http
} // end: namespace bd

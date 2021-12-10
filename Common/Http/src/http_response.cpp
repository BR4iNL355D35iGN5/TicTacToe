/**
  @file
  @author Alexander Wittrock
*/
#include "http_response.h"

#include <QTcpSocket>

namespace bd
{
namespace http
{

///////////////////////////////////////////////////////////////////////////////
HttpResponse::HttpResponse(const std::shared_ptr<QTcpSocket>& tcpSocket) :
    m_chunkedMode(false),
    m_sentHeaders(false),
    m_sentLastPart(false),
    m_statusCode(200),
    m_statusText("OK"),
    m_tcpSocket(tcpSocket)
{
}

///////////////////////////////////////////////////////////////////////////////
const QMap<QByteArray,HttpCookie>& HttpResponse::getCookies() const
{
    return m_cookies;
}

///////////////////////////////////////////////////////////////////////////////
const QMap<QByteArray,QByteArray>& HttpResponse::getHeaders() const
{
    return m_headers;
}

///////////////////////////////////////////////////////////////////////////////
int HttpResponse::getStatusCode() const
{
   return m_statusCode;
}

///////////////////////////////////////////////////////////////////////////////
bool HttpResponse::hasSentLastPart() const
{
    return m_sentLastPart;
}

///////////////////////////////////////////////////////////////////////////////
bool HttpResponse::isConnected() const
{
    return m_tcpSocket->isOpen();
}

///////////////////////////////////////////////////////////////////////////////
bool HttpResponse::setCookie(const HttpCookie& cookie)
{
    Q_ASSERT(!m_sentHeaders);
    if(cookie.getName().isEmpty())
    {
        return false;
    }

    m_cookies.insert(cookie.getName(),cookie);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void HttpResponse::setHeader(const QByteArray& name, const QByteArray& value)
{
    Q_ASSERT(!m_sentHeaders);
    m_headers.insert(name, value);
}

///////////////////////////////////////////////////////////////////////////////
void HttpResponse::setHeader(const QByteArray& name, int value)
{
    Q_ASSERT(!m_sentHeaders);
    m_headers.insert(name, QByteArray::number(value));
}

///////////////////////////////////////////////////////////////////////////////
void HttpResponse::write(const QByteArray& data, bool lastPart)
{
    Q_ASSERT(!m_sentLastPart);

    if(!m_sentHeaders)
    {
        if(lastPart)
        {
           m_headers.insert("Content-Length", QByteArray::number(data.size()));
        }
        else
        {
            const auto connectionValue =
                m_headers.value("Connection", m_headers.value("connection"));
            const auto connectionClose =
                QString::compare(connectionValue,
                                 "close", Qt::CaseInsensitive) == 0;
            if(!connectionClose)
            {
                m_headers.insert("Transfer-Encoding", "chunked");
                m_chunkedMode = true;
            }
        }

        writeHeaders();
    }

    if(data.size() > 0)
    {
        if(m_chunkedMode)
        {
            if(data.size() > 0)
            {
                const auto size = QByteArray::number(data.size(), 16);
                writeToSocket(size);
                writeToSocket("\r\n");
                writeToSocket(data);
                writeToSocket("\r\n");
            }
        }
        else
        {
            writeToSocket(data);
        }
    }

    if(lastPart)
    {
        if(m_chunkedMode)
        {
            writeToSocket("0\r\n\r\n");
        }
        m_tcpSocket->flush();
        m_sentLastPart = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
void HttpResponse::writeHeaders()
{
    Q_ASSERT(!m_sentHeaders);
    QByteArray buffer;
    buffer.append("HTTP/1.1 ");
    buffer.append(QByteArray::number(m_statusCode));
    buffer.append(' ');
    buffer.append(m_statusText);
    buffer.append("\r\n");

    for(const auto& name : m_headers.keys())
    {
        buffer.append(name);
        buffer.append(": ");
        buffer.append(m_headers.value(name));
        buffer.append("\r\n");
    }

    for(const auto& cookie : m_cookies.values())
    {
        buffer.append("Set-Cookie: ");
        buffer.append(cookie.toByteArray());
        buffer.append("\r\n");
    }
    buffer.append("\r\n");
    writeToSocket(buffer);
    m_tcpSocket->flush();
    m_sentHeaders = true;
}

///////////////////////////////////////////////////////////////////////////////
bool HttpResponse::writeToSocket(const QByteArray& data)
{
    auto remaining = data.size();
    auto dataPtr = data.data();

    while(m_tcpSocket->isOpen() && remaining > 0)
    {
        if(m_tcpSocket->bytesToWrite() > 16384)
        {
            m_tcpSocket->waitForBytesWritten(-1);
        }

        const auto written = m_tcpSocket->write(dataPtr, remaining);
        if(written == -1)
        {
          return false;
        }

        dataPtr += written;
        remaining -= written;
    }

    return true;
}

} // end: namespace http
} // end: namespace bd

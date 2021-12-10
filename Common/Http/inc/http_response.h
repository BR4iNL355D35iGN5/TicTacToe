/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "http_cookie.h"

#include <QMap>

class QTcpSocket;

namespace bd
{
namespace http
{

class HTTP_API HttpResponse
{
public:
    HttpResponse(const std::shared_ptr<QTcpSocket>& tcpSocket);

    const QMap<QByteArray,HttpCookie>& getCookies() const;
    const QMap<QByteArray,QByteArray>& getHeaders() const;
    int getStatusCode() const;
    bool hasSentLastPart() const;
    bool isConnected() const;
    bool setCookie(const HttpCookie& cookie);
    void setHeader(const QByteArray& name, const QByteArray& value);
    void setHeader(const QByteArray& name, int value);
    void write(const QByteArray& data, bool lastPart = false);

private:
    void writeHeaders();
    bool writeToSocket(const QByteArray& data);

    bool m_chunkedMode;
    QMap<QByteArray, HttpCookie> m_cookies;
    QMap<QByteArray, QByteArray> m_headers;
    bool m_sentHeaders;
    bool m_sentLastPart;
    int m_statusCode;
    QByteArray m_statusText;
    const std::shared_ptr<QTcpSocket>& m_tcpSocket;
};

} // end: namespace http
} // end: namespace bd

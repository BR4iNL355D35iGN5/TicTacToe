/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "http_listener.h"

#include <QHostAddress>

class QTcpSocket;
class QTemporaryFile;

namespace bd
{
namespace http
{

class HTTP_API HttpRequest : public QObject
{
    Q_OBJECT

public:
    enum class Status
    {
        waitForRequest,
        waitForHeader,
        waitForBody,
        complete,
        abort
    };

    HttpRequest(const HttpListener& httpListener);
    virtual ~HttpRequest();

    const QByteArray& getBodyData() const;
    QByteArray getCookie(const QByteArray& name) const;
    const QMap<QByteArray,QByteArray>& getCookieMap() const;
    QByteArray getHeader(const QByteArray& name) const;
    const QMultiMap<QByteArray,QByteArray>& getHeaderMap() const;
    QList<QByteArray> getHeaders(const QByteArray& name) const;
    const QByteArray& getMethod() const;
    QByteArray getParameter(const QByteArray& name) const;
    const QMultiMap<QByteArray,QByteArray>& getParameterMap() const;
    QList<QByteArray> getParameters(const QByteArray& name) const;
    QByteArray getPath() const;
    const QHostAddress& getPeerAddress() const;
    const QByteArray& getRawPath() const;
    Status getStatus() const;
    std::shared_ptr<QTemporaryFile> getUploadedFile(const QByteArray& fieldName) const;
    const QByteArray& getVersion() const;

    void readFromSocket(std::shared_ptr<QTcpSocket> tcpSocket);

private:
    static QByteArray urlDecode(const QByteArray& source);

    void decodeRequestParams();
    void extractCookies();
    void parseMultiPartFile();
    void readBody(std::shared_ptr<QTcpSocket> tcpSocket);
    void readHeader(std::shared_ptr<QTcpSocket> tcpSocket);
    void readRequest(std::shared_ptr<QTcpSocket> tcpSocket);

    //static constexpr qint64 64K_BLOCK_SIZE = 65536;
    QByteArray m_bodyData;
    QByteArray m_boundary;
    QMap<QByteArray,QByteArray> m_cookies;
    QByteArray m_currentHeader;
    int m_currentSize;
    int m_expectedBodySize;
    QMultiMap<QByteArray,QByteArray> m_headers;
    const HttpListener& m_httpListener;
    QByteArray m_lineBuffer;
    int m_maxMultiPartSize;
    int m_maxRequestSize;
    QByteArray m_method;
    QMultiMap<QByteArray,QByteArray> m_parameters;
    QByteArray m_path;
    QHostAddress m_peerAddress;
    Status m_status;
    std::shared_ptr<QTemporaryFile> m_temporaryFile;
    QMap<QByteArray,std::shared_ptr<QTemporaryFile>> m_uploadedFiles;
    QByteArray m_version;
};

} // end: namespace http
} // end: namespace bd

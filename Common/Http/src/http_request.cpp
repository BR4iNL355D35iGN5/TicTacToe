/**
  @file
  @author Alexander Wittrock
*/
#include "http_request.h"
#include "http_cookie.h"

#include <QTcpSocket>
#include <QTemporaryFile>

namespace bd
{
namespace http
{

///////////////////////////////////////////////////////////////////////////////
HttpRequest::HttpRequest(const HttpListener& httpListener) :
    m_bodyData(),
    m_boundary(),
    m_cookies(),
    m_currentSize(0),
    m_expectedBodySize(0),
    m_headers(),
    m_httpListener(httpListener),
    m_lineBuffer(),
    m_maxMultiPartSize(httpListener.getConfiguration().getMaxMultiPartSize()),
    m_maxRequestSize(httpListener.getConfiguration().getMaxRequestSize()),
    m_method(),
    m_parameters(),
    m_path(),
    m_status(Status::waitForRequest),
    m_uploadedFiles(),
    m_version()
{
}

///////////////////////////////////////////////////////////////////////////////
HttpRequest::~HttpRequest()
{
    for(auto key : m_uploadedFiles.keys())
    {
        auto uploadedFile = m_uploadedFiles.value(key);
        if(uploadedFile->isOpen())
        {
            uploadedFile->close();
        }
        uploadedFile.reset();
    }

    if(m_temporaryFile)
    {
        if(m_temporaryFile->isOpen())
        {
            m_temporaryFile->close();
        }
        m_temporaryFile.reset();
    }
}

///////////////////////////////////////////////////////////////////////////////
void HttpRequest::decodeRequestParams()
{
    qDebug("HttpRequest: extract and decode request parameters");

    // Get URL parameters
    QByteArray rawParameters;
    int questionMark = m_path.indexOf('?');
    if(questionMark >= 0)
    {
        rawParameters = m_path.mid(questionMark+1);
        m_path = m_path.left(questionMark);
    }
    // Get request body parameters
    QByteArray contentType = m_headers.value("content-type");
    if(!m_bodyData.isEmpty() &&
       (contentType.isEmpty() ||
        contentType.startsWith("application/x-www-form-urlencoded")))
    {
        if(!rawParameters.isEmpty())
        {
            rawParameters.append('&');
            rawParameters.append(m_bodyData);
        }
        else
        {
            rawParameters = m_bodyData;
        }
    }
    // Split the parameters into pairs of value and name
    const auto parameterList = rawParameters.split('&');
    for(const auto& parameter : parameterList)
    {
        const auto equalsChar = parameter.indexOf('=');
        if(equalsChar >= 0)
        {
            QByteArray name=parameter.left(equalsChar).trimmed();
            QByteArray value=parameter.mid(equalsChar + 1).trimmed();
            m_parameters.insert(urlDecode(name), urlDecode(value));
        }
        else if(!parameter.isEmpty())
        {
            m_parameters.insert(urlDecode(parameter), "");
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void HttpRequest::extractCookies()
{
    qDebug("HttpRequest: extract cookies");

    for(auto cookieData : m_headers.values("cookie"))
    {
        const auto list = HttpCookie::splitCSV(cookieData);
        for(const auto& part : list)
        {
            qDebug("HttpRequest: found cookie %s",part.data());
            QByteArray name;
            QByteArray value;
            const auto pos = part.indexOf('=');

            if(pos)
            {
                name = part.left(pos).trimmed();
                value = part.mid(pos + 1).trimmed();
            }
            else
            {
                name = part.trimmed();
                value = "";
            }

            m_cookies.insert(name,value);
        }
    }

    m_headers.remove("cookie");
}

///////////////////////////////////////////////////////////////////////////////
const QByteArray& HttpRequest::getBodyData() const
{
    return m_bodyData;
}

///////////////////////////////////////////////////////////////////////////////
QByteArray HttpRequest::getCookie(const QByteArray& name) const
{
    return m_cookies.value(name);
}

///////////////////////////////////////////////////////////////////////////////
const QMap<QByteArray,QByteArray>& HttpRequest::getCookieMap() const
{
    return m_cookies;
}

///////////////////////////////////////////////////////////////////////////////
QByteArray HttpRequest::getHeader(const QByteArray& name) const
{
    return m_headers.value(name.toLower());
}

///////////////////////////////////////////////////////////////////////////////
const QMultiMap<QByteArray,QByteArray>& HttpRequest::getHeaderMap() const
{
    return m_headers;
}

///////////////////////////////////////////////////////////////////////////////
QList<QByteArray> HttpRequest::getHeaders(const QByteArray& name) const
{
    return m_headers.values(name.toLower());
}

///////////////////////////////////////////////////////////////////////////////
const QByteArray& HttpRequest::getMethod() const
{
    return m_method;
}

///////////////////////////////////////////////////////////////////////////////
QByteArray HttpRequest::getPath() const
{
    return urlDecode(m_path);
}

///////////////////////////////////////////////////////////////////////////////
QByteArray HttpRequest::getParameter(const QByteArray& name) const
{
    return m_parameters.value(name);
}

///////////////////////////////////////////////////////////////////////////////
const QMultiMap<QByteArray,QByteArray>& HttpRequest::getParameterMap() const
{
    return m_parameters;
}

///////////////////////////////////////////////////////////////////////////////
QList<QByteArray> HttpRequest::getParameters(const QByteArray& name) const
{
    return m_parameters.values(name);
}

///////////////////////////////////////////////////////////////////////////////
const QHostAddress& HttpRequest::getPeerAddress() const
{
    return m_peerAddress;
}

///////////////////////////////////////////////////////////////////////////////
const QByteArray& HttpRequest::getRawPath() const
{
    return m_path;
}

///////////////////////////////////////////////////////////////////////////////
HttpRequest::Status HttpRequest::getStatus() const
{
    return m_status;
}

///////////////////////////////////////////////////////////////////////////////
std::shared_ptr<QTemporaryFile> HttpRequest::getUploadedFile(const QByteArray& fieldName) const
{
    return m_uploadedFiles.value(fieldName);
}

///////////////////////////////////////////////////////////////////////////////
const QByteArray& HttpRequest::getVersion() const
{
    return m_version;
}

///////////////////////////////////////////////////////////////////////////////
void HttpRequest::parseMultiPartFile()
{
    qDebug("HttpRequest: parsing multipart temp file");
    m_temporaryFile->seek(0);
    auto finished = false;
    while(!m_temporaryFile->atEnd() &&
          !finished &&
          !m_temporaryFile->error())
    {
        qDebug("HttpRequest: reading multpart headers");

        QByteArray fieldName;
        QByteArray fileName;
        while(!m_temporaryFile->atEnd() &&
              !finished &&
              !m_temporaryFile->error())
        {
            const auto line = m_temporaryFile->readLine(65536).trimmed();
            if(line.startsWith("Content-Disposition:"))
            {
                if(line.contains("form-data"))
                {
                    auto start = line.indexOf(" name=\"");
                    auto end = line.indexOf("\"", start + 7);
                    if(start >= 0 && end >= start)
                    {
                        fieldName = line.mid(start + 7, end - start - 7);
                    }
                    start = line.indexOf(" filename=\"");
                    end = line.indexOf("\"", start + 11);
                    if(start >=0 && end >= start)
                    {
                        fileName=line.mid(start + 11, end - start - 11);
                    }

                    qDebug("HttpRequest: multipart field=%s, filename=%s",
                           fieldName.data(), fileName.data());
                }
                else
                {
                    qDebug("HttpRequest: ignoring unsupported content part %s",
                           line.data());
                }
            }
            else if(line.isEmpty())
            {
                break;
            }
        }

        qDebug("HttpRequest: reading multpart data");
        std::shared_ptr<QTemporaryFile> uploadedFile;
        QByteArray fieldValue;
        while(!m_temporaryFile->atEnd() &&
              !finished &&
              !m_temporaryFile->error())
        {
            QByteArray line = m_temporaryFile->readLine(65536);
            if(line.startsWith("--" + m_boundary))
            {
                // Boundary found. Until now we have collected 2 bytes too much,
                // so remove them from the last result
                if(fileName.isEmpty() && !fieldName.isEmpty())
                {
                    // last field was a form field
                    fieldValue.remove(fieldValue.size() - 2, 2);
                    m_parameters.insert(fieldName, fieldValue);
                    qDebug("HttpRequest: set parameter %s=%s",
                           fieldName.data(), fieldValue.data());
                }
                else if(!fileName.isEmpty() && !fieldName.isEmpty())
                {
                    // last field was a file
                    if(uploadedFile)
                    {
                        qDebug("HttpRequest: finishing writing to uploaded file");
                        uploadedFile->resize(uploadedFile->size() - 2);
                        uploadedFile->flush();
                        uploadedFile->seek(0);
                        m_parameters.insert(fieldName, fileName);
                        qDebug("HttpRequest: set parameter %s=%s",
                               fieldName.data(), fileName.data());
                        m_uploadedFiles.insert(fieldName, uploadedFile);
                        long int fileSize=(long int) uploadedFile->size();
                        qDebug("HttpRequest: uploaded file size is %li",fileSize);
                    }
                    else
                    {
                        qWarning("HttpRequest: format error, unexpected end of file data");
                    }
                }
                if(line.contains(m_boundary + "--"))
                {
                    finished = true;
                }
                break;
            }
            else
            {
                if(fileName.isEmpty() && !fieldName.isEmpty())
                {
                    // this is a form field.
                    m_currentSize += line.size();
                    fieldValue.append(line);
                }
                else if(!fileName.isEmpty() && !fieldName.isEmpty())
                {
                    // this is a file
                    if(!uploadedFile)
                    {
                        uploadedFile = std::make_shared<QTemporaryFile>();
                        uploadedFile->open();
                    }
                    uploadedFile->write(line);
                    if(uploadedFile->error())
                    {
                        qCritical("HttpRequest: error writing temp file, %s",
                                  qPrintable(uploadedFile->errorString()));
                    }
                }
            }
        }
    }
    if(m_temporaryFile->error())
    {
        qCritical("HttpRequest: cannot read temp file, %s",
                  qPrintable(m_temporaryFile->errorString()));
    }

    qDebug("HttpRequest: finished parsing multipart temp file");
}

///////////////////////////////////////////////////////////////////////////////
void HttpRequest::readFromSocket(std::shared_ptr<QTcpSocket> tcpSocket)
{
    Q_ASSERT(m_status != Status::complete);
    switch(m_status)
    {
    case Status::waitForRequest:
        readRequest(tcpSocket);
        break;
    case Status::waitForHeader:
        readHeader(tcpSocket);
        break;
    case Status::waitForBody:
        readBody(tcpSocket);
        break;
    default:
        break;
    }

    if((m_boundary.isEmpty() && m_currentSize > m_maxRequestSize) ||
       (!m_boundary.isEmpty() && m_currentSize > m_maxMultiPartSize))
    {
        qWarning("HttpRequest: received too many bytes");
        m_status = Status::abort;
    }

    if(m_status == Status::complete)
    {
        decodeRequestParams();
        extractCookies();
    }
}

///////////////////////////////////////////////////////////////////////////////
void HttpRequest::readBody(std::shared_ptr<QTcpSocket> tcpSocket)
{
    Q_ASSERT(m_expectedBodySize != 0);
    if(m_boundary.isEmpty())
    {
        qDebug("HttpRequest: receive body");
        int toRead = m_expectedBodySize - m_bodyData.size();
        QByteArray newData = tcpSocket->read(toRead);
        m_currentSize += newData.size();
        m_bodyData.append(newData);
        if(m_bodyData.size() >= m_expectedBodySize)
        {
            m_status = Status::complete;
        }
    }
    else
    {
        qDebug("HttpRequest: receiving multipart body");
        if(!m_temporaryFile)
        {
            m_temporaryFile = std::make_shared<QTemporaryFile>();
        }

        if(!m_temporaryFile->isOpen())
        {
            m_temporaryFile->open();
        }

        // Transfer data in 64kb blocks
        auto fileSize = m_temporaryFile->size();
        qint64 toRead = m_expectedBodySize - fileSize;
        if(toRead > 65536)
        {
            toRead = 65536;
        }
        fileSize += m_temporaryFile->write(tcpSocket->read(toRead));
        if(fileSize >= m_maxMultiPartSize)
        {
            qWarning("HttpRequest: received too many multipart bytes");
            m_status = Status::abort;
        }
        else if(fileSize >= m_expectedBodySize)
        {
            qDebug("HttpRequest: received whole multipart body");
            m_temporaryFile->flush();
            if(m_temporaryFile->error())
            {
                qCritical("HttpRequest: Error writing temp file for multipart body");
            }

            parseMultiPartFile();
            m_temporaryFile->close();
            m_status = Status::complete;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void HttpRequest::readHeader(std::shared_ptr<QTcpSocket> tcpSocket)
{
     // allow one byte more to be able to detect overflow
    int toRead = m_maxRequestSize - m_currentSize + 1;
    const auto dataRead = tcpSocket->readLine(toRead);
    m_currentSize += dataRead.size();
    m_lineBuffer.append(dataRead);

    if(!m_lineBuffer.contains("\r\n"))
    {
        qDebug("HttpRequest: collecting more parts until line break");
        return;
    }

    const auto newData = m_lineBuffer.trimmed();
    m_lineBuffer.clear();
    int colon = newData.indexOf(':');

    if(colon > 0)
    {
        m_currentHeader = newData.left(colon).toLower();
        const auto value = newData.mid(colon + 1).trimmed();
        m_headers.insert(m_currentHeader, value);
        qDebug("HttpRequest: received header %s: %s",
               m_currentHeader.data(), value.data());
    }
    else if(!newData.isEmpty())
    {
        qDebug("HttpRequest: read additional line of header");
        if(m_headers.contains(m_currentHeader))
        {
            m_headers.insert(m_currentHeader,
                             m_headers.value(m_currentHeader) + " " + newData);
        }
    }
    else
    {
        qDebug("HttpRequest: headers completed");
        const auto contentType = m_headers.value("content-type");
        if(contentType.startsWith("multipart/form-data"))
        {
            auto pos = contentType.indexOf("boundary=");
            if (pos>=0)
            {
                m_boundary = contentType.mid(pos + 9);
                if(m_boundary.startsWith('"') && m_boundary.endsWith('"'))
                {
                   m_boundary = m_boundary.mid(1, m_boundary.length() - 2);
                }
            }
        }

        const auto contentLength = m_headers.value("content-length");
        if(!contentLength.isEmpty())
        {
            m_expectedBodySize = contentLength.toInt();
        }

        if(m_expectedBodySize == 0)
        {
            qDebug("HttpRequest: expect no body");
            m_status = Status::complete;
        }
        else if(m_boundary.isEmpty() &&
                (m_expectedBodySize + m_currentSize) > m_maxRequestSize)
        {
            qWarning("HttpRequest: expected body is too large");
            m_status = Status::abort;
        }
        else if(!m_boundary.isEmpty() &&
                m_expectedBodySize > m_maxMultiPartSize)
        {
            qWarning("HttpRequest: expected multipart body is too large");
            m_status = Status::abort;
        }
        else
        {
            qDebug("HttpRequest: expect %i bytes body", m_expectedBodySize);
            m_status = Status::waitForBody;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void HttpRequest::readRequest(std::shared_ptr<QTcpSocket> tcpSocket)
{
    qDebug("HttpRequest: read request");
    int toRead = m_maxRequestSize - m_currentSize + 1;
    const auto lineData = tcpSocket->readLine(toRead);
    m_currentSize += lineData.size();
    m_lineBuffer.append(lineData);

    if (!m_lineBuffer.contains("\r\n"))
    {
        qDebug("HttpRequest: collecting more parts until line break");
        return;
    }

    const auto newData = m_lineBuffer.trimmed();
    m_lineBuffer.clear();

    if(newData.isEmpty())
    {
        return;
    }

    qDebug("HttpRequest: from %s: %s",
           qPrintable(tcpSocket->peerAddress().toString()),
           newData.data());
    const auto list = newData.split(' ');

    if(list.count() != 3 || !list.at(2).contains("HTTP"))
    {
        qWarning("HttpRequest: received broken HTTP request, invalid first line");
        m_status = Status::abort;
    }
    else
    {
        m_method = list.at(0).trimmed();
        m_path = list.at(1);
        m_version = list.at(2);
        m_peerAddress = tcpSocket->peerAddress();
        m_status = Status::waitForHeader;
    }
}

///////////////////////////////////////////////////////////////////////////////
QByteArray HttpRequest::urlDecode(const QByteArray& source)
{
    QByteArray buffer(source);
    buffer.replace('+', ' ');
    int percentChar = buffer.indexOf('%');

    while(percentChar >= 0)
    {
        bool ok;
        const auto hexCode = buffer.mid(percentChar+1,2).toInt(&ok,16);
        if(ok)
        {
            auto c = static_cast<char>(hexCode);
            buffer.replace(percentChar, 3, &c, 1);
        }
        percentChar = buffer.indexOf('%', percentChar + 1);
    }

    return buffer;
}

} // end: namespace http
} // end: namespace bd

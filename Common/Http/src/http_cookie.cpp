/**
  @file
  @author Alexander Wittrock
*/
#include "http_cookie.h"

namespace bd
{
namespace http
{

///////////////////////////////////////////////////////////////////////////////
QList<QByteArray> HttpCookie::splitCSV(const QByteArray& source)
{
    auto inString = false;
    QList<QByteArray> list;
    QByteArray buffer;

    for(auto i = 0; i < source.size(); ++i)
    {
        char c=source.at(i);
        if(inString == false)
        {
            if(c == '\"')
            {
                inString = true;
            }
            else if(c == ';')
            {
                const auto trimmed = buffer.trimmed();
                if(!trimmed.isEmpty())
                {
                    list.append(trimmed);
                }
                buffer.clear();
            }
            else
            {
                buffer.append(c);
            }
        }
        else
        {
            if(c == '\"')
            {
                inString=false;
            }
            else
            {
                buffer.append(c);
            }
        }
    }

    const auto trimmed = buffer.trimmed();
    if(!trimmed.isEmpty())
    {
        list.append(trimmed);
    }

    return list;
}

///////////////////////////////////////////////////////////////////////////////
HttpCookie::HttpCookie(const QByteArray& source) :
    m_httpOnly(false),
    m_maxAge(0),
    m_secure(false),
    m_version(1)
{
    const auto list = splitCSV(source);
    for(const auto& part : list)
    {
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

        if(name == "Comment")
        {
            m_comment = value;
        }
        else if(name == "Domain")
        {
            m_domain = value;
        }
        else if(name == "Max-Age")
        {
            m_maxAge = value.toInt();
        }
        else if(name == "Path")
        {
            m_path = value;
        }
        else if(name == "Secure")
        {
            m_secure = true;
        }
        else if(name == "HttpOnly")
        {
            m_httpOnly = true;
        }
        else if(name == "SameSite")
        {
            m_sameSite = value;
        }
        else if(name == "Version")
        {
            m_version = value.toInt();
        }
        else
        {
            if(m_name.isEmpty())
            {
                m_name = name;
                m_value = value;
            }
            else
            {
                qWarning("HttpCookie: Ignoring unknown %s=%s",
                         name.data(), value.data());
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
const QByteArray& HttpCookie::getComment() const
{
    return m_comment;
}

///////////////////////////////////////////////////////////////////////////////
const QByteArray& HttpCookie::getDomain() const
{
    return m_domain;
}

///////////////////////////////////////////////////////////////////////////////
bool HttpCookie::getHttpOnly() const
{
    return m_httpOnly;
}

///////////////////////////////////////////////////////////////////////////////
int HttpCookie::getMaxAge() const
{
    return m_maxAge;
}

///////////////////////////////////////////////////////////////////////////////
const QByteArray& HttpCookie::getName() const
{
    return m_name;
}

///////////////////////////////////////////////////////////////////////////////
const QByteArray& HttpCookie::getPath() const
{
    return m_path;
}

///////////////////////////////////////////////////////////////////////////////
const QByteArray& HttpCookie::getSameSite() const
{
    return m_sameSite;
}

///////////////////////////////////////////////////////////////////////////////
bool HttpCookie::getSecure() const
{
    return m_secure;
}

///////////////////////////////////////////////////////////////////////////////
const QByteArray& HttpCookie::getValue() const
{
    return m_value;
}

///////////////////////////////////////////////////////////////////////////////
int HttpCookie::getVersion() const
{
    return m_version;
}

///////////////////////////////////////////////////////////////////////////////
QByteArray HttpCookie::toByteArray() const
{
    QByteArray buffer(m_name);
    buffer.append('=');
    buffer.append(m_value);

    if(!m_comment.isEmpty())
    {
        buffer.append("; Comment=");
        buffer.append(m_comment);
    }

    if(!m_domain.isEmpty())
    {
        buffer.append("; Domain=");
        buffer.append(m_domain);
    }

    if(m_maxAge != 0)
    {
        buffer.append("; Max-Age=");
        buffer.append(QByteArray::number(m_maxAge));
    }

    if(!m_path.isEmpty())
    {
        buffer.append("; Path=");
        buffer.append(m_path);
    }

    if(m_secure)
    {
        buffer.append("; Secure");
    }

    if(m_httpOnly)
    {
        buffer.append("; HttpOnly");
    }

    if(!m_sameSite.isEmpty())
    {
        buffer.append("; SameSite=");
        buffer.append(m_sameSite);
    }

    buffer.append("; Version=");
    buffer.append(QByteArray::number(m_version));
    return buffer;
}

} // end: namespace http
} // end: namespace bd

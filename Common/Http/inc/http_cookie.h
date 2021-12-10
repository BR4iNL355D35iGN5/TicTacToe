/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "export.h"

#include <QByteArray>
#include <QList>

namespace bd
{
namespace http
{

class HTTP_API HttpCookie
{
public:
    static QList<QByteArray> splitCSV(const QByteArray& source);

    HttpCookie(const QByteArray& source);

    const QByteArray& getComment() const;
    const QByteArray& getDomain() const;
    bool getHttpOnly() const;
    int getMaxAge() const;
    const QByteArray& getName() const;
    const QByteArray& getPath() const;
    const QByteArray& getSameSite() const;
    bool getSecure() const;
    QByteArray toByteArray() const;
    const QByteArray& getValue() const;
    int getVersion() const;

private:
    QByteArray m_comment;
    QByteArray m_domain;
    QByteArray m_name;
    bool m_httpOnly;
    int m_maxAge;
    QByteArray m_path;
    QByteArray m_sameSite;
    bool m_secure;
    QByteArray m_value;
    int m_version;
};

} // end: namespace http
} // end: namespace bd

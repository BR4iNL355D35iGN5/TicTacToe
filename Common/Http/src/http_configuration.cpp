/**
  @file
  @author Alexander Wittrock
*/

#include "http_configuration.h"

#include <QJsonObject>

namespace bd
{
namespace http
{

///////////////////////////////////////////////////////////////////////////////
HttpConfiguration::HttpConfiguration(const QJsonObject& jsonObject) :
    QJsonDocument(jsonObject)
{
}

///////////////////////////////////////////////////////////////////////////////
int HttpConfiguration::getCleanupInterval() const
{
    const auto json = object();
    return json.value("CleanupInterval").toInt();
}

///////////////////////////////////////////////////////////////////////////////
QString HttpConfiguration::getHost() const
{
    const auto json = object();
    return json.value("Host").toString();
}

///////////////////////////////////////////////////////////////////////////////
int HttpConfiguration::getMaxMultiPartSize() const
{
    const auto json = object();
    return json.value("MaxMutiPartSize").toInt();
}

///////////////////////////////////////////////////////////////////////////////
int HttpConfiguration::getMaxParallelConnections() const
{
    const auto json = object();
    return json.value("MaxParallelConnections").toInt();
}

///////////////////////////////////////////////////////////////////////////////
int HttpConfiguration::getMaxRequestSize() const
{
    const auto json = object();
    return json.value("MaxRequestSize").toInt();
}

///////////////////////////////////////////////////////////////////////////////
int HttpConfiguration::getMinParallelConnections() const
{
    const auto json = object();
    return json.value("MinParallelConnections").toInt();
}

///////////////////////////////////////////////////////////////////////////////
quint16 HttpConfiguration::getPort() const
{
    const auto json = object();
    return static_cast<quint16>(json.value("Port").toInteger() & 0xFFFF);
}

///////////////////////////////////////////////////////////////////////////////
int HttpConfiguration::getReadTimeout() const
{
    const auto json = object();
    return json.value("ReadTimeout").toInt();
}

} // end: namespace http
} // end: namespace bd

/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "export.h"

#include <QJsonDocument>

namespace bd
{
namespace http
{

class HTTP_API HttpConfiguration : public QJsonDocument
{
public:
    HttpConfiguration() = default;
    HttpConfiguration(const QJsonObject& jsonObject);
    virtual ~HttpConfiguration() = default;

    int getCleanupInterval() const;
    QString getHost() const;
    int getMaxMultiPartSize() const;
    int getMaxParallelConnections() const;
    int getMaxRequestSize() const;
    int getMinParallelConnections() const;
    quint16 getPort() const;
    int getReadTimeout() const;
};

} // end: namespace http
} // end: namespace bd

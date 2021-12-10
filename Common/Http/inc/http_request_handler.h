/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "http_listener.h"
#include "http_request.h"
#include "http_response.h"

namespace bd
{
namespace http
{

class HTTP_API HttpRequestHandler
{
public:
    HttpRequestHandler(const HttpListener& httpListener);

    virtual void process(const std::shared_ptr<HttpRequest>& httpRequest,
                         const HttpResponse& httpResponse) = 0;

private:
    const HttpListener& m_httpListener;
};

} // end: namespace http
} // end: namespace bd

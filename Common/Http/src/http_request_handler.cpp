/**
  @file
  @author Alexander Wittrock
*/
#include "http_request_handler.h"

namespace bd
{
namespace http
{

///////////////////////////////////////////////////////////////////////////////
HttpRequestHandler::HttpRequestHandler(const HttpListener& httpListener) :
    m_httpListener(httpListener)
{
}

} // end: namespace http
} // end: namespace bd

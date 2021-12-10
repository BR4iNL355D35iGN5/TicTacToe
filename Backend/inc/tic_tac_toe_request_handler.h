/**
  @file
  @author Alexander Wittrock
*/
#pragma once

#include "http_listener.h"
#include "http_request_handler.h"

namespace bd
{
namespace tictactoe
{
namespace backend
{

class TicTacToeRequestHandler : public bd::http::HttpRequestHandler
{
public:
    TicTacToeRequestHandler(const bd::http::HttpListener& httpListener);

    virtual void process(const std::shared_ptr<bd::http::HttpRequest>& httpRequest,
                         const bd::http::HttpResponse& httpResponse) override;
};

} // end: namespace backend
} // end: namespace tictactoe
} // end: namespace bd

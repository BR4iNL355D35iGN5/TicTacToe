/**
  @file
  @author Alexander Wittrock
*/
#include "tic_tac_toe_request_handler.h"

namespace bd
{
namespace tictactoe
{
namespace backend
{

using namespace bd::http;

///////////////////////////////////////////////////////////////////////////////
TicTacToeRequestHandler::TicTacToeRequestHandler(const HttpListener& httpListener) :
    HttpRequestHandler(httpListener)
{
}

///////////////////////////////////////////////////////////////////////////////
void TicTacToeRequestHandler::process(const std::shared_ptr<HttpRequest>& httpRequest,
                                      const HttpResponse& httpResponse)
{

}

} // end: namespace backend
} // end: namespace tictactoe
} // end: namespace bd

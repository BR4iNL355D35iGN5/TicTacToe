/**
  @file
  @author Alexander Wittrock
*/

#include "http_listener.h"

#include <QCoreApplication>

using namespace bd::http;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);
    app.setApplicationName("TicTacToeServer");

 #if 0
    ///\todo Fix this call
    HttpListener httpListener;
    if(!httpListener.startListening())
    {
        return 1;
    }
 #endif

    qWarning("TicTacToeServer has started");
    app.exec();
    qWarning("TicTacToeServer has stopped");

    return 0;
}

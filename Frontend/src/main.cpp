#include <QCoreApplication>
#include "httprequest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpRequest request;
    request.send("http://127.0.0.1:8080");

    return a.exec();
}

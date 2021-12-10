#include "http_listener.h"
#include "http_request_handler.h"

#include <QtTest/QtTest>

using namespace bd::http;

class CustomHttpRequestHandler : public HttpRequestHandler
{
public:
    CustomHttpRequestHandler(const HttpListener& httpListener) :
        HttpRequestHandler(httpListener)
    {
    }

    virtual void process(const std::shared_ptr<HttpRequest>& httpRequest,
                         const HttpResponse& httpResponse) override
    {
    }
};

class TestHttpListener: public QObject
{
    Q_OBJECT

private slots:
    void listening();    
    void checkConfiguration();

private:
    HttpListener m_httpListener;
};

///////////////////////////////////////////////////////////////////////////////
void TestHttpListener::listening()
{
    QVERIFY(!m_httpListener.isListening());
    const std::shared_ptr<CustomHttpRequestHandler> customRequestHandler =
        std::make_shared<CustomHttpRequestHandler>(m_httpListener);
    QVERIFY(m_httpListener.startListening(customRequestHandler));
    QVERIFY(m_httpListener.isListening());
}

///////////////////////////////////////////////////////////////////////////////
void TestHttpListener::checkConfiguration()
{
    QCOMPARE(m_httpListener.getConfiguration().getHost(), "127.0.0.1");
    QCOMPARE(m_httpListener.getConfiguration().getPort(), 8080);
}

QTEST_MAIN(TestHttpListener)
#include "test_http_listener.moc"

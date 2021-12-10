/**
  @file
  @author Alexander Wittrock
*/
#include "http_connection_handler_pool.h"
#include "http_connection_handler.h"

namespace bd
{
namespace http
{

///////////////////////////////////////////////////////////////////////////////
HttpConnectionHandlerPool::HttpConnectionHandlerPool(const HttpListener& httpListener) :
    QObject(),
    m_connectionHandlers(),
    m_httpListener(httpListener),
    m_mutex(),
    m_timer()
{
    m_timer.start(m_httpListener.getConfiguration().getCleanupInterval());
    connect(&m_timer, SIGNAL(timeout()), SLOT(cleanup()));
}

///////////////////////////////////////////////////////////////////////////////
HttpConnectionHandlerPool::~HttpConnectionHandlerPool()
{
    clear();
    qDebug("HttpConnectionHandlerPool (%p): destroyed", this);
}

///////////////////////////////////////////////////////////////////////////////
void HttpConnectionHandlerPool::cleanup()
{
    const auto maxIdleHandlers =
        m_httpListener.getConfiguration().getMinParallelConnections();
    int idleCounter = 0;

    QMutexLocker locker(&m_mutex);

    for(const auto& handler : m_connectionHandlers)
    {
        if(handler->isBusy())
        {
            continue;
        }

        if(++idleCounter > maxIdleHandlers)
        {
            m_connectionHandlers.removeOne(handler);
            long int poolSize=static_cast<long int>(m_connectionHandlers.size());
            qDebug("HttpConnectionHandlerPool: Removed connection handler (%p),"
                   "pool size is now %li", handler.get(), poolSize);
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void HttpConnectionHandlerPool::clear()
{
    m_mutex.lock();
    m_connectionHandlers.clear();
    m_mutex.unlock();
}

///////////////////////////////////////////////////////////////////////////////
std::shared_ptr<HttpConnectionHandler> HttpConnectionHandlerPool::getConnectionHandler()
{
    std::shared_ptr<HttpConnectionHandler> freeConnectionHandler(nullptr);

    QMutexLocker locker(&m_mutex);

    for(auto handler : m_connectionHandlers)
    {
        if(!handler->isBusy())
        {
            freeConnectionHandler = handler;
            freeConnectionHandler->setBusy();
            break;
        }
    }

    if(!freeConnectionHandler)
    {
        const auto maxConnectionHandlers =
            m_httpListener.getConfiguration().getMaxParallelConnections();
        if(m_connectionHandlers.count() < maxConnectionHandlers)
        {
            freeConnectionHandler =
                std::make_shared<HttpConnectionHandler>(m_httpListener);
            freeConnectionHandler->setBusy();
            m_connectionHandlers.append(freeConnectionHandler);
        }
    }

    return freeConnectionHandler;
}

} // end: namespace http
} // end: namespace bd

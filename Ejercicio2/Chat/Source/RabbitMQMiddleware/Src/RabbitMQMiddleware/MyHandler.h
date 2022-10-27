#ifndef MY_HANDLER_H
#define MY_HANDLER_H
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>

#include "Interface/IConnectionManager.h"

class MyHandler: public AMQP::LibEvHandler {
private:
	IConnectionManager* connectionManager;

    virtual void onAttached(AMQP::TcpConnection *connection) override;
    virtual void onConnected(AMQP::TcpConnection *connection) override;
    virtual bool onSecured(AMQP::TcpConnection *connection, const SSL *ssl) override;
    virtual void onReady(AMQP::TcpConnection *connection) override;
    virtual void onError(AMQP::TcpConnection *connection, const char *message) override;
    virtual void onClosed(AMQP::TcpConnection *connection) override;
    virtual void onLost(AMQP::TcpConnection *connection) override;
    virtual void onDetached(AMQP::TcpConnection *connection) override;

public:
        MyHandler(struct ev_loop *loop, IConnectionManager* connectionManager);
};
#endif

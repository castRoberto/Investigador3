#include "MyHandler.h"

MyHandler::MyHandler(struct ev_loop *loop, IConnectionManager* connectionManager) : AMQP::LibEvHandler(loop) {
	this->connectionManager = connectionManager;
}

void MyHandler::onAttached(AMQP::TcpConnection *connection) {
//	std::cout << "[MyHandler.onAttached]" << std::endl;
}

void MyHandler::onConnected(AMQP::TcpConnection *connection) {
//	std::cout << "[MyHandler.onConnected]" << std::endl;
}

bool MyHandler::onSecured(AMQP::TcpConnection *connection, const SSL *ssl) {
        return true;
}

void MyHandler::onReady(AMQP::TcpConnection *connection) {
//        std::cout<<"ready"<<std::endl;
}

void MyHandler::onError(AMQP::TcpConnection *connection, const char *message) {
	std::cout<<"[MyHandler.onError]: connection Error"<<std::endl;
	std::cout << message << std::endl;
	connectionManager->manageConnection();
	connectionManager->restartEntities();
}

void MyHandler::onClosed(AMQP::TcpConnection *connection) {
	std::cout << "[MyHandler.onClosed]" << std::endl;
}

void MyHandler::onLost(AMQP::TcpConnection *connection) {
//	std::cout<<"[MyHandler.onLost]: Connection Lost"<<std::endl;
}

void MyHandler::onDetached(AMQP::TcpConnection *connection) {
//	std::cout << "[MyHandler.onDetached]" << std::endl;
}


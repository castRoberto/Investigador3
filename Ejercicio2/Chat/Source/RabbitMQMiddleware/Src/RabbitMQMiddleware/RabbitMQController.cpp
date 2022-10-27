/*
 * RabbitMQController.cpp
 *
 *  Created on: 13/06/2022
 *      Author: rcastro
 */

#include "RabbitMQController.h"
// pthread_testcancel  pthread_join
RabbitMQController::RabbitMQController() {}

RabbitMQController::~RabbitMQController() {
	if (rabbitThread != 0) {
		rabbitThread->terminate();
		try {
			rabbitThread->join();
		} catch (posix::PosixError e) {
			LOG("[RabbitMQController]: falla al cerrar rabbitThread");
			std::cout << ": " << e.what() << std::endl;
		}
		delete rabbitThread;
		rabbitThread = 0;
	}

	if (connection != 0) {
		connection->closed();
		delete connection;
		connection = 0;
	}

	if (channel != 0) {
		delete channel;
		channel = 0;
	}

	if (address != 0) {
		delete address;
		address = 0;
	}

	if (myHandler != 0) {
		delete myHandler;
		myHandler = 0;
	}
}

RabbitMQController& RabbitMQController::getInstance() {
	static RabbitMQController instance;

	return instance;
}

void RabbitMQController::addRabbitEntity(Entity* rabbitEntity) {
	this->rabbitMQEntities.push_back(rabbitEntity);
}

void RabbitMQController::configure() {
	started = false;
	configured = false;


	LOG("[RabbitMQController.configure]: Controlador de rabbit configurado");

#ifdef DEBUG
	LOG("[RabbitMQController.configure]: Controladcor de rabbit configurado");
#endif

	this->url = "amqp://" + this->username + ":" + this->password + "@"
			+ this->host + "/" + this->vhost;

	loop = ev_loop_new(0);
	address = new AMQP::Address(url);
//	myHandler = new MyHandler(loop, this);
//	connection = new AMQP::TcpConnection(myHandler, *address);
//	channel = new AMQP::TcpChannel(connection);
//
	rabbitThread = new posix::EndlessThread(this);
	manageConnection();
	configured = true;
}

void RabbitMQController::manageConnection() {
//	LOG("[RabbitMQController.manageConnection]: Se administra la conexion");

	if(channel != 0) {
		channel->close();
		delete channel;
		channel = 0;
	}

	if (connection != 0) {
		connection->close();
		delete connection;
		connection = 0;
	}

	if(myHandler != 0) {
		delete myHandler;
		myHandler = 0;
	}
	myHandler = new MyHandler(loop, this);
	connection = new AMQP::TcpConnection(myHandler, *address);
	channel = new AMQP::TcpChannel(connection);
}

void RabbitMQController::restartEntities() {
//	LOG("[RabbitMQController.restartEntities]: Se reinician las entidades");
	for(Entity* entity : rabbitMQEntities) {
		entity->init();
	}
}

void RabbitMQController::configure(std::string username, std::string password,
		std::string host, std::string vhost) {

	this->username = username;
	this->password = password;
	this->host = host;
	this->vhost = vhost;

	configure();
}

void RabbitMQController::configure(json config) {
	this->username = config[loginKey][usernameKey];
	this->password = config[loginKey][passwordKey];
	this->host = config[loginKey][hostKey];
	this->vhost = config[loginKey][vhostKey];

	configure();
}

void RabbitMQController::reConfigure() {

}

void RabbitMQController::start() {
	started = true;
	rabbitThread->start();
}

AMQP::TcpChannel* RabbitMQController::getChannel() {
	return channel;
}

void* RabbitMQController::run(void *args) {
	ev_run(loop);

	return 0;
}

AMQP::ExchangeType RabbitMQController::eExchangeTypeStringtoEnum(std::string type) {
	AMQP::ExchangeType retEnum = AMQP::ExchangeType::topic;
	std::string typeStr = "eExchangeType_";
	typeStr.append(type);

	if (typeStr.compare("eExchangeType_topic") == 0) {
		retEnum = AMQP::ExchangeType::topic;
	}
	else if (typeStr.compare("eExchangeType_fanout") == 0) {
		retEnum = AMQP::ExchangeType::fanout;
	}
	else if (typeStr.compare("eExchangeType_header") == 0) {
		retEnum = AMQP::ExchangeType::headers;
	}
	else if (typeStr.compare("eExchangeType_direct") == 0) {
		retEnum = AMQP::ExchangeType::direct;
	}
	return retEnum;
}


/*
 * ComunicationManager.cpp
 *
 *  Created on: 31/07/2022
 *      Author: operador
 */

#include "ComunicationManager.h"

ComunicationManager::ComunicationManager(json configuration, IComunicationManagerListener* listener) :
		producer(0), consumer(0)
{
	this->configuration = configuration;
	this->listener = listener;
}

ComunicationManager::~ComunicationManager() {

}

bool ComunicationManager::init() {
	producer = new Producer(keyProducer, configuration);
	producer->init();
	consumer = new Consumer(keyConsumer, configuration, this);
	consumer->init();

	return 0;
}

void ComunicationManager::notifyDataRabbitMQ(json data, std::string exchange, std::string routingKey) {
	std::string m = data.dump();
	listener->handleMessage(data, routingKey);
}

void ComunicationManager::sendMessage(json message, const std::string& routingKey) {
	std::string messageStr = message.dump();
	producer->publish(messageStr, routingKey);
}


/*
 * ComunicationManager.h
 *
 *  Created on: 31/07/2022
 *      Author: operador
 */

#ifndef SRC_COMUNICATION_COMUNICATIONMANAGER_H_
#define SRC_COMUNICATION_COMUNICATIONMANAGER_H_

#include <string>

#include <RabbitMQMiddleware/Consumer.h>
#include <RabbitMQMiddleware/Producer.h>
#include <RabbitMQMiddleware/Interface/RabbitMQListener.h>
#include <UtilJson/UtilJson.h>

#include "Interface/IComunicationManagerListener.h"

class ComunicationManager : public RabbitMQListener {
private:
	std::string keyProducer = "Producer";
	std::string keyConsumer = "Consumer";
	Producer* producer;
	Consumer* consumer;
	json configuration;

	IComunicationManagerListener* listener;

public:
	ComunicationManager(json configuration, IComunicationManagerListener* listener);
	virtual ~ComunicationManager();

	void notifyDataRabbitMQ(json data, std::string exchange, std::string routingKey) override;

	bool init();

	void sendMessage(json message, const std::string& routingKey);
};

#endif /* SRC_COMUNICATION_COMUNICATIONMANAGER_H_ */

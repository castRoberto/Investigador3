/*
 * Consumer.h
 *
 *  Created on: 13/06/2022
 *      Author: rcastro
 */

#ifndef CONSUMER_H_
#define CONSUMER_H_

#include <list>
#include <functional>
#include <iostream>
#include <vector>

#include "Interface/RabbitMQListener.h"
#include "RabbitMQEntity.h"
#include "RabbitMQController.h"

class Consumer : public RabbitMQEntity {
private :
	std::string nameQueue;
	std::list<std::string> routingkeys;
	RabbitMQListener* listener;

	bool qDurable = false;
	bool qAutoDelete = false;
	bool qPassive = false;
	bool qExclusive = true;

public:
	Consumer(std::string& consumerName,
			std::string& username,
			std::string& password,
			std::string& host,
			std::string& vhost,
			std::string &exchange,
			std::list<std::string>& routingkeys,
			RabbitMQListener* listener,
			std::string exchangeType);

	Consumer(std::string consumerName, json config ,RabbitMQListener* listener);

	virtual ~Consumer();
	void init() override;

};

#endif /* CONSUMER_H_ */

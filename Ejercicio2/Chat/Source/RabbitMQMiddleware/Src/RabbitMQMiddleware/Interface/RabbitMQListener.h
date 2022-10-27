/*
 * RabbitMQListener.h
 *
 *  Created on: 15/06/2022
 *      Author: rcastro
 */

#ifndef RABBITMQMIDDLEWARE_LISTENERS_RABBITMQLISTENER_H_
#define RABBITMQMIDDLEWARE_LISTENERS_RABBITMQLISTENER_H_

#include <iostream>
#include <string>

#include <UtilJson/UtilJson.h>

class RabbitMQListener {
public:
	RabbitMQListener() {}
	virtual ~RabbitMQListener() {};

	virtual void notifyDataRabbitMQ(json data, std::string exchange, std::string routingKey) = 0;
};



#endif /* RABBITMQMIDDLEWARE_LISTENERS_RABBITMQLISTENER_H_ */

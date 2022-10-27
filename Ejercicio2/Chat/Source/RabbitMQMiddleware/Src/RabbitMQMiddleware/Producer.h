/*
 * Producer.h
 *
 *  Created on: 13/06/2022
 *      Author: rcastro
 */

#ifndef PRODUCER_H_
#define PRODUCER_H_

#include <global/common/global/Runnable.h>
#include <global/common/sys/sync/Monitor.h>

#include "RabbitMQEntity.h"
#include "RabbitMQController.h"

struct queueMSG {
	std::string message;
	std::string routingKey;
};

class Producer : public RabbitMQEntity, public Runnable {
private:
	posix::Thread* producerThread = 0;

	std::list<queueMSG> queueMessages;
	/// @brief monitor para controlar el trabajo de la hebra
	posix::Monitor queueMsgToSend;
	/// @brief monitor para controlar el trabajo de la hebra
	posix::Monitor notEmptyMsgToSend;

public:
	Producer(std::string& producerName,
			std::string& username,
			std::string& password,
			std::string& host,
			std::string& vhost,
			std::string& exchange,
			std::string& exchangeType);
	Producer(std::string& producerName, json config);
	virtual ~Producer();

	void init() override;
	void publish(const std::string& msg, const std::string& routingKey);

	void sendData();

	void* run(void* args = 0);

private:
	void destroyThread();
};

#endif /* PRODUCER_H_ */

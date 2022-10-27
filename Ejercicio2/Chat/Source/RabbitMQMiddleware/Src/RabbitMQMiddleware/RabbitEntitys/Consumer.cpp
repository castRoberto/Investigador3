/*
 * Consumer.cpp
 *
 *  Created on: 13/06/2022
 *      Author: rcastro
 */

#include "../Consumer.h"

Consumer::Consumer(
		std::string& consumerName,
		std::string& username,
		std::string& password,
		std::string& host,
		std::string& vhost,
		std::string& exchange,
		std::list<std::string>& routingkeys,
		RabbitMQListener* listener,
		std::string exchangeType) :
	RabbitMQEntity(consumerName, username, password, host, vhost, exchange, exchangeType),
	routingkeys(routingkeys),
	listener(listener)
{
	activateRabbit();
}

Consumer::Consumer(std::string consumerName, json config, RabbitMQListener* listener) :
	RabbitMQEntity(consumerName, config),
	listener(listener)
{
	json queueConf = config[entityKey][jKeys::queueKey];
	this->qDurable = queueConf[jKeys::queueDurableKey];
	this->qAutoDelete = queueConf[jKeys::queueAutoDeleteKey];
	this->qPassive = queueConf[jKeys::queuePassiveKey];
	this->qExclusive = queueConf[jKeys::queueExclusiveKey];

	std::list<std::string> rks = config[entityKey][jKeys::exchangeKey][jKeys::exchangeRoutingKeysKey];

	for(std::string rk : rks) {
		this->routingkeys.push_back(rk);
	}

	activateRabbit();
}

Consumer::~Consumer() {

}


void Consumer::init() {
//	LOG("**************************[Consumer.init]: Se configura un consumer");
	int qflags = this->qExclusive? AMQP::exclusive: 0;

//	std::this_thread::sleep_for(std::chrono::milliseconds(20000));

//	rabbitController.getChannel()->removeQueue(nameQueue);
	nameQueue = "";
	rabbitController.getChannel()->declareQueue(qflags).onSuccess([this](const std::string &nameQueue, uint32_t messageCount,uint32_t consumercount) {});

	int eflags = this->eDurable? AMQP::durable : 0;
	rabbitController.getChannel()->declareExchange(exchange, exchangeType, eflags).onSuccess([this]() {
		std::list<std::string>::iterator it;
		for (it = routingkeys.begin(); it != routingkeys.end(); ++it) {
			rabbitController.getChannel()->bindQueue(exchange, nameQueue, *it);
		}
	});

	auto callBackData = [this](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
		char msg_receive[message.bodySize() + 1];
		strncpy(msg_receive, message.body(), message.bodySize() + 1);
		msg_receive[message.bodySize()] = '\0';  //extract message from broker and converts it into string
		std::string msg_final = std::string(msg_receive);

		listener->notifyDataRabbitMQ(json::parse(msg_final), exchange, message.routingkey());
		rabbitController.getChannel()->ack(deliveryTag);
	};

	auto callBackSuccess = [](const std::string& consumertag) {
		LOG(consumertag);
	};

	auto callBackError = [](const char *message) {
		LOG(message);
		LOG("[callBackError]: Se produjo un error en CallBackError error");
	};

	rabbitController.getChannel()->consume(nameQueue).onReceived(callBackData).onSuccess(callBackSuccess).onError(callBackError);
}


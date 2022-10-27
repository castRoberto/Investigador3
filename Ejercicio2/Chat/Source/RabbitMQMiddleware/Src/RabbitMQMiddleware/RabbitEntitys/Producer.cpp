/*
 * Producer.cpp
 *
 *  Created on: 13/06/2022
 *      Author: rcastro
 */

#include "../Producer.h"

Producer::Producer(std::string& producerName,
		std::string& username,
		std::string& password,
		std::string& host,
		std::string& vhost,
		std::string& exchange,
		std::string& exchangeType) :
	RabbitMQEntity(producerName, username, password, host, vhost, exchange, exchangeType)
{
	activateRabbit();
	producerThread = new posix::EndlessThread(this);
	producerThread->start();
}

Producer::Producer(std::string& producerName, json config) :
	RabbitMQEntity(producerName, config)
{
	activateRabbit();
	producerThread = new posix::EndlessThread(this);
	producerThread->start();
}

Producer::~Producer() {
	destroyThread();
}

void Producer::destroyThread() {
	if (producerThread != 0) {
		producerThread->terminate();
		try {
			producerThread->join();
		} catch (posix::PosixError e) {
			LOG("[RabbitMQController]: falla al cerrar rabbitThread");
			std::cout << ": " << e.what() << std::endl;
		}
		delete producerThread;
		producerThread = 0;
	}
}

void Producer::init() {
//	LOG("[Producer.init]");
	queueMessages.clear();
	int flags = this->eDurable? AMQP::durable : 0;
	rabbitController.getChannel()->declareExchange(exchange, exchangeType, flags).onSuccess([this]() {});
}

void Producer::sendData() {
	AMQP::TcpChannel* channel = rabbitController.getChannel();
	if(channel != NULL) {
		queueMSG data;
		queueMsgToSend.lock();
		data = queueMessages.front();
		queueMessages.pop_front();
		queueMsgToSend.unlock();

		rabbitController.getChannel()->publish(exchange, data.routingKey, data.message.c_str());
	}
}

void* Producer::run(void* args) {
	queueMSG data;
	if (queueMessages.empty() == true) {
		notEmptyMsgToSend.lock();
		notEmptyMsgToSend.wait();
		notEmptyMsgToSend.unlock();
	}
	if (queueMessages.empty() == false) {
		sendData();
		posix::Thread::sleep(5000);
	}
	return 0;
}

void Producer::publish(const std::string& msg, const std::string& routingKey) {
	queueMSG data;
	data.message = msg;
	data.routingKey = routingKey;

	queueMsgToSend.lock();
	queueMessages.push_back(data);
	queueMsgToSend.unlock();
	notEmptyMsgToSend.signal();
}


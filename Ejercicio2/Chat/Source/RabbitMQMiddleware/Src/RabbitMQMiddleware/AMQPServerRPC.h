/*
 * Producer.h
 *
 *  Created on: 13/06/2022
 *      Author: rcastro
 */

#ifndef AMQP_SERVER_RPC_
#define AMQP_SERVER_RPC_

#include <iostream>
#include <map>

#include "RabbitMQEntity.h"
#include "RabbitMQController.h"
#include "Interface/IProcessRequest.h"
#include "Interface/IRequestHandler.h"

class AMQPServerRPC : public RabbitMQEntity {
private:
	static const int prefechCoutn = 1;
	std::string rpcQueue = "rpc_queue";
	IRequestHandler* requestHandler;

public:
	AMQPServerRPC(std::string& serverName,
			std::string& username,
			std::string& password,
			std::string& host,
			std::string& vhost);

	AMQPServerRPC(std::string serverName, json config);

	virtual ~AMQPServerRPC();

	void setRequestHandler(IRequestHandler* requestHandler);
	void init() override;
};

#endif /* AMQP_SERVER_RPC_ */

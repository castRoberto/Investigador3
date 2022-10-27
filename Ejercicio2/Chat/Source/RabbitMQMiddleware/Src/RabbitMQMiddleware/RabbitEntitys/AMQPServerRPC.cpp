#include "../AMQPServerRPC.h"

AMQPServerRPC::AMQPServerRPC(std::string& serverName,
		std::string& username,
		std::string& password,
		std::string& host,
		std::string& vhost) :
	RabbitMQEntity(serverName, username, password, host, vhost)
{
	activateRabbit();
}

AMQPServerRPC::AMQPServerRPC(std::string serverName, json config) :
	RabbitMQEntity(serverName, config)
{
	activateRabbit();
}

AMQPServerRPC::~AMQPServerRPC() {

}

void AMQPServerRPC::setRequestHandler(IRequestHandler* requestHandler) {
//	this->requestProcessors[typeRequest] = processRequest;
	this->requestHandler = requestHandler;
}

void AMQPServerRPC::init() {
#ifdef DEBUG
	LOG("[AMQPServerRPC.init]: Se inicializa el server RPC");
#endif
	auto on_request = [this](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
		char msg_receive[message.bodySize() + 1];
		strncpy(msg_receive, message.body(), message.bodySize() + 1);
		msg_receive[message.bodySize()] = '\0'; //extract message from broker and converts it into string
		std::string msg_final = std::string(msg_receive);

		LOG(msg_final);
		json request = json::parse(msg_final);
		json response = requestHandler->handleRequest(request);

		std::string responseStr = response.dump();
		LOG(responseStr);

		rabbitController.getChannel()->publish("", message.replyTo(), responseStr.c_str());
		rabbitController.getChannel()->ack(deliveryTag);
	};

	rabbitController.getChannel()->declareQueue(rpcQueue);
	rabbitController.getChannel()->setQos(prefechCoutn);
	rabbitController.getChannel()->consume(rpcQueue).onReceived(on_request);
}

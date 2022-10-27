/*
 * RabbitMQEntity.cpp
 *
 *  Created on: 17/06/2022
 *      Author: rcastro
 */

#include "../RabbitMQEntity.h"

RabbitMQEntity::RabbitMQEntity(std::string entityKey,
		std::string username,
		std::string password,
		std::string host,
		std::string vhost,
		std::string exchange,
		std::string exchangeType) :
	entityKey(entityKey),
	username(username),
	password(password),
	host(host),
	vhost(vhost),
	exchange(exchange)
{
	this->exchangeType = rabbitController.eExchangeTypeStringtoEnum(exchangeType);
	if (!rabbitController.isConfigured()) {
		rabbitController.configure(username, password, host, vhost);
	}
}

RabbitMQEntity::RabbitMQEntity(std::string entityKey, json config) : entityKey(entityKey) {
	json exchangeConf = config[entityKey][jKeys::exchangeKey];

	if(exchangeConf != nlohmann::detail::value_t::null) {
		this->eDurable = exchangeConf[jKeys::exchangeDurableKey];
		this->eAutoDelete = exchangeConf[jKeys::exchangeAutoDeleteKey];
		this->ePassive = exchangeConf[jKeys::exchangePassiveKey];
		this->eInternal = exchangeConf[jKeys::exchangeInternalKey];
		this->exchange = exchangeConf[jKeys::exchangeNameKey];
		std::string eType = exchangeConf[jKeys::exchangeTypeKey];

		this->exchangeType = rabbitController.eExchangeTypeStringtoEnum(eType);
	}

	if (!rabbitController.isConfigured()) {
		rabbitController.configure(config);
	}
}

RabbitMQEntity::~RabbitMQEntity() {

}

void RabbitMQEntity::init() {
	LOG("***************************************[RabbitMQEntity.init]");
}

void RabbitMQEntity::activateRabbit() {
	if (!rabbitController.isReady()) {
		rabbitController.start();
	}
	this->rabbitController.addRabbitEntity(this);
}

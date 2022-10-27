/*
 * Sesion.cpp
 *
 *  Created on: 31/07/2022
 *      Author: operador
 */

#include "Sesion.h"

Sesion::Sesion(std::string& sesionName, json configuration) :
		comunicationManager(0),
		reportActivityThread(0)
{
	this->startedChat = false;
	this->sesionName = sesionName;
	this->configuration = configuration;
}

/**
 * @brief Destructor de la sesion
 *
 * Se verifican objetos existentes y se destruyen
 */
Sesion::~Sesion() {
	close();
}

std::string Sesion::getSender(std::string str) {
	std::reverse(str.begin(), str.end());
	std::string sender = str.substr(0, str.find("."));
	std::reverse(sender.begin(), sender.end());

	return sender;
}

void Sesion::init() {
	comunicationManager = new ComunicationManager(configuration, this);
	comunicationManager->init();
	/// Se crea hilo para el reporte de actividad
	reportActivityThread = new posix::EndlessThread(this);
	reportActivityThread->start();
}

void Sesion::close() {
	if (reportActivityThread != 0) {
		reportActivityThread->terminate();
		delete reportActivityThread;
		reportActivityThread = 0;
	}

	json offlineMessage;
	offlineMessage[message_Keys::messageType_Key] = eTypeMsg_Status;
	offlineMessage[message_Keys::messagePayload_Key] = eOffline;
	offlineMessage[message_Keys::sesionName_Key] = sesionName;

	comunicationManager->sendMessage(offlineMessage,
			routingKeysAranda::statusRouting_Key);

	if (comunicationManager != 0) {
		delete comunicationManager;
		comunicationManager = 0;
	}
}

void Sesion::activity(json message) {
	eStatus status = (eStatus) message[message_Keys::messagePayload_Key];
	std::string name = message[message_Keys::sesionName_Key];

	if(status == eOnline) {
		if(name.compare(sesionName) != 0) {
			usersOnline.insert(name);
		}
	} else {
		usersOnline.erase(name);
	}
}

void Sesion::processMessage(json message, std::string routingKey) {
	std::string dataMsg = message[message_Keys::messagePayload_Key];
	int pos = routingKey.find(sesionName);
	if(pos == -1 && startedChat) {
		LOG("[" + getSender(routingKey) + "]: " + dataMsg);
	}
}

void Sesion::handleMessage(json message, std::string routingKey) {
	if(message != nlohmann::detail::value_t::null) {
		try {
			eTypeMSG typeMessage = (eTypeMSG) message[message_Keys::messageType_Key];

			switch (typeMessage) {
			case eTypeMsg_Status:
				activity(message);
				break;
			case eTypeMsg_Data:
				processMessage(message, routingKey);
				break;
			}
		} catch (nlohmann::detail::exception e) {
			LOG("[Sesion.handleMessage]: La estructura del mensaje no es adecuada");
		}
	}
}

void Sesion::initChat() {
	startedChat = true;
	std::string comand;

	LOG("Chat iniciado, finalice con q");
	while(startedChat) {
		std::string routinK = routingKeysAranda::dataRouting_Keys;
		routinK.append("." + sesionName);
		json data;
		data[message_Keys::messageType_Key] = eTypeMsg_Data;

		std::getline(std::cin, comand);
		if(comand.compare("q") == 0) {
			startedChat = false;
			LOG("Chat finalizado");
			break;
		}

		data[message_Keys::messagePayload_Key] = comand;
		comunicationManager->sendMessage(data, routinK);
	}
}

void* Sesion::run(void *args) {
	json onlineMessage;
	onlineMessage[message_Keys::messageType_Key] = eTypeMsg_Status;
	onlineMessage[message_Keys::messagePayload_Key] = eOnline;
	onlineMessage[message_Keys::sesionName_Key] = sesionName;

	comunicationManager->sendMessage(onlineMessage, routingKeysAranda::statusRouting_Key);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return 0;
}

std::set<std::string> Sesion::getUsersOnline() {
	return usersOnline;
}


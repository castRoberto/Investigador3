/*
 * Sesion.h
 *
 *  Created on: 31/07/2022
 *      Author: operador
 */

#ifndef SRC_SESION_H_
#define SRC_SESION_H_

#include <set>
#include <chrono>
#include <thread>
#include <ostream>

#include <UtilJson/UtilJson.h>

#include <ComunicationManager/ComunicationManager.h>
#include <ComunicationManager/Interface/IComunicationManagerListener.h>
#include <ComunicationManager/Interface/eTypeMessageAranda.h>
#include <global/common/global/Runnable.h>
#include <global/common/sys/thread/Thread.h>
#include <global/common/sys/thread/EndlessThread.h>

#include "ISesion.h"

class ComunicationManager;

class Sesion: public ISesion,
		public IComunicationManagerListener,
		public Runnable
{
	const long TIME_REPORT_ACTIVITY = 1e6;

	std::string sesionName;
	json configuration;
	ComunicationManager* comunicationManager;
	std::set<std::string> usersOnline;

	posix::Thread* reportActivityThread;
	bool startedChat;

public:
	Sesion(std::string& sesionName, json configuration);
	virtual ~Sesion();

	void init() override;
	void close() override;

	void initChat();


	void handleMessage(json data, std::string routingKey) override;
	void* run(void* args = 0) override;

	void activity(json message) override;
	void processMessage(json message, std::string routingKey) override;
	std::set<std::string> getUsersOnline() override;

private:
	std::string getSender(std::string);
};

#endif /* SRC_SESION_H_ */

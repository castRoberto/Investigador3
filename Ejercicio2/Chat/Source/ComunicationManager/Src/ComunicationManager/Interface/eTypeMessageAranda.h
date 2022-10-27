/*
 * eTypeMessage.h
 *
 *  Created on: 31/07/2022
 *      Author: operador
 */

#ifndef SRC_COMUNICATION_INTERFACE_ETYPEMESSAGEARANDA_H_
#define SRC_COMUNICATION_INTERFACE_ETYPEMESSAGEARANDA_H_

#include <string>

namespace message_Keys {
	const std::string messageType_Key = "messageType";
	const std::string messagePayload_Key = "messagePayload";
	const std::string sesionName_Key = "sesionName";
	const std::string status_Key = "status";
};


namespace routingKeysAranda {
	const std::string statusRouting_Key = "aranda.status";
	const std::string dataRouting_Keys = "aranda.data";
};

typedef enum {
	eTypeMsg_NotFound = -1,
	eTypeMsg_Status = 0,
	eTypeMsg_Data = 1
} eTypeMSG;

typedef enum {
	eOnline = 0,
	eOffline
} eStatus;


#endif /* SRC_COMUNICATION_INTERFACE_ETYPEMESSAGEARANDA_H_ */

/*
 * ProcessRequestHandler.h
 *
 *  Created on: 21/07/2022
 *      Author: operador
 */

#ifndef SRC_RABBITMQMIDDLEWARE_INTERFACE_IREQUESTHANDLER_H_
#define SRC_RABBITMQMIDDLEWARE_INTERFACE_IREQUESTHANDLER_H_

#include <UtilJson/UtilJson.h>

#include "../Interface/IProcessRequest.h"

class IRequestHandler {
public:
	IRequestHandler() {};
	virtual ~IRequestHandler() {};

	virtual json handleRequest(json request) = 0;
};

#endif /* SRC_RABBITMQMIDDLEWARE_INTERFACE_IREQUESTHANDLER_H_ */

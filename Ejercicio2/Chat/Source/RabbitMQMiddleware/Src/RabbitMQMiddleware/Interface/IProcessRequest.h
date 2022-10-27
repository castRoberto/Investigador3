/*
 * IProcessRequest.h
 *
 *  Created on: 16/07/2022
 *      Author: operador
 */

#ifndef SRC_RABBITMQMIDDLEWARE_INTERFACE_IPROCESSREQUEST_H_
#define SRC_RABBITMQMIDDLEWARE_INTERFACE_IPROCESSREQUEST_H_

#include <iostream>
#include <set>

#include <UtilJson/UtilJson.h>

class IProcessRequest {
public:
	IProcessRequest() {};
	virtual ~IProcessRequest() {};

	virtual json process(json request) = 0;
};

#endif /* SRC_RABBITMQMIDDLEWARE_INTERFACE_IPROCESSREQUEST_H_ */

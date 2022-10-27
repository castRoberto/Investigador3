/*
 * IComunicationManagerListener.h
 *
 *  Created on: 31/07/2022
 *      Author: operador
 */

#ifndef SRC_COMUNICATION_INTERFACE_ICOMUNICATIONMANAGERLISTENER_H_
#define SRC_COMUNICATION_INTERFACE_ICOMUNICATIONMANAGERLISTENER_H_

#include <UtilJson/UtilJson.h>

class IComunicationManagerListener {
public:
	IComunicationManagerListener() {};
	virtual ~IComunicationManagerListener() {};

	virtual void handleMessage(json data, std::string routingKey) = 0;
};

#endif /* SRC_COMUNICATION_INTERFACE_ICOMUNICATIONMANAGERLISTENER_H_ */

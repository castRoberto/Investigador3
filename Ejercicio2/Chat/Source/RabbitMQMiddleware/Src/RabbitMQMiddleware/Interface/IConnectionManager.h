/*
 * IConnectionManager.h
 *
 *  Created on: 4/08/2022
 *      Author: operador
 */

#ifndef SRC_RABBITMQMIDDLEWARE_INTERFACE_ICONNECTIONMANAGER_H_
#define SRC_RABBITMQMIDDLEWARE_INTERFACE_ICONNECTIONMANAGER_H_

class IConnectionManager {
public:
	IConnectionManager() {};
	virtual ~IConnectionManager() {};

	virtual void manageConnection() = 0;
	virtual void restartEntities() = 0;
};

#endif /* SRC_RABBITMQMIDDLEWARE_INTERFACE_ICONNECTIONMANAGER_H_ */

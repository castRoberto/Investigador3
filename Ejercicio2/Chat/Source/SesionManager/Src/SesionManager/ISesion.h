/*
 * ISesion.h
 *
 *  Created on: 31/07/2022
 *      Author: operador
 */

#ifndef SRC_ISESION_H_
#define SRC_ISESION_H_

#include <set>
#include <UtilJson/UtilJson.h>

class ISesion {
public:
	ISesion() {};
	virtual ~ISesion() {};

	virtual void init() = 0;
	virtual void close() = 0;

	virtual std::set<std::string> getUsersOnline() = 0;
	virtual void activity(json message) = 0;
	virtual void processMessage(json message, std::string routingKey) = 0;
	virtual void initChat() = 0;
};

#endif /* SRC_ISESION_H_ */

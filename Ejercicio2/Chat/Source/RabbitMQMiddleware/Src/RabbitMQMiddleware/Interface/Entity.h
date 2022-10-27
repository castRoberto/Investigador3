/*
 * Entity.h
 *
 *  Created on: 4/08/2022
 *      Author: operador
 */

#ifndef SRC_RABBITMQMIDDLEWARE_INTERFACE_ENTITY_H_
#define SRC_RABBITMQMIDDLEWARE_INTERFACE_ENTITY_H_

class Entity {
public:
	Entity() {};
	virtual ~Entity() {};

	virtual void init() = 0;
};

#endif /* SRC_RABBITMQMIDDLEWARE_INTERFACE_ENTITY_H_ */

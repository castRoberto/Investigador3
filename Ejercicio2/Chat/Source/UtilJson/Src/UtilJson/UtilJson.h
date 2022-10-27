/*
 * UtilJson.h
 *
 *  Created on: 16/06/2022
 *      Author: rcastro
 */

#ifndef RABBITMQMIDDLEWARE_UTIL_UTILJSON_H_
#define RABBITMQMIDDLEWARE_UTIL_UTILJSON_H_

#include <fstream>
#include <iostream>

#include "json.hpp"

typedef nlohmann::json json;

#ifndef LOG
#define LOG(x) std::cout << x << std::endl;
#endif


class UtilJson {
public:
	UtilJson();
	virtual ~UtilJson();

	static json getJson(std::string& path);
};

#endif /* RABBITMQMIDDLEWARE_UTIL_UTILJSON_H_ */

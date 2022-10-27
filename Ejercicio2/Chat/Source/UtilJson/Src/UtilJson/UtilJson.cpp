/*
 * UtilJson.cpp
 *
 *  Created on: 16/06/2022
 *      Author: rcastro
 */

#include "UtilJson.h"

UtilJson::UtilJson() {

}

UtilJson::~UtilJson() {
}

nlohmann::json UtilJson::getJson(std::string& path) {

	std::ifstream i;
	i.open(path.c_str());
	if (i.fail()) {
		LOG("Error al abrir el archivo " + path);
	}
	nlohmann::json j;
	i >> j;
	i.close();

	return j;
}

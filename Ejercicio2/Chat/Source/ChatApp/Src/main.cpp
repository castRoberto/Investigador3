/**
 * @brief Entry point for chat application
 *
 * @date 31/07/2022
 * @authors robertoca07@hotmail.com
 * @name main.cpp
 */
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <unistd.h>

#include <RabbitMQMiddleware/Consumer.h>
#include <RabbitMQMiddleware/Producer.h>
#include <UtilJson/UtilJson.h>

#include <global/common/sys/trap.h>
#include <SesionManager/Sesion.h>

#include <ViewManager/View.h>

int main(int argc, char *argv[]) {
	std::string userName = argv[1];
	std::string path;

	path = argv[0];
	int pos = path.find("ChatApp");
	if (pos == -1) {
		return 1;
	}

	path = path.substr(0, pos);
	path.append("Config/ComunicationConfig.json");

	json config = UtilJson::getJson(path);

	Sesion* sesion = new Sesion(userName, config);
	sesion->init();
	usleep(1000000);

	View* view = new View(sesion);
	view->init();

	return 0;
}



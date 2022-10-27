/*
 * ViewController.cpp
 *
 *  Created on: 1/08/2022
 *      Author: operador
 */

#include "View.h"

View::View(ISesion* sesion) : sesion(sesion) {
	exitApp = false;
}

View::~View() {
	// TODO Auto-generated destructor stub
}

void View::init() {
	while(!exitApp) {
		showMenu();
		handleComands();
	}
}

void View::showMenu() {
	LOG("    SOFTWARE MENU CHAT PROGRAM");
	LOG("--------------------------------------------------------------------------------\r\n");
	showComands();
}

void View::showComands() {
	LOG("Comandos (Ingrese uno de los siguientes caracteres y luego presione enter).");
	LOG("--------------------------------------------------------------------------------\r\n");

	LOG(" Comandos generales:");
	LOG(" -----------------------------");
	LOG(" UO   Users online");
	LOG(" IC   Iniciar chat");
	LOG("");

	LOG("Q Quit\r\n");
	std::cout << "chat >";
}

void View::handleComands() {
	std::string comand;
	std::getline(std::cin, comand);
	transform(comand.begin(), comand.end(), comand.begin(), ::tolower);

	system("clear");
	if(comand.compare("uo") == 0) {
		showUsersOnline();
		LOG(" Presione enter para volver");
		std::getline(std::cin, comand);
	} else if(comand.compare("ic") == 0) {
		sesion->initChat();
	} else if(comand.compare("q") == 0) {
		sesion->close();
		delete sesion;
		sesion = 0;
		exitApp = true;
	}
}

void View::showUsersOnline() {
	std::set<std::string> users = sesion->getUsersOnline();
	if(!users.empty()) {
		for (std::string user : users) {
			LOG(" Los usuarios en linea son:");
			LOG(" ---------------------------------");
			LOG(" " + user);
		}
	} else {
		LOG(" No hay usuarios en linea");
	}

}

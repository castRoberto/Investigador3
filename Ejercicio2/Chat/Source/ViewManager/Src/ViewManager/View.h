/*
 * ViewController.h
 *
 *  Created on: 1/08/2022
 *      Author: operador
 */

#ifndef SRC_VIEW_VIEW_H_
#define SRC_VIEW_VIEW_H_

#include <set>
#include <SesionManager/ISesion.h>

class View {
private:
	bool exitApp;
	ISesion* sesion;



public:
	View(ISesion* sesion);
	virtual ~View();

	void init();

private:
	void showMenu();
	void showComands();
	void handleComands();
	void showUsersOnline();
	void handleChat();
};

#endif /* SRC_VIEW_VIEW_H_ */

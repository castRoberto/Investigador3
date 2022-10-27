/*
 * RabbitMQController.h
 *
 *  Created on: 13/06/2022
 *      Author: rcastro
 */

#ifndef RABBITMQCONTROLLER_H_
#define RABBITMQCONTROLLER_H_

#include <chrono>
#include <thread>

#include <iostream>
#include <string>
#include <functional>
#include <pthread.h>
#include <list>
#include <global/common/global/Runnable.h>
#include <global/common/sys/thread/Thread.h>
#include <global/common/sys/thread/EndlessThread.h>
#include <global/common/sys/PosixError.h>


#include "MyHandler.h"
#include "Interface/IConnectionManager.h"
#include "Interface/Entity.h"
#include <UtilJson/UtilJson.h>

#ifndef LOG
#define LOG(x) std::cout << x << std::endl;
#endif

typedef nlohmann::json json;

//class RabbitMQEntity;

/**
 * @brief Clase para gestionar la conexion al broker de rabbit
 */
class RabbitMQController : public IConnectionManager, public Runnable {
private:
	std::string url;
	std::string username;
	std::string password;
	std::string host;
	std::string vhost;

	struct ev_loop* loop = 0; /// loop de eventos necesario para gestiona conexion con broker rabbit
	MyHandler* myHandler = 0;
	AMQP::Address* address = 0;
	AMQP::TcpConnection* connection = 0;
	AMQP::TcpChannel* channel = 0;

	posix::Thread* rabbitThread = 0;

	std::list<Entity*> rabbitMQEntities;

	bool started = false;
	bool configured = false;

	const std::string loginKey 		= 	"login";
	const std::string usernameKey 	= 	"username";
	const std::string passwordKey 	= 	"password";
	const std::string hostKey 		= 	"host";
	const std::string vhostKey 		= 	"vhost";


private:
	RabbitMQController();
	void configure();

public:
	RabbitMQController(const RabbitMQController&) = delete;
	virtual ~RabbitMQController();

	/**
	 * @brief Metodo para crear una instancia unica de un controldador Rabbit
	 *
	 * @return RabbitMQController
	 */
	static RabbitMQController& getInstance();

	/**
	 * @brief Establece parametros de conexion a partir de un archivo de configuracion
	 * en formato json
	 *
	 * @param config Parametros de configuracion
	 */
	void configure(json config);

	void addRabbitEntity(Entity* rabbitEntity);

	/**
	 * @brief Establece parametros de conexion a partir de un conjunto de valores
	 * prestablecidos
	 *
	 *
	 * @param username Usuario registrado en el broker de rabbit
	 * @param password Contrasena del usuario
	 * @param host Direccion ip del broker
	 * @param vhost Parametro para segmentar exchange
	 */
	void configure(std::string username, std::string password, std::string host, std::string vhost);
	void reConfigure();

	/**
	 * @brief Metodo para iniciar la hebra que gestiona el loop de eventos
	 */
	void start();

	/**
	 * @brief Metodo para construir una conexion con el broker
	 */
	void manageConnection() override;

	void restartEntities() override;

	/**
	 * @brief Metodo para devolver la instancia unica del canal
	 *
	 * @return channel
	 */
	AMQP::TcpChannel* getChannel();

	/**
	 * @brief Metodo para convertir de string a enumerado que representa los tipos de exchange
	 *
	 * @param type String de tipo de exchange
	 * @return ExchangeType Enumerado de tipo de exchange
	 */
	AMQP::ExchangeType eExchangeTypeStringtoEnum(std::string type);

	/**
	 * @brief Metodo para consultar si la hebra del loop de eventos esta iniciada
	 *
	 * @return bool Estado de la hebra
	 */
	inline bool isReady() { return started; }

	/**
	 * @brief Metodo para verificar si el controlador de conexiones esta configurado
	 *
	 * @return bool Estado de configuracion
	 */
	inline bool isConfigured() { return configured; }

	/**
	 * @brief Actualiza el loop de eventos
	 *
	 * @param args
	 */
	void* run(void* args) override;
};

#endif /* RABBITMQCONTROLLER_H_ */

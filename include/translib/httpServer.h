/*
 * httpServer.h
 *
 *  Created on: 2015年7月7日
 *      Author: 
 */

#ifndef INCLUDE_EC_HTTPSERVER_H_
#define INCLUDE_EC_HTTPSERVER_H_

#include <string>
#include <map>
#include "translib/loop.h"
#include "translib/httpRequest.h"

namespace translib
{

typedef std::function<void (translib::HttpRequest *request)> HttpHandler;

class HttpServer : public translib::Loop
{
public:
	HttpServer();
	virtual ~HttpServer();

	bool listen(const char *ip, uint16_t port);

	void regHandler(const char * path, translib::HttpHandler handler);

private:
	static void genericCallback(struct evhttp_request *req, void *arg);
	static void mainCallback(struct evhttp_request *req, void *arg);

private:
	std::string _ip;
	uint16_t _port;

	struct evhttp *_http;

	std::map<std::string, translib::HttpHandler> _handlers;
};

} /* namespace translib */

#endif /* INCLUDE_EC_HTTPSERVER_H_ */

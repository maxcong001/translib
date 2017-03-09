/*
 * httpExample.cpp
 *
 *  Created on: 2015年7月7日
 *      Author: 
 */

#include <iostream>
#include <functional>
using namespace std;

#include "translib/httpServer.h"
#include "index.h"

class ExampleHttpServer : public translib::HttpServer
{
public:
	void reg()
	{
		regHandler("/", std::bind(&ExampleHttpServer::doMainRequest, this, placeholders::_1));
		regHandler("/test", std::bind(&ExampleHttpServer::doTestRequest, this, placeholders::_1));
	}

private:
	void doMainRequest(translib::HttpRequest *request)
	{
		request->setBody("main page");
	}

	void doTestRequest(translib::HttpRequest *request)
	{
		request->setBody("test page");
	}

};

void httpExample()
{
	ExampleHttpServer server;
	if (!server.listen("127.0.0.1", 4567))
	{
		cout << "listen failed" << endl;
	}

	server.reg();

	if (!server.start())
	{
		cout << "start failed" << endl;
	}

	server.wait();
}

/*
 * Copyright (c) 2016-20017 Max Cong <savagecm@qq.com>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

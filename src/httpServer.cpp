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

#include "translib/httpServer.h"
#include <string.h>
#include <vector>

namespace translib
{

const char *CONNECTION_KEY = "Connection";
const char *CONNECTION_VALUE = "keep-alive";

HttpServer::HttpServer() : _port(0),
						   _http(NULL)
{
}

HttpServer::~HttpServer()
{
	if (NULL != _http)
	{
		evhttp_free(_http);
	}
}

bool HttpServer::listen(const char *ip, uint16_t port)
{
	if (NULL != _http)
	{
		return false;
	}

	_http = evhttp_new(ev());
	if (NULL == _http)
	{
		return false;
	}

	if (0 != evhttp_bind_socket(_http, ip, port))
	{
		evhttp_free(_http);
		_http = NULL;
		return false;
	}

	_ip = ip;
	_port = port;
	evhttp_set_gencb(_http, genericCallback, this);
	return true;
}

void HttpServer::regHandler(const char *path, translib::HttpHandler handler)
{
	if (NULL == _http)
	{
		return;
	}

	_handlers[path] = handler;
	evhttp_set_cb(_http, path, mainCallback, &_handlers[path]);
}

void HttpServer::genericCallback(struct evhttp_request *req, void *arg)
{
	evhttp_send_reply_start(req, HTTP_NOTFOUND, "not found");
	evhttp_send_reply_end(req);
}

void HttpServer::mainCallback(struct evhttp_request *req, void *arg)
{
	HttpHandler *handler = static_cast<HttpHandler *>(arg);
	translib::HttpRequest *request = new HttpRequest(req);
	(*handler)(request);

	const char *value = request->findHeaders(CONNECTION_KEY);
	if (value != NULL && strncmp(CONNECTION_VALUE, value, strlen(CONNECTION_VALUE)) == 0)
	{
		evhttp_add_header(req->output_headers, CONNECTION_KEY, CONNECTION_VALUE);
	}

	evhttp_send_reply_start(req, HTTP_OK, "OK");
	evhttp_send_reply_chunk(req, request->_buffer);
	evhttp_send_reply_end(req);

	delete request;
}

} /* namespace translib */

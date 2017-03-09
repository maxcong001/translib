/*
 * httpServer.cpp
 *
 *  Created on: 2015年7月7日
 *      Author: 
 */

#include "translib/httpServer.h"
#include <string.h>
#include <vector>

namespace translib
{

const char * CONNECTION_KEY = "Connection";
const char * CONNECTION_VALUE = "keep-alive";

HttpServer::HttpServer() :
		_port(0),
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


void HttpServer::regHandler(const char * path, translib::HttpHandler handler)
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

	const char * value = request->findHeaders(CONNECTION_KEY);
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



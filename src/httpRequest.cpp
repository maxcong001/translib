/*
 * httpRequest.cpp
 *
 *  Created on: 2015年7月7日
 *      Author: 
 */

#include "translib/httpRequest.h"

namespace translib
{

HttpRequest::HttpRequest(evhttp_request * req) :
		_req(req)
{
	_buffer = evbuffer_new();
}

HttpRequest::~HttpRequest()
{
	evbuffer_free(_buffer);
}

const char * HttpRequest::getUri()
{
	return evhttp_request_get_uri(_req);
}

const char * HttpRequest::getPath()
{
	return evhttp_uri_get_path(evhttp_request_get_evhttp_uri(_req));
}

const char * HttpRequest::getHost()
{
	return evhttp_uri_get_host(evhttp_request_get_evhttp_uri(_req));
}

uint16_t HttpRequest::getPort()
{
	return evhttp_uri_get_port(evhttp_request_get_evhttp_uri(_req));
}

evhttp_cmd_type HttpRequest::getCommand()
{
	return evhttp_request_get_command(_req);
}

const char * HttpRequest::findHeaders(const char * key)
{
	return evhttp_find_header(evhttp_request_get_input_headers(_req), key);
}


bool HttpRequest::setHeader(const char * key, const char * value)
{
	evhttp_remove_header(evhttp_request_get_output_headers(_req), key);
	evhttp_add_header(evhttp_request_get_output_headers(_req), key, value);
	return true;
}

bool HttpRequest::setBody(const char * content)
{
	evbuffer_add_printf(_buffer, content);
	return true;
}

bool HttpRequest::setChunk(const char * content)
{
	  evbuffer_add_printf(_buffer, content);
	  evhttp_send_reply_chunk(_req, _buffer);

	  return true;
}

} /* namespace translib */


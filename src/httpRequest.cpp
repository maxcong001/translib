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
#include "translib/httpRequest.h"

namespace translib
{

HttpRequest::HttpRequest(evhttp_request *req) : _req(req)
{
	_buffer = evbuffer_new();
}

HttpRequest::~HttpRequest()
{
	evbuffer_free(_buffer);
}

const char *HttpRequest::getUri()
{
	return evhttp_request_get_uri(_req);
}

const char *HttpRequest::getPath()
{
	return evhttp_uri_get_path(evhttp_request_get_evhttp_uri(_req));
}

const char *HttpRequest::getHost()
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

const char *HttpRequest::findHeaders(const char *key)
{
	return evhttp_find_header(evhttp_request_get_input_headers(_req), key);
}

bool HttpRequest::setHeader(const char *key, const char *value)
{
	evhttp_remove_header(evhttp_request_get_output_headers(_req), key);
	evhttp_add_header(evhttp_request_get_output_headers(_req), key, value);
	return true;
}

bool HttpRequest::setBody(const char *content)
{
	evbuffer_add_printf(_buffer, content);
	return true;
}

bool HttpRequest::setChunk(const char *content)
{
	evbuffer_add_printf(_buffer, content);
	evhttp_send_reply_chunk(_req, _buffer);

	return true;
}

} /* namespace translib */

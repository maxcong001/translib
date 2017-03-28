/*
 * httpRequest.h
 *
 *  Created on: 2015年7月7日
 *      Author: 
 */

#ifndef INCLUDE_EC_HTTPREQUEST_H_
#define INCLUDE_EC_HTTPREQUEST_H_

#include <evhttp.h>

namespace translib
{

class HttpRequest
{
	friend class HttpServer;
public:
	HttpRequest(evhttp_request * req);
	~HttpRequest();

	const char * getUri();
	const char * getPath();
	const char * getHost();
	uint16_t getPort();
	evhttp_cmd_type getCommand(); 		
	const char * findHeaders(const char * key);

	bool setHeader(const char * key, const char * value);
	bool setBody(const char * content);
	bool setChunk(const char * content);

private:
	evhttp_request * _req;
	struct evbuffer * _buffer;
};

} /* namespace translib */


#endif /* INCLUDE_EC_HTTPREQUEST_H_ */

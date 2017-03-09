/*
 * tcpSessionFactory.h
 *
 *  Created on: 2016年7月12日
 *      Author: 
 */

#ifndef INCLUDE_EC_TCPSESSIONFACTORY_H_
#define INCLUDE_EC_TCPSESSIONFACTORY_H_

#include "translib/tcpSession.h"

namespace translib
{
class TcpSessionFactory
{
public:
	TcpSessionFactory() {};
	virtual ~TcpSessionFactory() {};

	virtual TcpSessionPtr create()
	{
		return TcpSessionPtr(new TcpSession());
	}
};

} /* namespace translib */



#endif /* INCLUDE_EC_TCPSESSIONFACTORY_H_ */

//============================================================================
// Name        : test.cpp
// Author      :
// Version     :
// Copyright   :
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <string>
#include <map>
using namespace std;

#include "translib/example/index.h"
#include "translib/define.h"
#include "logger/logger.h"
int main(int argc, char *argv[])
{
	// setup log related
	set_log_level(logger_iface::log_level::debug);

	string cmd = "TM";
	if (argc > 1)
	{
		cmd = argv[1];
	}

	map<string, translib::Handler> handlers;
	handlers["timer"] = std::bind(&timerExample);
	handlers["tcp"] = std::bind(&tcpExample);
	handlers["http"] = std::bind(&httpExample);
	handlers["TM"] = std::bind(&timerManagerExample);

	translib::Handler handler = handlers[cmd];
	if (!handler)
	{
		cout << "invalid handler" << endl;
		return 0;
	}

	handler();
	return 0;
}

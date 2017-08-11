/*
 * timer.cpp
 *
 *  Created on: 2015年6月29日
 *      Author: 
 */

#include <iostream>
#include <functional>
#include <unistd.h>
using namespace std;
//using namespace translib;

#include "translib/loop.h"
#include "translib/timerManager.h"
#include "index.h"

void timerManagerExample()
{
	translib::TimerManager TM;
	int timerID001 = 100;
	translib::Timer::ptr_p timer001 = TM.getTimer(&timerID001);
	std::cout << "timer ID 001 is " << timerID001 << std::endl;
	timer001->startOnce(10, [] { std::cout << "timer001" << std::endl; });

	int timerID002 = 100;
	translib::Timer::ptr_p timer002 = TM.getTimer(&timerID002);
	std::cout << "timer ID 002 is " << timerID002 << std::endl;
	timer002->startForever(1000, [] { std::cout << "timer002" << std::endl; });

	sleep(5);

	timer001->stop();
	TM.killTimer(timerID002);
	std::cout << "kill all the timer" << std::endl;
	sleep(5);
}

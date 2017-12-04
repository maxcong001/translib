#pragma once
/*
 * Copyright (c) 2016-20017 Max Cong <savagecm@qq.com>
 * this code can be found at https://github.com/maxcong001/CPP_test_env
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
#include "include/testUtil.hpp"
#include "translib/loop.h"

// name: dbw_001
// info: this field should record the case info
case_result TM_basic_body(void *arg)
{
  translib::Loop loop;
  translib::TimerManager TM(loop);
  int counter = 10;
  int timerIDCb001 = 100;
  __LOG(debug, "the address of timerIDCb001 is : " << (void *)(&timerIDCb001));
  {
    auto cbTimer = TM.getTimer(&timerIDCb001);
    cbTimer->startCB(500, [&](void *usrData) -> int {
      __LOG(debug, "user data is : " << (void *)usrData);
      return counter--;
    },
                     &timerIDCb001);
  }
  loop.start();
  sleep(10);

  TM.killTimer(timerIDCb001);
  return EXCEPT_EQ("test", "test");
}

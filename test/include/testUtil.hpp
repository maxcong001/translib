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
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "include/NonCopyable.hpp"
#include "include/singleton.hpp"
#include "logger/logger.h"
#include <unistd.h>
#include "translib/timerManager.h"
using namespace std;

enum case_result {
  CASE_SUCCESS = 0,
  CASE_RUNNING,
  CASE_STUB,
  CASE_FAIL

};
/*
typedef std::function<void(void *)> TEST_BODY_FUNCTION;
typedef std::function<void *()> TEST_PREPARE_FUNCTION;
typedef std::function<void(void *)> TEST_DESTROY_FUNCTION;
*/

typedef case_result (*TEST_BODY_FUNCTION)(void *);
typedef void *(*TEST_PREPARE_FUNCTION)();
typedef void (*TEST_DESTROY_FUNCTION)(void *);

typedef std::tuple<std::string, case_result> RESULT_TUPLE;
// case name and case result list, this will be read after all the case done.
// when after a case run, should put the case name and case pass, waiting , fail
// info to the list.
typedef std::list<RESULT_TUPLE> RESULT_LIST;

RESULT_LIST case_reslut_list;
//  NOTE: before using this template, you should write operator ==.
//  this is for sync compare result
//  to do: maybe use case class instance as arg, then print the case name and
//  case info.
template <typename EXP_RESULT, typename REL_RESULT>
case_result EXCEPT_EQ(EXP_RESULT &&except_result, REL_RESULT &&real_result) {
  return ((except_result == real_result) ? CASE_SUCCESS : CASE_FAIL);
}

void REC_RESULT(case_result result, string case_name) {
  case_reslut_list.emplace_back(std::make_tuple(case_name, result));
}
void ADD_SUIT_INFO(string suit_name) {
  case_reslut_list.emplace_back(std::make_tuple(suit_name, CASE_STUB));
}
// to do: async result calculate
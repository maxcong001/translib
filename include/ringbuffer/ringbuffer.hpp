/*
 * Copyright (c) 2016-20017 Max Cong <savagecm@qq.com>
 * this code can be found at https://github.com/maxcong001/ring_buffer_flat.git
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

#include <string.h>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
#define RB_SIZE 1024
#define RB_ELEMENT_SIZE 4096

std::mutex message_mutex;
std::condition_variable message_cond;
typedef std::unique_lock<std::mutex> message_lock;

class ring_buffer {
 public:
  // the mem map is as below:
  /*

  ------------------------------------------------------------------------
  |           |end_p                          |start_p                   |
  1. add message: memcpy to the start_p and add increase by length
  2. get message: get the end_p and increase end_p by length
  */
  ring_buffer() { init(); };
  ~ring_buffer() { destroy(); };

 private:
  bool init();
  bool destroy();

 public:
  // this function will increase the tail pointer by length
  // and return the start pointer
  // Note:
  // 1. you need to use the memory as soon as possible, or it will be overwrite
  // soon
  // especially in a havy load
  // 2. no need to free the message
  // 3. if you want to peek, just get(0), it will return the end_p
  // 4. return NULL when get fail, maybe no enough memory.
  typedef std::shared_ptr<char> shared_char;

  bool get(size_t length, char *buff) {
    if (length > size()) {
      // no enough space
      return false;
    }
    char *tmp_end = end_p;

    // char tmp_p[length];

    // std::shared_ptr<char[length]> ret(new char(length));
    char *tmp_p = buff;

    if (start_p >= end_p) {
      memcpy(tmp_p, end_p, length);
      end_p = tmp_end + length;
    } else {
      if ((end_p + length) < buffer_end) {
        memcpy(tmp_p, end_p, length);
        end_p = tmp_end + length;
      } else {
        memcpy(tmp_p, end_p, (buffer_end - end_p));
        memcpy((tmp_p + (buffer_end - end_p)), buffer_start,
               (length - (buffer_end - end_p)));
        end_p = tmp_end + length - buffer_end + buffer_start;
      }
    }
    return true;
  }

  bool peek(size_t length, char *buff) {
    if (length > size()) {
      // no enough space
      return false;
    }

    // char tmp_p[length];

    // std::shared_ptr<char[length]> ret(new char(length));
    char *tmp_p = buff;

    if (start_p >= end_p) {
      memcpy(tmp_p, end_p, length);
    } else {
      if ((end_p + length) < buffer_end) {
        memcpy(tmp_p, end_p, length);
      } else {
        memcpy(tmp_p, end_p, (buffer_end - end_p));
        memcpy((tmp_p + (buffer_end - end_p)), buffer_start,
               (length - (buffer_end - end_p)));
      }
    }
    return true;
  }

  // this function return the size of buffed message/
  size_t size() {
    size_t tmp = 0;
    if (start_p >= end_p) {
      tmp = start_p - end_p;
    } else {
      tmp = buffer_end - buffer_start - (end_p - start_p);
    }
    return tmp;
  }

  // not get the head pointer
  // note: when new message come, add to the head.
  // when add, you need to
  // 1. peek head pointer
  // 2. memcpy the message
  // 3. add the head pointer by length(call add())
  //

  char *peek_head_p() { return start_p; }

  char *peek_tail_p() { return end_p; }

  // when add fail, should retry
  bool add(size_t length, const char *buff) {
    if ((start_p + length) < buffer_end) {
      if (start_p < end_p) {
        if ((start_p + length) > end_p) {
          // no enough space
          return false;
        }
      }
      if (!memcpy(start_p, buff, length)) {
        // memcpy error
        return false;
      }

      start_p += length;
    }
    /*
    else if ((start_p + length) == buffer_end)
    {
        start_p = buffer_start;
    }
    */
    else {
      if (length >= (RB_SIZE * RB_ELEMENT_SIZE)) {
        // length is to large, just return
        return false;
      }
      char *tmp = NULL;

      tmp = start_p + length - buffer_end + buffer_start;

      if (tmp > end_p) {
        // no enough space
        return false;
      } else {
        if (!memcpy(start_p, buff, (buffer_end - start_p))) {
          return false;
        }
        if (!memcpy((buffer_start), (buff + (buffer_end - start_p)),
                    (length - (buffer_end - start_p)))) {
          return false;
        }
        start_p = tmp;
      }
    }
    return true;
  }

  void reborn() {
    start_p = buffer_start;
    end_p = buffer_start;
  }

  void dump() { std::cout << "ring buffer element is \n" << std::endl; }

  char *start_p;
  char *end_p;
  char *buffer_start;  // this is the start of the buffer
  char *buffer_end;
};

bool ring_buffer::init() {
  // to do: alloc page allagent
  buffer_start = (char *)calloc(RB_SIZE, RB_ELEMENT_SIZE);
  if (!buffer_start) {
    return false;
  }
  buffer_end = buffer_start + RB_SIZE * RB_ELEMENT_SIZE;
  start_p = buffer_start;
  end_p = buffer_start;
  return true;
}

bool ring_buffer::destroy() {
  if (buffer_start) {
    free(buffer_start);
  }
  return true;
}

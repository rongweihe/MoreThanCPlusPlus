/*
  Copyright (c) 2019 rongweihe, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Author: rongweihe (rongweihe1995@gmail.com)
  Data:   2020/11/09
  Desc: 可适配的队列 C++ 实现
*/
#pragma once

#include "CQueue.h"
#include <mutex>
#include <condition_variable>  
#include <queue> 
#include <deque>
#include <chrono>
#include <iostream>
#include <boost>

template <class TQueueMember>
CQueue<TQueueMember>::CQueue(int max_queue_size/*=-1*/) : m_max_task_num(max_queue_size), m_stop_flag(true) {}

template <class TQueueMember>
void CQueue<TQueueMember>::Open() {
    {
        sd::unique_lock<std::mutex> ul(m_task_mutex);
        m_stop_flag = false;
    }
}

template <class TQueueMember>
void CQueue<TQueueMember>::Close() {
    {
        std::unique_lock<std::mutex> ul(m_task_mutex);
        m_stop_flag = true;
        m_cond_notFull.notify_all();
        m_cond_notEmpty.notify_all();
    }
}

//添加任务
template <class TQueueMember>
int CQueue<TQueueMember>::Add(boost::shared_ptr<TQueueMember>&task, int timeout_ms /*-1*/) {
    int retry_count = 0;
    {
        std::unique_lock<std::mutex> ul(m_task_mutex);
        while (true) {
            if (m_stop_flag) return 1;
            if (m_max_task_num == -1 || m_task_queue.size() < m_max_task_num) {
                m_task_queue.push_back(task);
                m_cond_notEmpty.notify_one();
                break;
            } else {
                if (timeout_ms == 0) return -1;
                if (timeout_ms <0 ) {
                    m_cond_notFull.wait(ul);
                } else if (retry_count >=3) {
                    return -3;
                } else {
                    if (m_cond_notFull.wait_for(ul, std::chrono::milliseconds(timeout_ms)) == std::cv_status::timeout) {
                        return -2;
                    }
                }
            }
            ++ retry_count;
        }
    }
    return 0;
}

//取任务
template <class TQueueMember>
int CQueue<TQueueMember>::Get(boost::shared_ptr<TQueueMember>& ptask, int timeout_ms/* = -1*/) {
    int retry_count = 0;
    {
        std::unique_lock<std::mutex> ul(m_task_mutex);
        while (true) {
            if (m_stop_flag) return 1;
            if (m_task_queue.size() > 0) {
                ptask = m_task_queue.front();
                m_task_queue.pop_front();
                m_cond_notFull.notify_one();
                break;
            } else {
                if (timeout_ms == 0) return -1;
                if (timeout_ms < 0) {
                    m_cond_notEmpty.wait(ul);
                } else if (retry_count >= 3) {
                    return -3;
                } else {
                    if (m_cond_notEmpty.wait_for(ul, std::chrono::milliseconds(timeout_ms)) == std::cv_status::timeout) {
                        return -2;
                    }
                }
            }
            ++ retry_count;
        }
    }
    return 0;
}

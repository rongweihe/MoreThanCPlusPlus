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

#include <mutex>
#include <condition_variable>  
#include <queue> 
#include <deque>
#include <chrono>
#include <iostream>
#include <boost>

template <class TQueueMember>
class CQueue {
public:
	CQueue(int max_queue_size = 1);
	void Open();
	void Close();

	int Add(boost::shared_ptr<TQueueMember>& task, int timeout_ms = -1);//timeout_ms = 0, queue full return immediately
	int Get(boost::shared_ptr<TQueueMember>& task, int timeout_ms = -1); //timeout_ms = 0
protected:
	std::deque<boost::shared_ptr<TQueueMember> > m_task_queue;
	const int m_max_task_num;
	std::mutex m_task_mutex;
	bool m_stop_flag;
	std::condition_variable m_cond_notEmpty;
	std::condition_variable m_cond_notFull;
};
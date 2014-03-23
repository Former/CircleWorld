#pragma once 

#include "Common.h"
#include "AsyncOpForThreadPool.h"

namespace ThreadPool
{
	class 	WorkThread
	{
	public:
		WorkThread();
		~WorkThread();
		
		void AddOperation(const AsyncOpForPoolPtr& a_Operation);
		
	private:
		void Work();
		
		std::thread m_Thread;
		AsyncOpForPoolQueue m_OpQueue;
		std::recursive_mutex m_QueueMutex;
		std::condition_variable m_QueueEvent;
		bool m_Exit;
	};
}

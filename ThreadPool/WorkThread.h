#pragma once 

#include "Common.h"

namespace ThreadPool
{
	class 	WorkThread
	{
	public:
		WorkThread();
		~WorkThread();
		
		void AddOperations(const AsyncOpVector& a_Operations);
		
	private:
		void Work();
		
		std::thread m_Thread;
		AsyncOpQueue m_OpQueue;
		std::mutex m_QueueMutex;
		std::condition_variable m_QueueEvent;
		bool m_Exit;
	};
}

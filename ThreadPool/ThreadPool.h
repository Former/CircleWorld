#pragma once 

#include "Common.h"
#include "WorkThread.h"

namespace ThreadPool
{
	class AsyncOpForPool;
	typedef std::shared_ptr<AsyncOpForPool> AsyncOpForPoolPtr;
	
	class 	ThreadPool
	{
	public:
		ThreadPool(const size_t& a_ThreadCount);
		
		void AddOperation(const AsyncOpForPoolPtr& a_Operation);
		
	private:
		std::vector<WorkThread> m_WorkThreads;
		std::random_device m_RandomDevice;
		std::default_random_engine m_RandomEngine;
		std::uniform_int_distribution<int> m_RandomDis;
	};
	typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;
}

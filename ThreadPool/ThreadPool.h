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
		
		void AddOperation(const AsyncOpForPoolPtr& a_Operation, const size_t& a_ThreadIndex);

		size_t GetThreadCount() const;
		
	private:
		std::vector<WorkThreadPtr> m_WorkThreads;		
	};
	typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;
}

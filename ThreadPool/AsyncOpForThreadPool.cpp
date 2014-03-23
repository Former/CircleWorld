#include "AsyncOpForThreadPool.h"
#include "ThreadPool.h"

ThreadPool::AsyncOpForPool::AsyncOpForPool(const ThreadPoolPtr& a_ThreadPool)
{
	m_ThreadPool = a_ThreadPool;
}

void ThreadPool::AsyncOpForPool::Run(const IAsyncFinishHandlerPtr& a_Handler)
{
	m_Handler = a_Handler;
	AsyncOpForPoolPtr this_op = std::dynamic_pointer_cast<AsyncOpForPool>(shared_from_this());
	m_ThreadPool.AddOpearation(this_op);
}

void ThreadPool::AsyncOpForPool::SyncRun()
{
	__SyncRun();
	
	if (m_Handler)
		m_Handler->OnFinish();
}

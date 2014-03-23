#include "WorkThread.h"

ThreadPool::WorkThread::WorkThread() 
: m_Thread(&WorkThread::Work, this)
{
	m_Exit = false;
}

ThreadPool::WorkThread::~WorkThread()
{
	m_Exit = true;
	m_QueueEvent.notify_one();
	m_Thread.join();
}

void ThreadPool::WorkThread::AddOperation(const AsyncOpForPoolPtr& a_Operation)
{
	std::unique_lock<std::mutex> lock(m_QueueMutex);
	
	m_OpQueue.push(a_Operation);
	
	SortByPriority(m_OpQueue);
	
	m_QueueEvent.notify_one();
}

void ThreadPool::WorkThread::Work()
{
	while(!m_Exit)
	{
		IAsyncOperationPtr cur_op;
		{
			std::unique_lock<std::mutex> lock(m_QueueMutex);
			if (!m_OpQueue.empty())
			{
				cur_op = m_OpQueue.front();
				m_OpQueue.pop();
			}
			else
				m_QueueEvent.wait(lock);		
		}
		
		if (!cur_op)
			continue;
			
		cur_op->SyncRun();
	}
}

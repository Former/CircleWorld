#include "WaitHandler.h"

void ThreadPool::WaitHandler::OnFinish()
{
	m_QueueEvent.notify_one();
}

void ThreadPool::WaitHandler::Wait()
{
	std::unique_lock<std::mutex> lock(m_QueueMutex);
	m_QueueEvent.wait(lock);
}
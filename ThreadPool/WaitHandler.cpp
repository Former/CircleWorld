#include "WaitHandler.h"

ThreadPool::WaitHandler::WaitHandler()
{
	m_IsFinished = false;
}

void ThreadPool::WaitHandler::OnFinish()
{
	std::unique_lock<std::mutex> lock(m_QueueMutex);
	m_QueueEvent.notify_one();
	m_IsFinished = true;
}

void ThreadPool::WaitHandler::Wait()
{
	std::unique_lock<std::mutex> lock(m_QueueMutex);
	while (!m_IsFinished)
		m_QueueEvent.wait(lock);
}
#include "ThreadPool.h"

ThreadPool::ThreadPool::ThreadPool(const size_t& a_ThreadCount)
{
	m_WorkThreads.resize(a_ThreadCount);
	for (size_t i = 0; i < m_WorkThreads.size(); ++i)
		m_WorkThreads[i] = std::make_shared<WorkThread>();
}

void ThreadPool::ThreadPool::AddOperation(const AsyncOpForPoolPtr& a_Operation, const size_t& a_ThreadIndex)
{
	if (m_WorkThreads.empty())
		return;
	
	m_WorkThreads[a_ThreadIndex]->AddOperation(a_Operation);
}


size_t ThreadPool::ThreadPool::GetThreadCount() const
{
	return m_WorkThreads.size();
}

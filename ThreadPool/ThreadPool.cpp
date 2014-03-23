
ThreadPool::ThreadPool::ThreadPool(const size_t& a_ThreadCount)
: m_RandomEngine(m_RandomDevice()), m_RandomDis(0, a_ThreadCount - 1)
{
	m_WorkThreads.resize(a_ThreadCount);
}

void ThreadPool::ThreadPool::AddOperation(const AsyncOpForPoolPtr& a_Operation)
{
	if (m_WorkThreads.empty())
		return;
	
	int cur_index = m_RandomDis(m_RandomEngine);
	m_WorkThreads[cur_index].AddOperation(a_Operation);
}


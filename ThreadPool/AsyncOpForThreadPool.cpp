#include "AsyncOpForThreadPool.h"
#include "ThreadPool.h"

namespace
{
	class RandomGenerator
	{
	public:
		RandomGenerator();
		
		size_t GetRandom(const size_t& a_Max);
	
	private:
		std::random_device m_RandomDevice;
		std::default_random_engine m_RandomEngine;
	};

	RandomGenerator::RandomGenerator()
	: m_RandomEngine(m_RandomDevice())
	{			
	}

	size_t RandomGenerator::GetRandom(const size_t& a_Max)
	{
		std::uniform_int_distribution<int> random_dis(0, a_Max - 1);
		
		return random_dis(m_RandomEngine);
	}
}

ThreadPool::AsyncOpForPool::AsyncOpForPool(const ThreadPoolPtr& a_ThreadPool)
{
	m_ThreadPool = a_ThreadPool;
}

void ThreadPool::AsyncOpForPool::Run(const IAsyncFinishHandlerPtr& a_Handler)
{
	m_Handler = a_Handler;
	AsyncOpForPoolPtr this_op = std::dynamic_pointer_cast<AsyncOpForPool>(shared_from_this());
	m_ThreadPool->AddOperation(this_op, GetThreadIndex(m_ThreadPool->GetThreadCount()));
}

void ThreadPool::AsyncOpForPool::SyncRun()
{
	__SyncRun();
	
	if (m_Handler)
		m_Handler->OnFinish();
}

RandomGenerator g_Random;

size_t ThreadPool::AsyncOpForPool::GetThreadIndex(const size_t& a_MaxThreadCount) const
{
	return g_Random.GetRandom(a_MaxThreadCount);
}

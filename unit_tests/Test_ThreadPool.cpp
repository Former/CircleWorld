#include <gtest/gtest.h>

#include <ThreadPool.h>
#include <AsyncOpForThreadPool.h>
#include <AsyncSequenceOperation.h>
#include <AsyncParallelOperation.h>
#include <WaitHandler.h>

namespace
{
	class TestOp: public ThreadPool::AsyncOpForPool
	{
	public:
		TestOp(size_t& a_Value, const ThreadPool::ThreadPoolPtr& a_ThreadPool)
		: m_Value(a_Value), ThreadPool::AsyncOpForPool(a_ThreadPool)
		{			
		}

		virtual double GetPriority() const override
		{
			return 0;
		}
	
	protected:
		virtual void __SyncRun() override
		{
			m_Value++;
		}
		
		size_t& m_Value;
	};
}

TEST(ThreadPool, Init)
{
	ThreadPool::ThreadPoolPtr pool = std::make_shared<ThreadPool::ThreadPool>(2);
	
	EXPECT_EQ(2, pool->GetThreadCount());
}

TEST(ThreadPool, SequenceOp)
{
	ThreadPool::ThreadPoolPtr pool = std::make_shared<ThreadPool::ThreadPool>(2);
	
	std::vector<size_t> values(10);
	
	ThreadPool::AsyncOpVector m_Ops;
	for (size_t i = 0; i < 10; ++i)
		m_Ops.push_back(std::make_shared<TestOp>(std::ref(values[i]), std::cref(pool)));
		
	ThreadPool::IAsyncOperationPtr seq_op = std::make_shared<ThreadPool::AsyncParallelOperation>(m_Ops);
	
	ThreadPool::WaitHandlerPtr handler = std::make_shared<ThreadPool::WaitHandler>();
	
	seq_op->Run(handler);
	
	handler->Wait();
	
	for (size_t i = 0; i < 10; ++i)
		EXPECT_EQ(1, values[i]);
}

TEST(ThreadPool, ParallelOp)
{
	ThreadPool::ThreadPoolPtr pool = std::make_shared<ThreadPool::ThreadPool>(2);
	
	size_t value = 0;
	
	ThreadPool::AsyncOpVector m_Ops;
	for (size_t i = 0; i < 10; ++i)
		m_Ops.push_back(std::make_shared<TestOp>(std::ref(value), std::cref(pool)));
		
	ThreadPool::IAsyncOperationPtr seq_op = std::make_shared<ThreadPool::AsyncSequenceOperation>(m_Ops);
	
	ThreadPool::WaitHandlerPtr handler = std::make_shared<ThreadPool::WaitHandler>();
	
	seq_op->Run(handler);
	
	handler->Wait();
	
	EXPECT_EQ(10, value);
}
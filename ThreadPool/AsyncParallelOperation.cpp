#include "Common.h"
#include "AsyncOperation.h"
#include "AsyncParallelOperation.h"

namespace
{
	class AsyncParallel_FinishHandler : public ThreadPool::IAsyncFinishHandler
	{
	public:
		AsyncParallel_FinishHandler(const size_t& a_OpVectorSize, const ThreadPool::IAsyncFinishHandlerPtr& a_ParentHandler);
		
		virtual void OnFinish() override;
	
	private:
		ThreadPool::IAsyncFinishHandlerPtr m_ParentHandler;
		size_t m_OpVectorSize;
		std::mutex m_FinishCountMutex;
		size_t m_FinishCount; 
	};
	
	AsyncParallel_FinishHandler::AsyncParallel_FinishHandler(const size_t& a_OpVectorSize, const ThreadPool::IAsyncFinishHandlerPtr& a_ParentHandler)
	{
		m_ParentHandler = a_ParentHandler;
		m_OpVectorSize = a_OpVectorSize;
		m_FinishCount = 0;
	}

	void AsyncParallel_FinishHandler::OnFinish()
	{
		{
			std::unique_lock<std::mutex> lock(m_FinishCountMutex);
			m_FinishCount++;
		}

		if (m_FinishCount == m_OpVectorSize)
		{
			if (m_ParentHandler)
				m_ParentHandler->OnFinish();
		}
	}
}

ThreadPool::AsyncParallelOperation::AsyncParallelOperation(const AsyncOpVector& a_OpVector)
{
	m_OpVector = a_OpVector;
}

void ThreadPool::AsyncParallelOperation::Run(const IAsyncFinishHandlerPtr& a_Handler)
{
	if (m_OpVector.empty())
		return;
		
	IAsyncFinishHandlerPtr handler = std::make_shared<AsyncParallel_FinishHandler>(m_OpVector.size(), a_Handler);
	
	for (size_t i = 0; i < m_OpVector.size(); ++i)
		m_OpVector[i]->Run(handler);
}

double ThreadPool::AsyncParallelOperation::GetPriority() const
{
	return MinPriority(m_OpVector);
}
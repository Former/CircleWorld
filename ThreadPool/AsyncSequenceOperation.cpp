#include "Common.h"
#include "AsyncOperation.h"
#include "AsyncSequenceOperation.h"

namespace
{
	class AsyncSequence_FinishHandler : public ThreadPool::IAsyncFinishHandler
	{
	public:
		AsyncSequence_FinishHandler(const ThreadPool::AsyncOpVector& a_OpVector, const ThreadPool::IAsyncFinishHandlerPtr& a_ParentHandler);
		
		virtual void OnFinish() override;
	
	private:
		const ThreadPool::AsyncOpVector& m_OpVector;
		ThreadPool::IAsyncFinishHandlerPtr m_ParentHandler;
		size_t m_CurIndex; 
	};
	
	AsyncSequence_FinishHandler::AsyncSequence_FinishHandler(const ThreadPool::AsyncOpVector& a_OpVector, const ThreadPool::IAsyncFinishHandlerPtr& a_ParentHandler)
	: m_OpVector(a_OpVector)
	{
		m_ParentHandler = a_ParentHandler;
		m_CurIndex = 0;		
	}

	void AsyncSequence_FinishHandler::OnFinish()
	{
		m_CurIndex++;
		if (m_CurIndex >= m_OpVector.size())
		{
			if (m_ParentHandler)
				m_ParentHandler->OnFinish();
			
			return	;
		}
		
		m_OpVector[m_CurIndex]->Run(shared_from_this());
	}
}

ThreadPool::AsyncSequenceOperation::AsyncSequenceOperation(const AsyncOpVector& a_OpVector)
{
	m_OpVector = a_OpVector;
}

void ThreadPool::AsyncSequenceOperation::Run(const IAsyncFinishHandlerPtr& a_Handler)
{
	if (m_OpVector.empty())
		return;
		
	IAsyncFinishHandlerPtr handler = std::make_shared<AsyncSequence_FinishHandler>(m_OpVector, a_Handler);
	m_OpVector[0]->Run(handler);
}

double ThreadPool::AsyncSequenceOperation::GetPriority() const
{
	return MinPriority(m_OpVector);
}
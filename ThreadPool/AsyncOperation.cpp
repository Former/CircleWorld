#include "AsyncOperation.h"
#include "Common.h"

ThreadPool::IAsyncFinisshHandler::~IAsyncFinisshHandler()
{
}

void ThreadPool::AsyncOperation::OnFinish()
{	
	if (m_Handler)
		m_Handler->OnFinish();
}

void ThreadPool::AsyncOperation::SetFinishHandler(const IAsyncFinisshHandlerPtr& a_Handler)
{
	m_Handler = a_Handler;
}

ThreadPool::AsyncOperation::~AsyncOperation()
{
}

namespace
{
	class OpPredicate
	{
	public:
		bool operator()(const IAsyncOperationPtr& a_Op1, const IAsyncOperationPtr& a_Op2)
		{
			return (a_Op1->GetPriority() < a_Op2->GetPriority());
		}	
	};
}

void ThreadPool::SortByPriority(IN OUT AsyncOpQueue& a_OpQueue)
{
	std::sort(a_OpQueue.begin(), a_OpQueue.end(), OpPredicate());
}

#pragma once 

#include "IAsyncOperation.h"

namespace ThreadPool
{
	class AsyncParallelOperation : public IAsyncOperation
	{
	public:
		AsyncParallelOperation(const AsyncOpVector& a_OpVector);

		virtual void Run(const IAsyncFinishHandlerPtr& a_Handler) override;
		
		double GetPriority() const override;
		
	private:
		AsyncOpVector m_OpVector;
	};
}
#pragma once 

#include "IAsyncOperation.h"

namespace ThreadPool
{
	class AsyncSequenceOperation : public IAsyncOperation
	{
	public:
		AsyncSequenceOperation(const AsyncOpVector& a_OpVector);

		virtual void Run(const IAsyncFinishHandlerPtr& a_Handler) override;
		
		virtual double GetPriority() const override;
		
	private:
		AsyncOpVector m_OpVector;
	};
}
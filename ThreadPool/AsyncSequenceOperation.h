#pragma once 

namespace ThreadPool
{
	class AsyncSequenceOperation : public IAsyncOperation
	{
	public:
		AsyncSequenceOperation(const AsyncOpVector& a_OpVector);

		virtual void Run() override;
		
		virtual void OnFinish() override;

		virtual double GetPriority() const override;
		
	private:
		AsyncOpVector m_OpVector;
	};
}
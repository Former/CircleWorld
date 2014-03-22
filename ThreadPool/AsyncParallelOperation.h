#pragma once 

namespace ThreadPool
{
	class AsyncParallelOperation : public IAsyncOperation
	{
	public:
		AsyncParallelOperation(const AsyncOpVector& a_OpVector);

		virtual void Run() override;
		
		virtual void OnFinish() override;

		double GetPriority() const override;
		
	private:
		AsyncOpVector m_OpVector;
	};
}
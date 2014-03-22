#pragma once 

namespace ThreadPool
{
	class IAsyncFinishHandler
	{
	public:
		virtual ~IAsyncFinishHandler();
		
		virtual OnFinish() = 0;		
	};
	typedef std::shared_ptr<IAsyncFinishHandler> IAsyncFinishHandlerPtr;


	class IAsyncOperation
	{
		friend class AsyncParallelOperation;
		friend class AsyncSequenceOperation;
	public:
		virtual ~IAsyncOperation();

		virtual void Run() = 0;
		
		virtual void OnFinish();

		virtual double GetPriority() const = 0;

	protected:
		void SetFinishHandler(const IAsyncFinishHandlerPtr& a_Handler);

		IAsyncFinishHandlerPtr m_Handler;
	};
	typedef std::shared_ptr<IAsyncOperation> IAsyncOperationPtr;
	typedef std::vector<IAsyncOperationPtr> AsyncOpVector;
	typedef std::queue<IAsyncOperationPtr> AsyncOpQueue;
	
	void SortByPriority(IN OUT AsyncOpQueue& a_OpVector);
}
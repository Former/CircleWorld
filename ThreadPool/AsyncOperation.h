#pragma once 

namespace ThreadPool
{
	class IAsyncFinishHandler : public std::enable_shared_from_this<IAsyncFinishHandler>
	{
	public:
		virtual ~IAsyncFinishHandler();
		
		virtual OnFinish() = 0;		
	};
	typedef std::shared_ptr<IAsyncFinishHandler> IAsyncFinishHandlerPtr;

	//////////////////////////////////////////////////////////////////////////////

	class IAsyncOperation : public std::enable_shared_from_this<IAsyncOperation>
	{
	public:
		virtual ~IAsyncOperation();

		virtual void Run(const IAsyncFinishHandlerPtr& a_Handler) = 0;
		
		virtual double GetPriority() const = 0;
	};
	typedef std::shared_ptr<IAsyncOperation> IAsyncOperationPtr;
	typedef std::vector<IAsyncOperationPtr> AsyncOpVector;
	typedef std::queue<IAsyncOperationPtr> AsyncOpQueue;
	
	//////////////////////////////////////////////////////////////////////////////
	
	class OpPriorityPredicate
	{
	public:
		bool operator()(const IAsyncOperationPtr& a_Op1, const IAsyncOperationPtr& a_Op2)
		{
			return (a_Op1->GetPriority() < a_Op2->GetPriority());
		}	
	};

	template <typename OpVector>
	void SortByPriority(IN OUT OpVector& a_OpQueue)
	{
		std::sort(a_OpQueue.begin(), a_OpQueue.end(), OpPriorityPredicate());
	}
	
	double MinPriority(IN const OpVector& a_OpVector);

}
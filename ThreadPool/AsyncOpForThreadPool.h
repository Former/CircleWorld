#pragma once 

#include "AsyncOperation.h"

namespace ThreadPool
{
	class ThreadPool;
	typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;
	
	class AsyncOpForPool : public IAsyncOperation
	{
	public:
		AsyncOpForPool(const ThreadPoolPtr& a_ThreadPool);

		virtual void Run(const IAsyncFinishHandlerPtr& a_Handler) override;

		void SyncRun();

		virtual size_t GetThreadIndex(const size_t& a_MaxThreadCount) const;
		
		virtual double GetPriority() const = 0;
	
	protected:
		virtual void __SyncRun() = 0;
	
	private:
		IAsyncFinishHandlerPtr m_Handler;
		ThreadPoolPtr m_ThreadPool;
	};
	typedef std::shared_ptr<AsyncOpForPool> AsyncOpForPoolPtr;
	typedef std::vector<AsyncOpForPoolPtr> AsyncOpForPoolVector;
}
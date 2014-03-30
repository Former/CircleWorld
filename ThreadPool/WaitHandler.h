#pragma once 

#include "AsyncOperation.h"

namespace ThreadPool
{
	class WaitHandler : public IAsyncFinishHandler
	{
	public:
		virtual void OnFinish() override;

		void Wait();
		
	private:
		std::condition_variable m_QueueEvent;
		std::mutex m_QueueMutex;
	};
	
	typedef std::shared_ptr<WaitHandler> WaitHandlerPtr;
}
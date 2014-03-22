#pragma once 

namespace ThreadPool
{
	class IAsyncFinisshHandler
	{
		virtual ~IAsyncFinisshHandler();
		
		
		
	};


	class IAsyncOperation
	{
		virtual ~IAsyncOperation();

		void Run();
		
		void OnFinish();

		void SetFinishHandler();
		
	protected:
		virtual void __Run() = 0;
	
		virtual void __OnFinish() = 0;
		
	};
}
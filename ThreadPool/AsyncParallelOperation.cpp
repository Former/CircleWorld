#include "AsyncParallelOperation.h"

ThreadPool::AsyncParallelOperation::AsyncParallelOperation(const AsyncOpVector& a_OpVector)
{
	m_OpVector = a_OpVector;
}

void ThreadPool::AsyncParallelOperation::Run()
{
	
}

void ThreadPool::AsyncParallelOperation::OnFinish()
{
	
}

double ThreadPool::AsyncParallelOperation::GetPriority() const
{
	
}
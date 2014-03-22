#include "AsyncSequenceOperation.h"

ThreadPool::AsyncSequenceOperation::AsyncSequenceOperation(const AsyncOpVector& a_OpVector)
{
	m_OpVector = a_OpVector;
}

void ThreadPool::AsyncSequenceOperation::Run()
{
	
}

void ThreadPool::AsyncSequenceOperation::OnFinish()
{
	
}

double ThreadPool::AsyncSequenceOperation::GetPriority() const
{
	
}
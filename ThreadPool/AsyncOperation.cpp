#include "AsyncOperation.h"
#include "Common.h"

ThreadPool::IAsyncFinishHandler::~IAsyncFinishHandler()
{
}

ThreadPool::IAsyncOperation::~IAsyncOperation()
{
}

double ThreadPool::MinPriority(IN const OpVector& a_OpVector)
{
	IAsyncOperationPtr min_op = std::min_element(a_OpVector.begin(), a_OpVector.end(), OpPriorityPredicate());
	return min_op->GetPriority();
}

#pragma once

#include "CircleItem.h"
#include "IntPoint.h"
#include "ThreadPool/AsyncOperation.h"
#include "ThreadPool/ThreadPool.h"

ThreadPool::IAsyncOperationPtr OpFillItems(const F3DCircleNodePtr& a_InOutObject, const IntPoint& a_Center, const double& a_Radius, const CircleItem& a_Item, const size_t& a_GroupLength, const ThreadPool::ThreadPoolPtr& a_ThreadPool);

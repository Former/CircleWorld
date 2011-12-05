#pragma once

#include <vector>
#include <string>
#include <tr1/memory>

#define engine_shared_ptr 	std::tr1::shared_ptr
#define engine_weak_ptr 	std::tr1::weak_ptr
#define engine_make_shared 	std::tr1::make_shared

namespace CircleEngine
{
	typedef double CoordinateType;
	typedef float WeightType;
	typedef size_t COIndex;
}

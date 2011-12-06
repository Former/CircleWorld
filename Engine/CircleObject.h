#pragma once

#include "Point.h"

namespace CircleEngine
{
	class CircleObject
	{
	public:
		CircleObject();

		Point			Center;
		Point			Velocity;
		CoordinateType	Radius;
		WeightType		Weight;
		bool 			IsFixed;
	};
	
	typedef engine_shared_ptr<CircleObject> CircleObjectPtr;
}
#pragma once

#include "Point.h"

namespace CircleEngine
{
	class CircleObject
	{
	public:
		CircleObject();
		
		size_t GetID();
		
		Point			Center;
		Point			Velocity;
		CoordinateType	Radius;
		WeightType		Weight;
		
	private:
		size_t m_ID;
	};
	
	typedef engine_shared_ptr<CircleObject> CircleObjectPtr;
}
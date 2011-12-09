#include "CircleObject.h"

namespace CircleEngine
{
	size_t g_CircleObject_LastID = 0;
	
	CircleObject::CircleObject()
	{
		Radius 	= 1.0;
		Weight 	= 1.0;
		m_ID 	= ++g_CircleObject_LastID;
	}
	
	size_t CircleObject::GetID()
	{
		return m_ID;
	}
}
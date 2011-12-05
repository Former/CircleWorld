#include "Point.h"

#include <math.h>

#define abs(x) (((x) >= 0) ? (x) : (-(x)))

namespace CircleEngine
{
	Point::Point()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}

	Point::Point(CoordinateType a_x, CoordinateType a_y, CoordinateType a_z)
	{
		x = a_x;
		y = a_y;
		z = a_z;
	}

	bool Point::CheckConnect(const Point& a_OtherPoint, const CoordinateType& a_Distance) const
	{
		const CoordinateType absX = abs(x - a_OtherPoint.x);
		const CoordinateType absY = abs(y - a_OtherPoint.y);
		const CoordinateType absZ = abs(z - a_OtherPoint.z);

		if (absX > a_Distance || absY > a_Distance || absY > a_Distance)
			return false;
		
		const CoordinateType dist2 = (absX * absX) + (absY * absY) + (absZ * absZ);

		return (dist2 < a_Distance * a_Distance);
	}

	CoordinateType Point::Distance2(const Point& a_OtherPoint) const
	{
		return ((x - a_OtherPoint.x) * (x - a_OtherPoint.x) + (y - a_OtherPoint.y) * (y - a_OtherPoint.y) + (z - a_OtherPoint.z) * (z - a_OtherPoint.z));
	}

	CoordinateType Point::Distance(const Point& a_OtherPoint) const
	{
		return sqrt(Distance2(a_OtherPoint)); 
	}

	CoordinateType Point::operator * (const Point& a_Value) const // Скаляное произведение
	{
		return (x * a_Value.x + y * a_Value.y + z * a_Value.z); 
	}


	Point Point::operator * (const CoordinateType& a_Value) const
	{
		return Point(x * a_Value, y * a_Value, z * a_Value); 
	}

	Point Point::operator / (const CoordinateType& a_Value) const
	{
		return Point(x / a_Value, y / a_Value, z / a_Value); 
	}

	Point Point::operator + (const Point& a_Value) const
	{
		return Point(x + a_Value.x, y + a_Value.y, z + a_Value.z); 
	}

	Point Point::operator - (const Point& a_Value) const
	{
		return Point(x - a_Value.x, y - a_Value.y, z - a_Value.z); 
	}
}
#pragma once

#include "CommonTypes.h"

namespace CircleEngine
{
	class Point
	{
	public:
		Point();
		Point(CoordinateType a_x, CoordinateType a_y, CoordinateType a_z);

		bool 			CheckConnect(const Point& a_OtherPoint, const CoordinateType& a_Distance) const;
		CoordinateType 	Distance2(const Point& a_OtherPoint) const;
		CoordinateType 	Distance(const Point& a_OtherPoint) const;

		CoordinateType operator * (const Point& a_Value) const; // Скалярное произведение
		Point operator * (const CoordinateType& a_Value) const;
		Point operator / (const CoordinateType& a_Value) const;
		Point operator + (const Point& a_Value) const;
		Point operator - (const Point& a_Value) const;
		bool operator == (const Point& a_Value) const;

		CoordinateType x;
		CoordinateType y;
		CoordinateType z;
	};
}
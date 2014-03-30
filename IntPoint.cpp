#include "IntPoint.h"

IntPoint::IntPoint()
{
	x = 0;
	y = 0;
	z = 0;
}

IntPoint::IntPoint(const IntType& a_X, const IntType& a_Y, const IntType& a_Z)
{
	x = a_X;
	y = a_Y;
	z = a_Z;
}

IntPoint IntPoint::operator * (const IntType& a_Value) const
{
	return IntPoint(x * a_Value, y * a_Value, z * a_Value);
}

IntPoint IntPoint::operator * (const double& a_Value) const
{
	return IntPoint(x * a_Value, y * a_Value, z * a_Value);
}

IntPoint IntPoint::operator + (const IntPoint& a_Value) const
{
	return IntPoint(x + a_Value.x, y + a_Value.y, z + a_Value.z);
}

IntPoint IntPoint::operator - (const IntPoint& a_Value) const
{
	return IntPoint(x - a_Value.x, y - a_Value.y, z - a_Value.z);
}

IntPoint IntPoint::operator / (const IntType& a_Value) const
{
	return IntPoint(x / a_Value, y / a_Value, z / a_Value);
}

bool IntPoint::operator == (const IntPoint& a_Value) const
{
	return (x == a_Value.x) && (y == a_Value.y) && (z == a_Value.z);
}

bool IntPoint::operator <=(const IntPoint& a_Value) const
{
	return (x <= a_Value.x) && (y <= a_Value.y) && (z <= a_Value.z);
}

bool IntPoint::operator >=(const IntPoint& a_Value) const
{
	return (x >= a_Value.x) && (y >= a_Value.y) && (z >= a_Value.z);
}

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

double IntPoint::GetLength2() const
{
	return (double(x) * x) + (double(y) * y) + (double(z) * z);
}

//////////////////////////////////////////////////////////////

BBox::BBox(const IntPoint& a_Start, const IntPoint& a_End)
{
	m_Start = a_Start;
	m_End = a_End;
}

IntPoint BBox::GetCenter() const
{
	return (m_End + m_Start) / 2; // IntPoint((m_End.x + m_Start.x) >> 1, (m_End.y + m_Start.y) >> 1, (m_End.z + m_Start.z) >> 1);
}

double BBox::GetRadius2() const
{
	IntPoint radius = m_End - GetCenter();
	return radius.GetLength2();
}

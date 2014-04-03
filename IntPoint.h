#pragma once

#include <cmath>

#pragma pack (push, 1)

template <typename CoordinateType>
struct CPoint
{
public:
	CPoint()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	CPoint(const CoordinateType& a_X, const CoordinateType& a_Y, const CoordinateType& a_Z)
	{
		x = a_X;
		y = a_Y;
		z = a_Z;
	}
	
	CPoint operator * (const CoordinateType& a_Value) const
	{
		return CPoint(x * a_Value, y * a_Value, z * a_Value);
	}

	CPoint operator * (const double& a_Value) const
	{
		return CPoint(x * a_Value, y * a_Value, z * a_Value);
	}
	
	CPoint operator / (const CoordinateType& a_Value) const
	{
		return CPoint(x / a_Value, y / a_Value, z / a_Value);
	}
	
	CPoint operator + (const CPoint& a_Value) const
	{
		return CPoint(x + a_Value.x, y + a_Value.y, z + a_Value.z);
	}
	
	CPoint operator - (const CPoint& a_Value) const
	{
		return CPoint(x - a_Value.x, y - a_Value.y, z - a_Value.z);
	}
	
	bool operator == (const CPoint& a_Value) const
	{
		return (x == a_Value.x) && (y == a_Value.y) && (z == a_Value.z);
	}

	bool operator >= (const CPoint& a_Value) const
	{
		return (x >= a_Value.x) && (y >= a_Value.y) && (z >= a_Value.z);
	}
	
	bool operator <= (const CPoint& a_Value) const
	{
		return (x <= a_Value.x) && (y <= a_Value.y) && (z <= a_Value.z);
	}
	
	double GetLength2() const
	{
		return (double(x) * x) + (double(y) * y) + (double(z) * z);
	}
	
	double GetLength() const
	{
		return sqrt(GetLength2());
	}
	
	CoordinateType x;
	CoordinateType y;
	CoordinateType z;
};

typedef CPoint<int> IntPoint;

template <typename CoordinateType>
class BBox
{
public:
	BBox(const CPoint<CoordinateType>& a_Start, const CPoint<CoordinateType>& a_End)
	{
		m_Start = a_Start;
		m_End = a_End;
	}
	
	CPoint<CoordinateType> GetCenter() const
	{
		return (m_End + m_Start) / 2; // CPoint((m_End.x + m_Start.x) >> 1, (m_End.y + m_Start.y) >> 1, (m_End.z + m_Start.z) >> 1);
	}

	double GetRadius2() const
	{
		CPoint<CoordinateType> radius = m_End - GetCenter();
		return radius.GetLength2();
	}
	
	double GetRadius() const
	{
		CPoint<CoordinateType> radius = m_End - GetCenter();
		return radius.GetLength();
	}
	
private:
	CPoint<CoordinateType> m_Start;
	CPoint<CoordinateType> m_End;
};

#pragma pack ( pop)
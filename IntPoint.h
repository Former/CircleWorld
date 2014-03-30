#pragma once

struct IntPoint
{
public:
	typedef int IntType;

	IntPoint();
	IntPoint(const IntType& a_X, const IntType& a_Y, const IntType& a_Z);
	
	IntPoint operator * (const IntType& a_Value) const;
	IntPoint operator / (const IntType& a_Value) const;
	IntPoint operator + (const IntPoint& a_Value) const;
	IntPoint operator - (const IntPoint& a_Value) const;
	bool operator == (const IntPoint& a_Value) const;

	bool operator >= (const IntPoint& a_Value) const;
	bool operator <= (const IntPoint& a_Value) const;
	
	IntType x;
	IntType y;
	IntType z;
};

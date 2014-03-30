#pragma once

struct IntPoint
{
public:
	typedef int IntType;

	IntPoint();
	IntPoint(const IntType& a_X, const IntType& a_Y, const IntType& a_Z);
	
	IntPoint operator * (const IntType& a_Value) const;
	IntPoint operator * (const double& a_Value) const;
	IntPoint operator / (const IntType& a_Value) const;
	IntPoint operator + (const IntPoint& a_Value) const;
	IntPoint operator - (const IntPoint& a_Value) const;
	bool operator == (const IntPoint& a_Value) const;

	bool operator >= (const IntPoint& a_Value) const;
	bool operator <= (const IntPoint& a_Value) const;
	
	double GetLength2() const;
	
	IntType x;
	IntType y;
	IntType z;
};

class BBox
{
public:
	BBox(const IntPoint& a_Start, const IntPoint& a_End);
	
	IntPoint GetCenter() const;

	double GetRadius2() const;
	
private:
	IntPoint m_Start;
	IntPoint m_End;
};
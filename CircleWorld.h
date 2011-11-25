#pragma once 

typedef double CoordinateType;
typedef float WeightType;

class Point
{
public:
	Point();
	Point(CoordinateType a_x, CoordinateType a_y, CoordinateType a_z);

	bool CheckConnect(const Point& a_OtherPoint, const CoordinateType& a_Distance) const;
	CoordinateType Distance2(const Point& a_OtherPoint) const;
	CoordinateType Distance(const Point& a_OtherPoint) const;

	CoordinateType operator * (const Point& a_Value) const; // Ñêàëÿðíîå ïðîèçâåäåíèå
	Point operator * (const CoordinateType& a_Value) const;
	Point operator / (const CoordinateType& a_Value) const;
	Point operator + (const Point& a_Value) const;
	Point operator - (const Point& a_Value) const;

	CoordinateType x;
	CoordinateType y;
	CoordinateType z;
};

class CircleObject
{
public:
	CircleObject();
	bool 			IsIntersection(const CircleObject& a_OtherObject) const; // Ïåðåñåêàþòñÿ ëè ýòè îáåêòû
	void 			ResolveContact(CircleObject* a_OtherObject);
	static void 	ResolveContact(CircleObject* a_OtherObject1, CircleObject* a_OtherObject2);

	Point			Velocity;
	Point			Center;
	CoordinateType	Radius;
	WeightType		Weight;
	bool 			IsFixed;
};



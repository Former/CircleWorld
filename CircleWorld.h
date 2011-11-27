#pragma once 

#include <vector>
#include <string>

typedef double CoordinateType;
typedef float WeightType;
typedef size_t COIndex;

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

	Point			Velocity;
	Point			Center;
	CoordinateType	Radius;
};

class CONear
{
public:
	CONear();
	
	Point 						LastCenter;
	CoordinateType				RadiusNearArea;
	std::vector<COIndex> 		NearObjects;
	std::vector<CircleObject>* 	Objects;
};

class CircleObjectMover
{
public:
	CircleObjectMover();	
	
	COIndex 		AddObject(CircleObject a_Object);
	CircleObject& 	GetObject(COIndex a_Index);
	std::vector<CircleObject>& GetObjects();
	
	void Move(const std::vector<COIndex>& a_Indexes, CoordinateType a_Accuracy);
	void Contact(const std::vector<COIndex>& a_FirstIndexes, const std::vector<COIndex>& a_SecondIndexes, bool a_IsFirstFixed);
	void NearContact(const std::vector<COIndex>& a_FirstIndexes, const std::vector<COIndex>& a_SecondIndexes, bool a_IsFirstFixed);
	void Gravity(const std::vector<COIndex>& a_Indexes, Point a_CenterGravity, CoordinateType a_Force, CoordinateType a_Accuracy);

private:
	static bool		IsIntersection(CircleObject* a_Object1, CircleObject* a_Object2);
	static void 	ResolveContact(CircleObject* a_Object1, CircleObject* a_Object2, bool a_IsObject1Fixed);

	std::vector<CircleObject> m_Objects;
	std::vector<CONear> m_NearObjects;
};

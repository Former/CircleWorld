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
	
private:
	Point 			m_LastCenter;
	CoordinateType	m_RadiusNearArea;
	std::vector<COIndex> m_NearObjects;
};

class CircleObjectMover
{
public:
	CircleObjectMover();	
	
	COIndex 		AddObject(CircleObject a_Object);
	CircleObject& 	GetObject(COIndex a_Index);
	std::vector<CircleObject>& GetObjects();
	
	void Move(std::vector<COIndex> a_Indexes, CoordinateType a_Accuracy);
	void Contact(std::vector<COIndex> a_Indexes, bool a_UseInernal);
	void ContactWithFirstFixed(std::vector<COIndex> a_FirstIndexes, std::vector<COIndex> a_Indexes, bool a_UseNear);
	void Gravity(std::vector<COIndex> a_Indexes, Point a_CenterGravity, CoordinateType a_Force, CoordinateType a_Accuracy);

private:
	static bool		IsIntersection(CircleObject* a_Object1, CircleObject* a_Object2);
	static void 	ResolveContact(CircleObject* a_Object1, CircleObject* a_Object2);

	std::vector<CircleObject> m_Objects;
}

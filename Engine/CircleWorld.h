#pragma once 

class CONear
{
public:
	CONear();
	
	Point 						LastCenter;
	CoordinateType				RadiusNearArea;
	std::vector<COIndex> 		NearObjects;
};

class CircleObjectMover
{
public:
	CircleObjectMover();	
	
	COIndex 		AddObject(CircleObject a_Object);
	CircleObject& 	GetObject(COIndex a_Index);
	std::vector<CircleObject>& GetObjects();
	
	void Move(const std::vector<COIndex>& a_Indexes, CoordinateType a_Accuracy);
	void Contact(const std::vector<COIndex>& a_FirstIndexes, const std::vector<COIndex>& a_SecondIndexes, bool a_IgnoreFixed = false);
	void NearContact(const std::vector<COIndex>& a_FirstIndexes, const std::vector<COIndex>& a_SecondIndexes, bool a_IgnoreFixed = false);
	void Gravity(const std::vector<COIndex>& a_Indexes, Point a_CenterGravity, CoordinateType a_Force, CoordinateType a_Accuracy);

private:
	bool IsNear(Point a_Center, CoordinateType a_Radius, CircleObject&  a_Obj);
	void CalculateNear(CONear* a_NearData, const std::vector<COIndex>& a_Indexes, COIndex a_CurIndex);

	static bool		IsIntersection(CircleObject* a_Object1, CircleObject* a_Object2);
	static void 	ResolveContact(CircleObject* a_Object1, CircleObject* a_Object2, bool a_IsObject1Fixed);

	std::vector<CircleObject> m_Objects;
	std::vector<CONear> m_NearObjects;
};

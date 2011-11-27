#include "CircleWorld.h"
#include <math.h>

#define abs(x) (((x) >= 0) ? (x) : (-(x)))

Point::Point()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

Point::Point(CoordinateType a_x, CoordinateType a_y, CoordinateType a_z)
{
	x = a_x;
	y = a_y;
	z = a_z;
}

bool Point::CheckConnect(const Point& a_OtherPoint, const CoordinateType& a_Distance) const
{
	const CoordinateType absX = abs(x - a_OtherPoint.x);
	const CoordinateType absY = abs(y - a_OtherPoint.y);
	const CoordinateType absZ = abs(z - a_OtherPoint.z);

	if (absX > a_Distance || absY > a_Distance || absY > a_Distance)
		return false;
	
	const CoordinateType dist2 = (absX * absX) + (absY * absY) + (absZ * absZ);

	return (dist2 < a_Distance * a_Distance);
}

CoordinateType Point::Distance2(const Point& a_OtherPoint) const
{
	return ((x - a_OtherPoint.x) * (x - a_OtherPoint.x) + (y - a_OtherPoint.y) * (y - a_OtherPoint.y) + (z - a_OtherPoint.z) * (z - a_OtherPoint.z));
}

CoordinateType Point::Distance(const Point& a_OtherPoint) const
{
	return sqrt(Distance2(a_OtherPoint)); 
}

CoordinateType Point::operator * (const Point& a_Value) const // Скаляное произведение
{
	return (x * a_Value.x + y * a_Value.y + z * a_Value.z); 
}


Point Point::operator * (const CoordinateType& a_Value) const
{
	return Point(x * a_Value, y * a_Value, z * a_Value); 
}

Point Point::operator / (const CoordinateType& a_Value) const
{
	return Point(x / a_Value, y / a_Value, z / a_Value); 
}

Point Point::operator + (const Point& a_Value) const
{
	return Point(x + a_Value.x, y + a_Value.y, z + a_Value.z); 
}

Point Point::operator - (const Point& a_Value) const
{
	return Point(x - a_Value.x, y - a_Value.y, z - a_Value.z); 
}


CircleObject::CircleObject()
{
	Radius = 1.0;
}

CONear::CONear()
{
	RadiusNearArea 	= 0.0;
}

CircleObjectMover::CircleObjectMover()
{	
}	

COIndex CircleObjectMover::AddObject(CircleObject a_Object)
{
	m_Objects.push_back(a_Object);
}

CircleObject& CircleObjectMover::GetObject(COIndex a_Index)
{
	return m_Objects[a_Index];
}

std::vector<CircleObject>& CircleObjectMover::GetObjects()
{
	return m_Objects;
}

void CircleObjectMover::Move(const std::vector<COIndex>& a_Indexes, CoordinateType a_Accuracy)
{
	for (size_t i = 0; i < a_Indexes.size(); i++)
	{
		COIndex objIndex = a_Indexes[i];
		CircleObject& obj = m_Objects[objIndex];
		
		obj.Center = obj.Center + obj.Velocity * a_Accuracy;
	}
}

void CircleObjectMover::Contact(const std::vector<COIndex>& a_FirstIndexes, const std::vector<COIndex>& a_SecondIndexes, bool a_IsFirstFixed)
{	
	bool isIndsEqual = (&a_FirstIndexes == &a_SecondIndexes);
	for (size_t i = 0; i < a_FirstIndexes.size(); i++)
	{
		COIndex obj1Index = a_FirstIndexes[i];		
		CircleObject& obj1 = m_Objects[obj1Index];
		
		for (size_t j = (isIndsEqual ? i : 0); j < a_SecondIndexes.size(); j++)
		{
			COIndex obj2Index = a_SecondIndexes[j];		
			if (obj1Index == obj2Index)
				continue;

			CircleObject& obj2 = m_Objects[obj2Index];

			if (IsIntersection(&obj1, &obj2))
				ResolveContact(&obj1, &obj2, a_IsFirstFixed);
		}
	}
}

bool CircleObjectMover::IsNear(Point a_Center, CoordinateType a_Radius, Point a_Point)
{
	if ((a_Point.x < a_Center.x - a_Radius) || (a_Point.x > a_Center.x + a_Radius))
		return false;
	if ((a_Point.y < a_Center.y - a_Radius) || (a_Point.y > a_Center.y + a_Radius))
		return false;
	if ((a_Point.z < a_Center.z - a_Radius) || (a_Point.z > a_Center.z + a_Radius))
		return false;
	return true;
}

void CircleObjectMover::CalculateNear(CONear* a_NearData, const std::vector<COIndex>& a_Indexes, COIndex a_CurIndex)
{
	a_NearData->NearObjects.clear();
	for (size_t j = 0; j < a_Indexes.size(); j++)
	{
		COIndex objIndex = a_Indexes[j];
		if (objIndex == a_CurIndex)
			continue;

		CircleObject& obj = m_Objects[objIndex];

		if (IsNear(a_NearData->LastCenter, a_NearData->RadiusNearArea, obj.Center))
			a_NearData->NearObjects.push_back(objIndex);
	}
}

void CircleObjectMover::NearContact(const std::vector<COIndex>& a_FirstIndexes, const std::vector<COIndex>& a_SecondIndexes, bool a_IsFirstFixed)
{
	if (m_NearObjects.size() != m_Objects.size())
		m_NearObjects.resize(m_Objects.size());

	for (size_t i = 0; i < a_FirstIndexes.size(); i++)
	{
		COIndex obj1Index = a_FirstIndexes[i];		
		CircleObject& obj1 = m_Objects[obj1Index];
		CONear& near = m_NearObjects[obj1Index];
		
		if (near.RadiusNearArea == 0.0 || !IsNear(obj1.Center, near.RadiusNearArea / 2, near.LastCenter))
		{
			near.LastCenter = obj1.Center;
			near.RadiusNearArea = obj1.Radius * 10.0;
			CalculateNear(&near, a_SecondIndexes, obj1Index);	
		}
		
		for (size_t j = 0; j < near.NearObjects.size(); j++)
		{
			COIndex obj2Index = near.NearObjects[j];
			if (obj1Index == obj2Index)
				continue;

			CircleObject& obj2 = m_Objects[obj2Index];

			if (IsIntersection(&obj1, &obj2))
				ResolveContact(&obj1, &obj2, a_IsFirstFixed);
		}
	}
}

void CircleObjectMover::Gravity(const std::vector<COIndex>& a_Indexes, Point a_CenterGravity, CoordinateType a_Force, CoordinateType a_Accuracy)
{
	for (size_t i = 0; i < a_Indexes.size(); i++)
	{
		COIndex objIndex = a_Indexes[i];
		CircleObject& obj = m_Objects[objIndex];
		
		Point& circleCentr = obj.Center;

		CoordinateType dist = a_CenterGravity.Distance(circleCentr);
		CoordinateType onedivdist = 0.0;
		if (dist > 0.0)
			onedivdist = 1.0 / dist;

		if (dist > 0.1)
		{
			Point accelerationVector = (a_CenterGravity - circleCentr) * onedivdist;
			obj.Velocity = obj.Velocity + accelerationVector * onedivdist * onedivdist * a_Force * a_Accuracy; // 250;
		}

	}
}

bool	CircleObjectMover::IsIntersection(CircleObject* a_Object1, CircleObject* a_Object2)
{
	return a_Object1->Center.CheckConnect(a_Object2->Center, a_Object2->Radius + a_Object1->Radius);	
}

void CircleObjectMover::ResolveContact(CircleObject* a_Object1, CircleObject* a_Object2, bool a_IsObject1Fixed)
{
	const CoordinateType zapas = 0.01;
	
	Point& centr1 = a_Object1->Center;
	Point& centr2 = a_Object2->Center;
	Point centrWeight = (centr1 + centr2) * 0.5;
	
	CoordinateType dist = centr1.Distance(centr2);
	CoordinateType onedivdist = 0.0;
	if (dist > 0.0)
		onedivdist = 1.0 / dist;

	// Ñòàëêèâàíèå (çàêîí ñîõðàíåíèÿ èìïóëüñà)
	Point normVector = (centr2 - centr1) * onedivdist;
	
	Point& vel1 = a_Object1->Velocity;
	Point& vel2 = a_Object2->Velocity;
	
	Point vel2Paralell	= normVector * (normVector * vel2);
	Point vel2Perp		= vel2 - vel2Paralell;

	if (a_IsObject1Fixed)
	{
		if ((vel2Paralell * normVector) < 0)
			vel2 = vel2Perp - vel2Paralell;
		else
			vel2 = vel2Perp + vel2Paralell;
		centr2 = normVector * ((a_Object2->Radius + a_Object1->Radius) * (1.0 + zapas)) + centr1;		
		return;
	}

	Point vel1Paralell	= normVector * (normVector * vel1);
	Point vel1Perp		= vel1 - vel1Paralell;

	if ((vel1Paralell * normVector) > 0 || (vel2Paralell * normVector) < 0)
	{
		vel1 = vel1Perp + vel2Paralell;
		vel2 = vel2Perp + vel1Paralell;
	}
	
	// Îòäàëåíèå îêðóæíîñòåé äðóã îò äðóãà
	centr1 = normVector * (-a_Object1->Radius * (1.0 + zapas)) + centrWeight;
	centr2 = normVector * (+a_Object2->Radius * (1.0 + zapas)) + centrWeight;
}

//const CoordinateType accuraty = 0.01;


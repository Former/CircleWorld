#include "CircleWorld.h"


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

void CircleObjectMover::Contact(const std::vector<COIndex>& a_FirstIndexes, const std::vector<COIndex>& a_SecondIndexes, bool a_IgnoreFixed)
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

			if (!IsIntersection(&obj1, &obj2))
				continue;
			
			// @@@ ToDo Fix copy paste
			if (a_IgnoreFixed || (!obj1.IsFixed && !obj2.IsFixed))
			{
				ResolveContact(&obj1, &obj2, false);
				continue;
			}	
			
			if (obj2.IsFixed && obj1.IsFixed)
				continue;
			
			if (obj2.IsFixed)
				ResolveContact(&obj2, &obj1, true);
			else
				ResolveContact(&obj1, &obj2, true);
		}
	}
}

bool CircleObjectMover::IsNear(Point a_Center, CoordinateType a_Radius, CircleObject& a_Object)
{
	const Point centerMinusObjCenter = a_Center - a_Object.Center;
	const CoordinateType radiusPlusObjRadius = a_Object.Radius + a_Radius;
	if (centerMinusObjCenter.x > radiusPlusObjRadius || centerMinusObjCenter.x < -radiusPlusObjRadius )
		return false;
	if (centerMinusObjCenter.y > radiusPlusObjRadius || centerMinusObjCenter.y < -radiusPlusObjRadius )
		return false;
	if (centerMinusObjCenter.z > radiusPlusObjRadius || centerMinusObjCenter.z < -radiusPlusObjRadius )
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

		if (IsNear(a_NearData->LastCenter, a_NearData->RadiusNearArea, obj))
			a_NearData->NearObjects.push_back(objIndex);
	}
}

void CircleObjectMover::NearContact(const std::vector<COIndex>& a_FirstIndexes, const std::vector<COIndex>& a_SecondIndexes, bool a_IgnoreFixed)
{
	if (m_NearObjects.size() != m_Objects.size())
		m_NearObjects.resize(m_Objects.size());

	for (size_t i = 0; i < a_FirstIndexes.size(); i++)
	{
		COIndex obj1Index = a_FirstIndexes[i];		
		CircleObject& obj1 = m_Objects[obj1Index];
		CONear& near = m_NearObjects[obj1Index];
		
		if (near.RadiusNearArea == 0.0 || !IsNear(near.LastCenter, near.RadiusNearArea / 2, obj1))
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

			if (!IsIntersection(&obj1, &obj2))
				continue;
			
			if (a_IgnoreFixed || (!obj1.IsFixed && !obj2.IsFixed))
			{
				ResolveContact(&obj1, &obj2, false);
				continue;
			}	
			
			if (obj1.IsFixed && obj2.IsFixed)
				continue;
			
			if (obj2.IsFixed)
				ResolveContact(&obj2, &obj1, true);
			else
				ResolveContact(&obj1, &obj2, true);
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
	const CoordinateType weight1 = a_Object1->Weight;
	const CoordinateType weight2 = a_Object1->Weight;
	
	Point centrWeight = (centr1 * weight1 + centr2 * weight2) / (weight1 + weight2);
	
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


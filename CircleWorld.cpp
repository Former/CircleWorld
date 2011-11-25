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

CoordinateType Point::operator * (const Point& a_Value) const // Ñêàëÿðíîå ïðîèçâåäåíèå
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
	Radius 	= 0.0;
	Weight 	= 0.0;
	IsFixed = false;
}

bool CircleObject::IsIntersection(const CircleObject& a_OtherObject) const // Ïåðåñåêàþòñÿ ëè ýòè îáåêòû
{
	return a_OtherObject.Center.CheckConnect(Center, Radius + a_OtherObject.Radius);
}

void CircleObject::ResolveContact(CircleObject* a_OtherObject) 
{
	ResolveContact(this, a_OtherObject);
}

void CircleObject::ResolveContact(CircleObject* a_Object1, CircleObject* a_Object2) 
{
	const CoordinateType zapas = 0.01;
	
	if (a_Object1->IsFixed && a_Object2->IsFixed)
		return;
		
	if (a_Object1->IsFixed)
	{
		ResolveContact(a_Object2, a_Object1);
		return;
	}
	
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
	
	CoordinateType cosa1 = (normVector * vel1);
	Point vel1Paralell	= normVector * (normVector * vel1);
	Point vel1Perp		= vel1 - vel1Paralell;

	if (a_Object2->IsFixed)
	{
		if ((vel1Paralell * normVector) > 0)
			vel1 = vel1Perp - vel1Paralell;
		else
			vel1 = vel1Perp + vel1Paralell;
		centr1 = normVector * (-(a_Object1->Radius + a_Object2->Radius) * (1.0 + zapas)) + centr2;		
		return;
	}

	CoordinateType cosa2 = (normVector * vel2);
	Point vel2Paralell	= normVector * (normVector * vel2);
	Point vel2Perp		= vel2 - vel2Paralell;

	if ((vel1Paralell * normVector) > 0 || (vel2Paralell * normVector) < 0)
	{
		vel1 = vel1Perp + vel2Paralell;
		vel2 = vel2Perp + vel1Paralell;
	}
	
	// Îòäàëåíèå îêðóæíîñòåé äðóã îò äðóãà
	centr1 = normVector * (-a_Object1->Radius * (1.0 + zapas)) + centrWeight;
	centr2 = normVector * (+a_Object2->Radius * (1.0 + zapas)) + centrWeight;
}



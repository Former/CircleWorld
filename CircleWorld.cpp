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

Point Point::operator * (const Point& a_Value) const // Ñêàëÿðíîå ïðîèçâåäåíèå
{
	return Point(x * a_Value.x, y * a_Value.y, z * a_Value.z); 
}


Point Point::operator * (const double& a_Value) const
{
	return Point(x * a_Value, y * a_Value, z * a_Value); 
}

Point Point::operator / (const double& a_Value) const
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
	Radius = 0.0;
	Weight = 0.0;
}

bool CircleObject::IsIntersection(const CircleObject& a_OtherObject) const // Ïåðåñåêàþòñÿ ëè ýòè îáåêòû
{
	return a_OtherObject.Center.CheckConnect(Center, Radius + a_OtherObject.Radius);
}

void CircleObject::ResolveContact(CircleObject* a_OtherObject) 
{
	Point& otherCentr = a_OtherObject->Center;
	Point centrWeight = (Center + otherCentr) * 0.5;
	
	CoordinateType dist = otherCentr.Distance(Center);
	CoordinateType onedivdist = 0.0;
	if (dist > 0.0)
		onedivdist = 1.0 / dist;

	// Ñòàëêèâàíèå (çàêîí ñîõðàíåíèÿ èìïóëüñà)
	Point normVector = (otherCentr - Center) * onedivdist;
	
	Point velParalell	= normVector * (normVector * Velocity);
	Point velPerp		= Velocity - velParalell;

	Point velOtherParalell	= normVector * (normVector * a_OtherObject->Velocity);
	Point velOtherPerp		= a_OtherObject->Velocity - velOtherParalell;

	Velocity = velPerp + velOtherParalell;
	a_OtherObject->Velocity = velOtherPerp + velParalell;
	
	// Îòäàëåíèå îêðóæíîñòåé äðóã îò äðóãà
	CoordinateType distanceDivRadius = 1.01 * dist / (Radius + a_OtherObject->Radius);

	CoordinateType oneDivDistCentr = onedivdist * 2.0; //Center.Distance(centrWeight);
	Center = (Center - centrWeight) * oneDivDistCentr * distanceDivRadius + centrWeight;

	CoordinateType oneDivDistOtherCentr = oneDivDistCentr; // otherCentr.Distance(centrWeight);
	otherCentr = (otherCentr - centrWeight) * oneDivDistCentr * distanceDivRadius + centrWeight;
}



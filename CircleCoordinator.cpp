#include "CircleCoordinator.h"

CircleCoordinator::ObjectColor::ObjectColor(float a_Red, float a_Green, float a_Blue, float a_Alpha)
{
	red 	= a_Red;
	green 	= a_Green;
	blue 	= a_Blue;
	alpha 	= a_Alpha;
}	

CircleCoordinator::Object::Object()
:	Color(1, 1, 1, 1.0)
{
	Detal 	= 10;
}	

CircleCoordinator::CircleCoordinator()
{
	m_Scale = 10.0;
}

void CircleCoordinator::AddObject(const ObjectPtr& a_Object, irr::video::IVideoDriver* a_Driver, irr::scene::ISceneManager* a_SMgr)
{
/*	irr::scene::ISceneNode* sphere_node = a_SMgr->addSphereSceneNode(m_Scale * a_Object->Obj->Radius, a_Object->Detal);
    if (sphere_node)
    {
		sphere_node->setPosition(irr::core::vector3df(0,0,0));
		sphere_node->setMaterialTexture(0, a_Driver->getTexture("../../media/wall.jpg"));
		sphere_node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    }

	a_Object->IrrObject = sphere_node;*/
	a_Object->IrrObject = 0;
	m_Objects[a_Object->Obj->GetID()] = a_Object;
}

std::vector<CircleCoordinator::ObjectPtr> CircleCoordinator::GetObjects()
{
	std::vector<ObjectPtr> result; 
	
	for (ObjMap::iterator it = m_Objects.begin(); it != m_Objects.end(); it++)
		result.push_back(it->second); 
	
	return result;
}

void CircleCoordinator::AddRule(CircleEngine::RulePtr a_Rule)
{
	m_Rules.push_back(a_Rule);
}

void CircleCoordinator::DoStep()
{
	for(size_t i = 0; i < m_Rules.size(); i++)
	{
		m_Rules[i]->DoStep();
	}
}	

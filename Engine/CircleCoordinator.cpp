#include "CircleCoordinator.h"

namespace CircleEngine
{
	CircleCoordinator::ObjectColor::ObjectColor(float a_Red, float a_Green, float a_Blue)
	{
		red 	= a_Red;
		green 	= a_Green;
		blue 	= a_Blue;
	}	

	CircleCoordinator::Object::Object()
	:	Color(1, 1, 1)
	{
		Detal 	= 10;
	}	
	
	CircleCoordinator::CircleCoordinator()
	{
	}
	
	void CircleCoordinator::AddObject(const ObjectPtr& a_Object)
	{
		m_Objects[a_Object->Obj->GetID()] = a_Object;
	}
	
	std::vector<CircleCoordinator::ObjectPtr> CircleCoordinator::GetObjects()
	{
		std::vector<ObjectPtr> result; 
		
		for (ObjMap::iterator it = m_Objects.begin(); it != m_Objects.end(); it++)
			result.push_back(it->second); 
		
		return result;
	}

	void CircleCoordinator::AddRule(RulePtr a_Rule)
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
	
	////////////////////////////////////////////////////////////////////////
	
	class LuaTestParentFunction
	{
	public:
		LuaTestParentFunction(LuaScript* a_Parent)
		{
			m_Parent = static_cast<TestLuaScript*>(a_Parent);
		}

		static const LuaScript::LuaArgArray* GetInputArgs()
		{
			LuaScript::LuaArgArray* args = new LuaScript::LuaArgArray();
			args->Add(new LuaScript::Int_LuaArg());
			return args;
		}

		static const LuaScript::LuaArgArray* GetOutputArgs()
		{
			LuaScript::LuaArgArray* args = new LuaScript::LuaArgArray();
			args->Add(new LuaScript::Bool_LuaArg());
			return args;
		}

		static const std::string NameSpace() { return "engine"; }
		static const std::string Name() { return "check_parent"; }

		void Calc(const LuaScript::LuaArgArray& in, LuaScript::LuaArgArray& out) 
		{
		if( !m_Parent )
		{
		  dynamic_cast<LuaScript::Bool_LuaArg&>(*out[0]).SetValue(false);
		  return;
		}
		m_Parent->m_TestParametr =
				dynamic_cast<LuaScript::Int_LuaArg&>(*in[0]).GetValue();
		dynamic_cast<LuaScript::Bool_LuaArg&>(*out[0]).SetValue(true);
		}
	 private:
	  TestLuaScript* m_Parent;
	};
	
}
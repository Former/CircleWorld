// Copyright (c) 2009-2011 by Alexander Demin and Alexei Bezborodov

#ifndef _LUASCRIPT_H
#define _LUASCRIPT_H

#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <exception>
#include <algorithm>
#include <memory>

extern "C" 
{
  #include "lua/lua.h"
  #include "lua/lualib.h"
  #include "lua/lauxlib.h"
}

#define IntToPointer(a_Integer) reinterpret_cast<void*>(a_Integer)
#if defined(__x86_64__) || defined(__amd64__)
#  define PointerToInt(a_Pointer) reinterpret_cast<long long>(a_Pointer)
#else
#  define PointerToInt(a_Pointer) reinterpret_cast<long>(a_Pointer)
#endif

class LuaScript
{
 public:
  LuaScript();
  ~LuaScript();

  class iLuaArg
  {
   public:
    virtual             ~iLuaArg() {}

    virtual void        Unpack(lua_State* a_LuaState, int a_ParamIndex) = 0;
    virtual void        Pack(lua_State* a_LuaState) = 0;
    virtual std::string AsString() const = 0;
    virtual iLuaArg*    Clone() const = 0;
  };

  class Bool_LuaArg: public iLuaArg
  {
   public:
    typedef bool LuaValueType;

    Bool_LuaArg() : m_Value(false) { }
    Bool_LuaArg(bool a_Value) : m_Value(a_Value) { }

    virtual iLuaArg*    Clone() const;
    virtual void        Unpack(lua_State* a_LuaState, int a_ParamIndex);
    virtual void        Pack(lua_State* a_LuaState);
    std::string         AsString() const;
    const bool&         GetValue() const;
    void                SetValue(bool a_Value);

   private:
    bool m_Value;
  };

  template<class IntType>
  class Integer_LuaArg: public iLuaArg
  {
   public:
    typedef IntType LuaValueType;

    Integer_LuaArg() : m_Value(0) { }
    Integer_LuaArg(LuaValueType a_Value) : m_Value(a_Value) { }

    virtual iLuaArg*    Clone() const;
    virtual void        Unpack(lua_State* a_LuaState, int a_ParamIndex);
    virtual void        Pack(lua_State* a_LuaState);
    std::string         AsString() const;
    const IntType&      GetValue() const;
    void                SetValue(LuaValueType a_Value);

   private:
    LuaValueType m_Value;
  };

  class Double_LuaArg: public iLuaArg
  {
   public:
    typedef double LuaValueType;

    Double_LuaArg() : m_Value(0) { }
    Double_LuaArg(LuaValueType a_Value) : m_Value(a_Value) { }

    virtual iLuaArg*    Clone() const;
    virtual void        Unpack(lua_State* a_LuaState, int a_ParamIndex);
    virtual void        Pack(lua_State* a_LuaState);
    std::string         AsString() const;
    const LuaValueType& GetValue() const;
    void                SetValue(LuaValueType a_Value);

   private:
    LuaValueType m_Value;
  };
  
  class String_LuaArg: public iLuaArg
  {
   public:
    typedef std::string LuaValueType;

    String_LuaArg() : m_Value() {}
    String_LuaArg(const LuaValueType& a_Value) : m_Value(a_Value) {}

    virtual iLuaArg*    Clone() const;
    virtual void        Unpack(lua_State* a_LuaState, int a_ParamIndex);
    virtual void        Pack(lua_State* a_LuaState);
    std::string         AsString() const;
    const LuaValueType& GetValue() const;
    void                SetValue(LuaValueType a_Value);

   private:
    LuaValueType m_Value;
  };

  template<class LuaArgType>
  class Vector_LuaArg: public iLuaArg
  {
   public:
    typedef typename LuaArgType::LuaValueType VectorType;
    typedef std::vector<VectorType> LuaValueType;

    Vector_LuaArg() : m_Value() {}
    Vector_LuaArg(const LuaValueType& a_Value) : m_Value(a_Value) {}

    virtual iLuaArg*    Clone() const;
    virtual void        Unpack(lua_State* a_LuaState, int a_ParamIndex);
    virtual void        Pack(lua_State* a_LuaState);
    std::string         AsString() const;
    const LuaValueType& GetValue() const
    {
      return m_Value; 
    }
    void                SetValue(LuaValueType a_Value);

   private:
    LuaValueType m_Value;
  };
  
  typedef Integer_LuaArg<int>  Int_LuaArg;
  typedef Double_LuaArg        Ptr_LuaArg;

  typedef Vector_LuaArg<Bool_LuaArg>   VectorBool_LuaArg;
  typedef Vector_LuaArg<Int_LuaArg>    VectorInt_LuaArg;
  typedef Vector_LuaArg<String_LuaArg> VectorString_LuaArg;
  typedef Vector_LuaArg<Double_LuaArg> VectorDouble_LuaArg;
  
  class LuaArgArray: public std::deque<iLuaArg*>
  {
   public:
    LuaArgArray() { clear(); }
    LuaArgArray(const LuaArgArray& rhs);
    virtual ~LuaArgArray();

    LuaArgArray*  Clone() const;
    void          PopFromLua(lua_State* a_LuaState);
    void          PushToLua(lua_State* a_LuaState);
    LuaArgArray&  Add(iLuaArg* arg);
  };

  template<class LuaImplFuncType>
  static int LuaCallback(lua_State* a_LuaState);

  class LuaException : public std::exception 
  {
   public:
    explicit LuaException(const std::string& a_Message);
    virtual ~LuaException() throw() { }
    
    const char*        What()  const throw() { return m_Message.c_str(); }
    int                Line()  const throw() { return m_Line; }
    const std::string& Error() const throw() { return m_Error; }

   private:
    std::string m_Message;
    int         m_Line;
    std::string m_Error;
  };

  void       Execute(const std::string& a_ScriptString);

  template<class LuaArgType>
  LuaArgType GetVariable(const std::string& a_Name);

  template<class LuaArgType>
  void       SetVariable(const std::string& a_Name, 
                       const typename LuaArgType::LuaValueType& a_Value);

  template<class LuaArgType>
  void       RegisterFunction();

 protected:
  LuaScript(const LuaScript&);
  void operator = (const LuaScript&);
  lua_State* m_LuaState;
 
 private:
  class LuaDeleter
  {
  public:
    template <typename LuaArgType> 
    void operator() (const LuaArgType* a_Item) const 
    {
      delete a_Item;
    }
  };
};

template<class LuaImplFuncType>
class LuaFunc 
{
 public:
  static const LuaScript::LuaArgArray* GetInputArgs() 
  {
    static LuaScript::LuaArgArray* args = 0;
    if( !args )
    {
      args = const_cast<LuaScript::LuaArgArray *>
                                (LuaImplFuncType::GetInputArgs());
      if( !LuaImplFuncType::NameSpace().empty() )
        args->push_front(new LuaScript::Ptr_LuaArg());
    }
    return args;
  }

  static const LuaScript::LuaArgArray* GetOutputArgs()
  {
    static LuaScript::LuaArgArray* args = 0;
    if( !args )
      args = const_cast<LuaScript::LuaArgArray *>
                                (LuaImplFuncType::GetOutputArgs());
    return args;
  }

  static const std::string NameSpace()
  {
    return LuaImplFuncType::NameSpace();
  }
  
  static const std::string Name()
  {
    return LuaImplFuncType::Name();
  }

  static void Calc(const LuaScript::LuaArgArray& in,
                            LuaScript::LuaArgArray& out)
  {
    LuaImplFuncType::Calc(in, out);
  }
};

template<class LuaImplFuncType>
int LuaScript::LuaCallback(lua_State* a_LuaState) 
{
  const std::string sNameSpace  = LuaFunc<LuaImplFuncType>::NameSpace();
  const std::string sName       = LuaFunc<LuaImplFuncType>::Name();

  int argCount = lua_gettop(a_LuaState);
  int inputArgCount =
        static_cast<int>(LuaFunc<LuaImplFuncType>::GetInputArgs()->size());
  if( inputArgCount != argCount ) 
  {
    int reqArgCount = (sNameSpace.empty() ? inputArgCount : inputArgCount - 1);
    int gevenArgCount = (sNameSpace.empty() ? argCount : argCount - 1);
    std::stringstream fmt;
    fmt << "function '" << sName << "'"
      << " requires " << reqArgCount
      << " arguments, but " << gevenArgCount << " given";
    throw LuaScript::LuaException(fmt.str());
  }

  std::auto_ptr<LuaScript::LuaArgArray>
              inArgs(LuaFunc<LuaImplFuncType>::GetInputArgs()->Clone());
  inArgs->PopFromLua(a_LuaState);
  std::auto_ptr<LuaScript::LuaArgArray>
              outArgs(LuaFunc<LuaImplFuncType>::GetOutputArgs()->Clone());

  LuaScript* parent = 0;
  if( !sNameSpace.empty() )
  {
    LuaScript::Ptr_LuaArg firstParametr = 
                  dynamic_cast<LuaScript::Ptr_LuaArg&>(*inArgs->at(0));
    intptr_t ptr = static_cast<intptr_t>(firstParametr.GetValue());
    parent = static_cast<LuaScript*>(IntToPointer(ptr));
    std::for_each(inArgs->begin(), inArgs->begin() + 1, LuaDeleter());
	inArgs->pop_front();
  }

  LuaImplFuncType funcClass(parent);
  {
    funcClass.Calc(*inArgs, *outArgs);
  }
  outArgs->PushToLua(a_LuaState);
  return static_cast<int>(outArgs->size());
}

template<class LuaImplFuncType>
void LuaScript::RegisterFunction() 
{
  const std::string sNameSpace  = LuaFunc<LuaImplFuncType>::NameSpace();
  const std::string sName       = LuaFunc<LuaImplFuncType>::Name();
  if( sNameSpace.empty() )
  {
    lua_register(m_LuaState, sName.c_str(), LuaCallback<LuaImplFuncType>);
    return;
  }
  
  std::stringstream strThis;
  strThis << PointerToInt(this);

  std::string script = 
    std::string("if ") + sNameSpace + " == nil or " + 
                         sNameSpace + ".m_This == 0 then " + 
    sNameSpace + " = { m_This = " + strThis.str() + " }; "
    "end";
  Execute(script.c_str());

  lua_register(m_LuaState, "t_CurFunction", LuaCallback<LuaImplFuncType>);

  script = 
    sNameSpace + "." + sName + "_CppFunction = t_CurFunction; "
    "t_CurFunction = nil;";
  Execute(script.c_str());

  script = 
    std::string("function ") + sNameSpace + "." + sName + "(...) "
    "  local curArg = {}; "
    "  local sArgs = ''; "
    "  for ind, val in ipairs(arg) "
    "  do "
    "    curArg[ind] = val; "
    "    sArgs = sArgs..', curArg['..ind..']'; "
    "  end "
    "  return loadstring( "
    "   'return function (curArg) "
        "  return " + sNameSpace + "." + sName + "_CppFunction(" + 
              sNameSpace + ".m_This'..sArgs..' ); "
        "end'"
    "  )()(curArg); "
    "end ";
  
  Execute(script.c_str());
}

template<class LuaArgType>
LuaArgType LuaScript::GetVariable(const std::string& a_Name) 
{
  lua_getglobal(m_LuaState, a_Name.c_str());
  LuaArgType val;
  int index = lua_gettop(m_LuaState);
  val.Unpack(m_LuaState, index);
  lua_pop(m_LuaState, index);
  return val;
}

template<class LuaArgType>
void LuaScript::SetVariable(const std::string& a_Name,
                        const typename LuaArgType::LuaValueType& a_Value) 
{
  LuaArgType var(a_Value);
  var.Pack(m_LuaState);
  lua_setglobal(m_LuaState, a_Name.c_str());
}

////////////////////////////////////////////////////////////////////////////////

template<class IntType>
LuaScript::iLuaArg* LuaScript::Integer_LuaArg<IntType>::Clone() const
{ 
  return new Integer_LuaArg<IntType>(m_Value);
}

template<class IntType>
void LuaScript::Integer_LuaArg<IntType>::
  Unpack(lua_State* a_LuaState, int a_ParamIndex)
{
  if( lua_isnumber(a_LuaState, a_ParamIndex) )
    m_Value = LuaValueType(lua_tointeger(a_LuaState, a_ParamIndex));
  else
    throw 
	  LuaException("Integer_LuaArg<IntType>::Unpack(), value is not integer");
}

template<class IntType>
void LuaScript::Integer_LuaArg<IntType>::Pack(lua_State* a_LuaState)
{
  lua_pushinteger(a_LuaState, m_Value);
}

template<class IntType>
std::string LuaScript::Integer_LuaArg<IntType>::AsString() const
{
  std::stringstream fmt;
  fmt << m_Value;
  return fmt.str();
}

template<class IntType>
const IntType& 
LuaScript::Integer_LuaArg<IntType>::GetValue() const
{ 
  return m_Value; 
}

template<class IntType>
void 
LuaScript::Integer_LuaArg<IntType>::SetValue(LuaValueType a_Value)
{
  m_Value = a_Value;
}

////////////////////////////////////////////////////////////////////////////////

template<class LuaArgType>
LuaScript::iLuaArg* LuaScript::Vector_LuaArg<LuaArgType>::Clone() const
{
  return new Vector_LuaArg<LuaArgType>(m_Value);
}

template<class LuaArgType>
void LuaScript::Vector_LuaArg<LuaArgType>::Unpack(lua_State* a_LuaState,
                                                int a_ParamIndex)
{
  if( !lua_istable(a_LuaState, a_ParamIndex) )
    throw LuaException("Vector_LuaArg::Unpack(), value is not table");

  lua_pushvalue(a_LuaState, a_ParamIndex);
  
  m_Value.clear();
  const int count = luaL_getn(a_LuaState, -1);
  for( int i = 1; i <= count; ++i )
  {
    lua_pushnumber(a_LuaState, i);
    lua_gettable(a_LuaState, -2);

    LuaArgType newValue;
    newValue.Unpack(a_LuaState, -1);

    m_Value.push_back(newValue.GetValue());
    lua_pop(a_LuaState, 1);
  }
  lua_pop(a_LuaState, 1);
}

template<class LuaArgType>
void LuaScript::Vector_LuaArg<LuaArgType>::Pack(lua_State* a_LuaState)
{
  lua_newtable(a_LuaState);
  const size_t size = m_Value.size();
  for( size_t i = 0; i < m_Value.size() ; ++i )
  {
    lua_pushnumber(a_LuaState, i + 1);
    LuaArgType val(m_Value[i]);
    val.Pack(a_LuaState);
    lua_settable(a_LuaState, -3);
  }
}

template<class LuaArgType>
std::string LuaScript::Vector_LuaArg<LuaArgType>::AsString() const
{
  std::stringstream fmt;
  fmt << &m_Value;
  return fmt.str();
}

template<class LuaArgType>
void LuaScript::Vector_LuaArg<LuaArgType>::SetValue(LuaValueType a_Value)
{
  m_Value = a_Value;
}

#endif
// Copyright (c) 2009-2011 by Alexander Demin and Alexei Bezborodov
#include "stdafx.h"

#include "luascript/luascript.h"
#include <sstream>
#include <iostream>

LuaScript::LuaScript() 
{
  m_LuaState = lua_open();
  luaL_openlibs(m_LuaState);
}

LuaScript::~LuaScript() 
{
  lua_close(m_LuaState); 
}

////////////////////////////////////////////////////////////////////////////////

LuaScript::iLuaArg* LuaScript::Bool_LuaArg::Clone() const
{ 
  return new Bool_LuaArg(m_Value);
}

void LuaScript::Bool_LuaArg::Unpack(lua_State* a_LuaState, int a_ParamIndex)
{
  if( lua_isboolean(a_LuaState, a_ParamIndex) )
    m_Value = lua_toboolean(a_LuaState, a_ParamIndex) ? true : false;
  else
    throw LuaException("Bool_LuaArg::Unpack(), value is not boolean");
}

void LuaScript::Bool_LuaArg::Pack(lua_State* a_LuaState)
{
  lua_pushboolean(a_LuaState, m_Value);
}

std::string LuaScript::Bool_LuaArg::AsString() const
{
  std::stringstream fmt;
  fmt << m_Value;
  return fmt.str();
}

const LuaScript::Bool_LuaArg::LuaValueType& 
LuaScript::Bool_LuaArg::GetValue() const
{ 
  return m_Value; 
}

void 
LuaScript::Bool_LuaArg::SetValue(LuaValueType a_Value)
{
  m_Value = a_Value;
}

////////////////////////////////////////////////////////////////////////////////

LuaScript::iLuaArg* LuaScript::Double_LuaArg::Clone() const
{ 
  return new Double_LuaArg(m_Value);
}

void LuaScript::Double_LuaArg::Unpack(lua_State* a_LuaState, int a_ParamIndex)
{
  if( lua_isnumber(a_LuaState, a_ParamIndex) )
    m_Value = LuaValueType(lua_tonumber(a_LuaState, a_ParamIndex));
  else
    throw LuaException("Double_LuaArg::Unpack(), value is not double");
}

void LuaScript::Double_LuaArg::Pack(lua_State* a_LuaState)
{
  lua_pushnumber(a_LuaState, m_Value);
}

std::string LuaScript::Double_LuaArg::AsString() const
{
  std::stringstream fmt;
  fmt << m_Value;
  return fmt.str();
}

const LuaScript::Double_LuaArg::LuaValueType& 
LuaScript::Double_LuaArg::GetValue() const
{ 
  return m_Value; 
}

void 
LuaScript::Double_LuaArg::SetValue(LuaValueType a_Value)
{
  m_Value = a_Value;
}

////////////////////////////////////////////////////////////////////////////////

LuaScript::iLuaArg* LuaScript::String_LuaArg::Clone() const
{ 
  return new String_LuaArg(m_Value);
}

void LuaScript::String_LuaArg::Unpack(lua_State* a_LuaState, int a_ParamIndex)
{
  if( lua_isstring(a_LuaState, a_ParamIndex) )
    m_Value = lua_tostring(a_LuaState, a_ParamIndex);
  else
    throw LuaException("String_LuaArg::Unpack(), value is not string");
}

void LuaScript::String_LuaArg::Pack(lua_State* a_LuaState)
{
  lua_pushstring(a_LuaState, m_Value.c_str());
}

std::string LuaScript::String_LuaArg::AsString() const
{
  return m_Value;
}

const LuaScript::String_LuaArg::LuaValueType& 
LuaScript::String_LuaArg::GetValue() const
{ 
  return m_Value; 
}

void LuaScript::String_LuaArg::SetValue(LuaValueType a_Value)
{
  m_Value = a_Value;
}

////////////////////////////////////////////////////////////////////////////////

LuaScript::LuaArgArray::LuaArgArray(const LuaScript::LuaArgArray& rhs)
{
  clear();
  for( const_iterator i = rhs.begin(); i != rhs.end(); ++i )
    push_back((*i)->Clone());
}

LuaScript::LuaArgArray::~LuaArgArray()
{
  std::for_each(begin(), end(), LuaDeleter());
}

LuaScript::LuaArgArray& LuaScript::LuaArgArray::Add(iLuaArg* arg)
{ 
  push_back(arg); 
  return *this; 
}

void LuaScript::LuaArgArray::PopFromLua(lua_State* a_LuaState)
{
  for( size_t i = 0; i < size(); ++i )
    this->at(i)->Unpack(a_LuaState, (int)(i + 1));
}

void LuaScript::LuaArgArray::PushToLua(lua_State* a_LuaState)
{
  for( LuaArgArray::const_iterator i = begin(); i != end(); ++i )
    (*i)->Pack(a_LuaState);
}

LuaScript::LuaArgArray* LuaScript::LuaArgArray::Clone() const
{
  LuaScript::LuaArgArray* copy = new LuaArgArray();
  for( const_iterator i = begin(); i != end(); ++i )
    copy->push_back((*i)->Clone());
  return copy;
}

////////////////////////////////////////////////////////////////////////////////

LuaScript::LuaException::LuaException(const std::string& a_Message):
  m_Message(a_Message),
  m_Error(a_Message)
{
  size_t i = a_Message.find("]:");
  if( i == std::string::npos )
    m_Line = 0;
  else
  {
    std::sscanf(a_Message.c_str() + i + 2, "%d", &m_Line);
    i = a_Message.rfind(": ");
    if( i != std::string::npos )
      m_Error = a_Message.substr(i + 2);
  }
}

void LuaScript::Execute(const std::string& a_ScriptString)
{
  int nError = luaL_dostring(m_LuaState, a_ScriptString.c_str());
  if( nError )
    throw LuaException(lua_tostring(m_LuaState, -1));
}

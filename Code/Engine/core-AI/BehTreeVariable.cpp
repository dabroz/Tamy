#include "core-AI\BehTreeVariable.h"
#include "core-AI\BehaviorTreeRunner.h"
#include "core\ReflectionTypesRegistry.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehaviorTreeVariable );
   PARENT( ReflectionObject );
   PROPERTY_EDIT( "name", std::string, m_name );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BehaviorTreeVariable::BehaviorTreeVariable( const char* name )
   : m_name( name )
   , m_varIndex( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

BehaviorTreeVariable::~BehaviorTreeVariable()
{
}

///////////////////////////////////////////////////////////////////////////////

void BehaviorTreeVariable::setName( const char* name )
{
   NOTIFY_PROPERTY_CHANGE( m_name );
   m_name = name;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeVarFloat );
   PARENT( BehaviorTreeVariable );
   PROPERTY_EDIT( "value", float, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeID< float > BehTreeVarFloat::s_dataType;

///////////////////////////////////////////////////////////////////////////////

BehTreeVarFloat::BehTreeVarFloat( const char* name, float value )
   : BehaviorTreeVariable( name )
   , m_value( value )
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarFloat::valueToString( char* outStrBuf ) const
{
   sprintf_s( outStrBuf, 24, "%.6f", m_value );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarFloat::runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const
{
   RuntimeDataBuffer& data = player->data();
   float value = data[m_runtimeValue];
   sprintf_s( outStrBuf, 24, "%.6f", value );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarFloat::initializeLayout( BehaviorTreeRunner* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_runtimeValue );
   data[m_runtimeValue] = m_value;
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarFloat::setRuntime( BehaviorTreeRunner* player, float val ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_runtimeValue] = val;
}

///////////////////////////////////////////////////////////////////////////////

float BehTreeVarFloat::getRuntime( BehaviorTreeRunner* player ) const
{
   RuntimeDataBuffer& data = player->data();
   return data[m_runtimeValue];
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarFloat::set( float val )
{
   NOTIFY_PROPERTY_CHANGE( m_value );
   m_value = val;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeVarBool );
   PARENT( BehaviorTreeVariable );
   PROPERTY_EDIT( "value", bool, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeID< bool > BehTreeVarBool::s_dataType;

///////////////////////////////////////////////////////////////////////////////

BehTreeVarBool::BehTreeVarBool( const char* name, bool value )
   : BehaviorTreeVariable( name )
   , m_value( value )
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarBool::valueToString( char* outStrBuf ) const
{
   sprintf_s( outStrBuf, 24, m_value ? "true" : "false" );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarBool::runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const
{
   RuntimeDataBuffer& data = player->data();
   bool value = data[m_runtimeValue];
   sprintf_s( outStrBuf, 24, m_value ? "true" : "false" );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarBool::initializeLayout( BehaviorTreeRunner* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_runtimeValue );
   data[m_runtimeValue] = m_value;
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarBool::setRuntime( BehaviorTreeRunner* player, bool val ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_runtimeValue] = val;
}

///////////////////////////////////////////////////////////////////////////////

bool BehTreeVarBool::getRuntime( BehaviorTreeRunner* player ) const
{
   RuntimeDataBuffer& data = player->data();
   return data[m_runtimeValue];
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarBool::set( bool val )
{
   NOTIFY_PROPERTY_CHANGE( m_value );
   m_value = val;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeVarInt );
   PARENT( BehaviorTreeVariable );
   PROPERTY_EDIT( "value", int, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeID< int > BehTreeVarInt::s_dataType;

///////////////////////////////////////////////////////////////////////////////

BehTreeVarInt::BehTreeVarInt( const char* name, int value )
   : BehaviorTreeVariable( name )
   , m_value( value )
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarInt::valueToString( char* outStrBuf ) const
{
   sprintf_s( outStrBuf, 24, "%d", m_value );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarInt::runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const
{
   RuntimeDataBuffer& data = player->data();
   int value = data[m_runtimeValue];
   sprintf_s( outStrBuf, 24, "%d", value );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarInt::initializeLayout( BehaviorTreeRunner* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_runtimeValue );
   data[m_runtimeValue] = m_value;
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarInt::setRuntime( BehaviorTreeRunner* player, int val ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_runtimeValue] = val;
}

///////////////////////////////////////////////////////////////////////////////

int BehTreeVarInt::getRuntime( BehaviorTreeRunner* player ) const
{
   RuntimeDataBuffer& data = player->data();
   return data[m_runtimeValue];
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarInt::set( int val )
{
   NOTIFY_PROPERTY_CHANGE( m_value );
   m_value = val;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeVarVector );
   PARENT( BehaviorTreeVariable );
   PROPERTY_EDIT( "value", Vector, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeID< Vector > BehTreeVarVector::s_dataType;

///////////////////////////////////////////////////////////////////////////////

BehTreeVarVector::BehTreeVarVector( const char* name, const Vector& value )
   : BehaviorTreeVariable( name )
   , m_value( value )
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarVector::valueToString( char* outStrBuf ) const
{
   sprintf_s( outStrBuf, 64, "[%.4f, %.4f, %.4f]", m_value[0], m_value[1], m_value[2] );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarVector::runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const
{
   RuntimeDataBuffer& data = player->data();
   const Vector& value = data[m_runtimeValue];
   sprintf_s( outStrBuf, 64, "[%.4f, %.4f, %.4f]", value[0], value[1], value[2] );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarVector::initializeLayout( BehaviorTreeRunner* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_runtimeValue );
   data[m_runtimeValue] = m_value;
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarVector::setRuntime( BehaviorTreeRunner* player, const Vector& val ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_runtimeValue] = val;
}

///////////////////////////////////////////////////////////////////////////////

const Vector& BehTreeVarVector::getRuntime( BehaviorTreeRunner* player ) const
{
   RuntimeDataBuffer& data = player->data();
   return data[m_runtimeValue];
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarVector::set( const Vector& val )
{
   NOTIFY_PROPERTY_CHANGE( m_value );
   m_value = val;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeVarReflectionObject );
   PARENT( BehaviorTreeVariable );
   PROPERTY_EDIT( "value", ReflectionObject*, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeID< ReflectionObject > BehTreeVarReflectionObject::s_dataType;

///////////////////////////////////////////////////////////////////////////////

BehTreeVarReflectionObject::BehTreeVarReflectionObject( const char* name, const ReflectionObject* value )
   : BehaviorTreeVariable( name )
   , m_value( value )
{
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarReflectionObject::valueToString( char* outStrBuf ) const
{
   sprintf_s( outStrBuf, 24, "%.6f", m_value );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarReflectionObject::runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const
{
   RuntimeDataBuffer& data = player->data();
   const ReflectionObject* value = data[m_runtimeValue];
   sprintf_s( outStrBuf, 24, "%p", (void*)value );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarReflectionObject::initializeLayout( BehaviorTreeRunner* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_runtimeValue );
   data[m_runtimeValue] = m_value;
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarReflectionObject::setRuntime( BehaviorTreeRunner* player, const ReflectionObject* val ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_runtimeValue] = val;
}

///////////////////////////////////////////////////////////////////////////////

const ReflectionObject* BehTreeVarReflectionObject::getRuntime( BehaviorTreeRunner* player ) const
{
   RuntimeDataBuffer& data = player->data();
   return data[m_runtimeValue];
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeVarReflectionObject::set( const ReflectionObject* val )
{
   NOTIFY_PROPERTY_CHANGE( m_value );
   m_value = val;
}

///////////////////////////////////////////////////////////////////////////////

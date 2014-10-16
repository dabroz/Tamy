#include "core-AI\BlendTreeVariable.h"
#include "core-AI\BlendTreePlayer.h"
#include "core\ReflectionTypesRegistry.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BlendTreeVariable );
   PARENT( ReflectionObject );
   PROPERTY_EDIT( "name", std::string, m_name );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BlendTreeVariable::BlendTreeVariable( const char* name )
   : m_name( name )
   , m_varIndex( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////

BlendTreeVariable::~BlendTreeVariable()
{
}

///////////////////////////////////////////////////////////////////////////////

void BlendTreeVariable::setName( const char* name )
{
   NOTIFY_PROPERTY_CHANGE( m_name );
   m_name = name;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTVarFloat );
   PARENT( BlendTreeVariable );
   PROPERTY_EDIT( "value", float, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeID< float > BTVarFloat::s_dataType;

///////////////////////////////////////////////////////////////////////////////

BTVarFloat::BTVarFloat( const char* name, float value )
   : BlendTreeVariable( name )
   , m_value( value )
{
}

///////////////////////////////////////////////////////////////////////////////

void BTVarFloat::valueToString( char* outStrBuf ) const
{
   sprintf_s( outStrBuf, 24, "%.6f", m_value );
}

///////////////////////////////////////////////////////////////////////////////

void BTVarFloat::runtimeValueToString( BlendTreePlayer* player, char* outStrBuf ) const
{
   RuntimeDataBuffer& data = player->data();
   float value = data[m_runtimeValue];
   sprintf_s( outStrBuf, 24, "%.6f", value );
}

///////////////////////////////////////////////////////////////////////////////

void BTVarFloat::initializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_runtimeValue );
   data[m_runtimeValue] = m_value;
}

///////////////////////////////////////////////////////////////////////////////

void BTVarFloat::setRuntime( BlendTreePlayer* player, float val ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_runtimeValue] = val;
}

///////////////////////////////////////////////////////////////////////////////

float BTVarFloat::getRuntime( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   return data[m_runtimeValue];
}

///////////////////////////////////////////////////////////////////////////////

void BTVarFloat::set( float val )
{
   NOTIFY_PROPERTY_CHANGE( m_value );
   m_value = val;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarFloat::isEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   float thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return thisVal == rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return thisVal == rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare the value to vector's length
      float rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length().getFloat();
      return thisVal == rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarFloat::isLess( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   float thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return thisVal < rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return thisVal < rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare the value to vector's length
      float rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length().getFloat();
      return thisVal < rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarFloat::isLessEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   float thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return thisVal <= rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return thisVal <= rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare the value to vector's length
      float rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length().getFloat();
      return thisVal <= rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarFloat::isGreater( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   float thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return thisVal > rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return thisVal > rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare the value to vector's length
      float rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length().getFloat();
      return thisVal > rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarFloat::isGreaterEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   float thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return thisVal >= rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return thisVal >= rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare the value to vector's length
      float rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length().getFloat();
      return thisVal >= rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTVarBool );
   PARENT( BlendTreeVariable );
   PROPERTY_EDIT( "value", bool, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeID< bool > BTVarBool::s_dataType;

///////////////////////////////////////////////////////////////////////////////

BTVarBool::BTVarBool( const char* name, bool value )
   : BlendTreeVariable( name )
   , m_value( value )
{
}

///////////////////////////////////////////////////////////////////////////////

void BTVarBool::valueToString( char* outStrBuf ) const
{
   sprintf_s( outStrBuf, 24, m_value ? "true" : "false" );
}

///////////////////////////////////////////////////////////////////////////////

void BTVarBool::runtimeValueToString( BlendTreePlayer* player, char* outStrBuf ) const
{
   RuntimeDataBuffer& data = player->data();
   bool value = data[m_runtimeValue];
   sprintf_s( outStrBuf, 24, m_value ? "true" : "false" );
}

///////////////////////////////////////////////////////////////////////////////

void BTVarBool::initializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_runtimeValue );
   data[m_runtimeValue] = m_value;
}

///////////////////////////////////////////////////////////////////////////////

void BTVarBool::setRuntime( BlendTreePlayer* player, bool val ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_runtimeValue] = val;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarBool::getRuntime( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   return data[m_runtimeValue];
}

///////////////////////////////////////////////////////////////////////////////

void BTVarBool::set( bool val )
{
   NOTIFY_PROPERTY_CHANGE( m_value );
   m_value = val;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarBool::isEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   bool thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   // a bool can only be compared with another bool
   if( rhsTypeId == BTVarBool::getStaticRTTI().m_id )
   {
      bool rhsVal = ( static_cast< const BTVarBool& >( rhs ) ).getRuntime( player );
      return thisVal == rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarBool::isLess( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   bool thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   // a bool can only be compared with another bool
   if( rhsTypeId == BTVarBool::getStaticRTTI().m_id )
   {
      bool rhsVal = ( static_cast< const BTVarBool& >( rhs ) ).getRuntime( player );
      return thisVal < rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarBool::isLessEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   bool thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   // a bool can only be compared with another bool
   if( rhsTypeId == BTVarBool::getStaticRTTI().m_id )
   {
      bool rhsVal = ( static_cast< const BTVarBool& >( rhs ) ).getRuntime( player );
      return thisVal <= rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarBool::isGreater( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   bool thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   // a bool can only be compared with another bool
   if( rhsTypeId == BTVarBool::getStaticRTTI().m_id )
   {
      bool rhsVal = ( static_cast< const BTVarBool& >( rhs ) ).getRuntime( player );
      return thisVal > rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarBool::isGreaterEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   bool thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   // a bool can only be compared with another bool
   if( rhsTypeId == BTVarBool::getStaticRTTI().m_id )
   {
      bool rhsVal = ( static_cast< const BTVarBool& >( rhs ) ).getRuntime( player );
      return thisVal >= rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTVarInt );
   PARENT( BlendTreeVariable );
   PROPERTY_EDIT( "value", int, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeID< int > BTVarInt::s_dataType;

///////////////////////////////////////////////////////////////////////////////

BTVarInt::BTVarInt( const char* name, int value )
   : BlendTreeVariable( name )
   , m_value( value )
{
}

///////////////////////////////////////////////////////////////////////////////

void BTVarInt::valueToString( char* outStrBuf ) const
{
   sprintf_s( outStrBuf, 24, "%d", m_value );
}

///////////////////////////////////////////////////////////////////////////////

void BTVarInt::runtimeValueToString( BlendTreePlayer* player, char* outStrBuf ) const
{
   RuntimeDataBuffer& data = player->data();
   int value = data[m_runtimeValue];
   sprintf_s( outStrBuf, 24, "%d", value );
}

///////////////////////////////////////////////////////////////////////////////

void BTVarInt::initializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_runtimeValue );
   data[m_runtimeValue] = m_value;
}

///////////////////////////////////////////////////////////////////////////////

void BTVarInt::setRuntime( BlendTreePlayer* player, int val ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_runtimeValue] = val;
}

///////////////////////////////////////////////////////////////////////////////

int BTVarInt::getRuntime( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   return data[m_runtimeValue];
}

///////////////////////////////////////////////////////////////////////////////

void BTVarInt::set( int val )
{
   NOTIFY_PROPERTY_CHANGE( m_value );
   m_value = val;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarInt::isEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   int thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return thisVal == rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return thisVal == rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare the value to vector's length
      float rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length().getFloat();
      return thisVal == rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarInt::isLess( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   int thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return thisVal < rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return thisVal < rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare the value to vector's length
      float rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length().getFloat();
      return thisVal < rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarInt::isLessEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   int thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return thisVal <= rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return thisVal <= rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare the value to vector's length
      float rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length().getFloat();
      return thisVal <= rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarInt::isGreater( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   int thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return thisVal > rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return thisVal > rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare the value to vector's length
      float rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length().getFloat();
      return thisVal > rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarInt::isGreaterEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   int thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return thisVal >= rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return thisVal >= rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare the value to vector's length
      float rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length().getFloat();
      return thisVal >= rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTVarVector );
   PARENT( BlendTreeVariable );
   PROPERTY_EDIT( "value", Vector, m_value );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

ReflectionTypeID< Vector > BTVarVector::s_dataType;

///////////////////////////////////////////////////////////////////////////////

BTVarVector::BTVarVector( const char* name, const Vector& value )
   : BlendTreeVariable( name )
   , m_value( value )
{
}

///////////////////////////////////////////////////////////////////////////////

void BTVarVector::valueToString( char* outStrBuf ) const
{
   sprintf_s( outStrBuf, 25, "[%.4f, %.4f, %.4f]", m_value[0], m_value[1], m_value[2] );
}

///////////////////////////////////////////////////////////////////////////////

void BTVarVector::runtimeValueToString( BlendTreePlayer* player, char* outStrBuf ) const
{
   RuntimeDataBuffer& data = player->data();
   const Vector& value = data[m_runtimeValue];
   sprintf_s( outStrBuf, 25, "[%.4f, %.4f, %.4f]", value[0], value[1], value[2] );
}

///////////////////////////////////////////////////////////////////////////////

void BTVarVector::initializeLayout( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   data.registerVar( m_runtimeValue );
   data[m_runtimeValue] = m_value;
}

///////////////////////////////////////////////////////////////////////////////

void BTVarVector::setRuntime( BlendTreePlayer* player, const Vector& val ) const
{
   RuntimeDataBuffer& data = player->data();
   data[m_runtimeValue] = val;
}

///////////////////////////////////////////////////////////////////////////////

const Vector& BTVarVector::getRuntime( BlendTreePlayer* player ) const
{
   RuntimeDataBuffer& data = player->data();
   return data[m_runtimeValue];
}

///////////////////////////////////////////////////////////////////////////////

void BTVarVector::set( const Vector& val )
{
   NOTIFY_PROPERTY_CHANGE( m_value );
   m_value = val;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarVector::isEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   const Vector& thisVal = data[m_runtimeValue];

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      float len = thisVal.length().getFloat();
      return len == rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      float len = thisVal.length().getFloat();
      return len == rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      const Vector& rhsVal = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player );
      return thisVal == rhsVal;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarVector::isLess( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   const Vector& thisVal = data[m_runtimeValue];
   FastFloat len = thisVal.length();

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return len.getFloat() < rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return len.getFloat() < rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare vector lengths
      FastFloat rhsLen = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length();
      return len < rhsLen;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarVector::isLessEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   const Vector& thisVal = data[m_runtimeValue];
   FastFloat len = thisVal.length();

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return len.getFloat() <= rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return len.getFloat() <= rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare vector lengths
      FastFloat rhsLen = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length();
      return len <= rhsLen;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarVector::isGreater( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   const Vector& thisVal = data[m_runtimeValue];
   FastFloat len = thisVal.length();

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return len.getFloat() > rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return len.getFloat() > rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare vector lengths
      FastFloat rhsLen = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length();
      return len > rhsLen;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

bool BTVarVector::isGreaterEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const
{
   RuntimeDataBuffer& data = player->data();
   const Vector& thisVal = data[m_runtimeValue];
   FastFloat len = thisVal.length();

   const uint rhsTypeId = rhs.getVirtualRTTI().m_id;

   if( rhsTypeId == BTVarFloat::getStaticRTTI().m_id )
   {
      float rhsVal = ( static_cast< const BTVarFloat& >( rhs ) ).getRuntime( player );
      return len.getFloat() >= rhsVal;
   }
   else if( rhsTypeId == BTVarInt::getStaticRTTI().m_id )
   {
      int rhsVal = ( static_cast< const BTVarInt& >( rhs ) ).getRuntime( player );
      return len.getFloat() >= rhsVal;
   }
   else if( rhsTypeId == BTVarVector::getStaticRTTI().m_id )
   {
      // compare vector lengths
      FastFloat rhsLen = ( static_cast< const BTVarVector& >( rhs ) ).getRuntime( player ).length();
      return len >= rhsLen;
   }

   return false;
}

///////////////////////////////////////////////////////////////////////////////

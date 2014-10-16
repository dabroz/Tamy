#include "core-Renderer/MaterialSockets.h"
#include "core-Renderer/MaterialNode.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MSFloatInput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( MSOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( MSFloatOutput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< MaterialNode* >, m_connectedNodes );
END_OBJECT();

GBNodeOutput< MaterialNode >* MSFloatInput::createOutput( const char* name ) const
{
   return new MSFloatOutput( name );
}

GBNodeInput< MaterialNode >* MSFloatOutput::createInput( const char* name ) const
{
   return new MSFloatInput( name );
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MSMatrixInput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( MSOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( MSMatrixOutput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< MaterialNode* >, m_connectedNodes );
END_OBJECT();

GBNodeOutput< MaterialNode >* MSMatrixInput::createOutput( const char* name ) const
{
   return new MSMatrixOutput( name );
}

GBNodeInput< MaterialNode >* MSMatrixOutput::createInput( const char* name ) const
{
   return new MSMatrixInput( name );
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MSMatrixArrInput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( MSOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( MSMatrixArrOutput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< MaterialNode* >, m_connectedNodes );
END_OBJECT();

GBNodeOutput< MaterialNode >* MSMatrixArrInput::createOutput( const char* name ) const
{
   return new MSMatrixArrOutput( name );
}

GBNodeInput< MaterialNode >* MSMatrixArrOutput::createInput( const char* name ) const
{
   return new MSMatrixArrInput( name );
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MSColorInput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( MSOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( MSColorOutput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< MaterialNode* >, m_connectedNodes );
END_OBJECT()   

GBNodeOutput< MaterialNode >* MSColorInput::createOutput( const char* name ) const
{
   return new MSColorOutput( name );
}

GBNodeInput< MaterialNode >* MSColorOutput::createInput( const char* name ) const
{
   return new MSColorInput( name );
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MSVec2Input );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( MSOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( MSVec2Output );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< MaterialNode* >, m_connectedNodes );
END_OBJECT();

GBNodeOutput< MaterialNode >* MSVec2Input::createOutput( const char* name ) const
{
   return new MSVec2Output( name );
}

GBNodeInput< MaterialNode >* MSVec2Output::createInput( const char* name ) const
{
   return new MSVec2Input( name );
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MSVec3Input );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( MSOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( MSVec3Output );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< MaterialNode* >, m_connectedNodes );
END_OBJECT();

GBNodeOutput< MaterialNode >* MSVec3Input::createOutput( const char* name ) const
{
   return new MSVec3Output( name );
}

GBNodeInput< MaterialNode >* MSVec3Output::createInput( const char* name ) const
{
   return new MSVec3Input( name );
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MSVec4Input );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( MSOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( MSVec4Output );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< MaterialNode* >, m_connectedNodes );
END_OBJECT();

GBNodeOutput< MaterialNode >* MSVec4Input::createOutput( const char* name ) const
{
   return new MSVec4Output( name );
}

GBNodeInput< MaterialNode >* MSVec4Output::createInput( const char* name ) const
{
   return new MSVec4Input( name );
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MSBoolInput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( MSOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( MSBoolOutput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< MaterialNode* >, m_connectedNodes );
END_OBJECT();

GBNodeOutput< MaterialNode >* MSBoolInput::createOutput( const char* name ) const
{
   return new MSBoolOutput( name );
}

GBNodeInput< MaterialNode >* MSBoolOutput::createInput( const char* name ) const
{
   return new MSBoolInput( name );
}

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( MSIntInput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( MSOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( MSIntOutput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< MaterialNode* >, m_connectedNodes );
END_OBJECT();

GBNodeOutput< MaterialNode >* MSIntInput::createOutput( const char* name ) const
{
   return new MSIntOutput( name );
}

GBNodeInput< MaterialNode >* MSIntOutput::createInput( const char* name ) const
{
   return new MSIntInput( name );
}

///////////////////////////////////////////////////////////////////////////////

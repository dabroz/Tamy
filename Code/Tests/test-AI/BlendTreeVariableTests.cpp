#include "core-TestFramework\TestFramework.h"
#include "core-AI\BlendTreeVariable.h"


///////////////////////////////////////////////////////////////////////////////

TEST( BlendTreeVariable, accessingVariables )
{
   BTVarFloat floatVar( "speed", 5.0f );
   BTVarBool boolVar( "isMoving", true );

   // accessing variable value directly
   CPPUNIT_ASSERT_EQUAL( 5.0f, floatVar.get() );
   CPPUNIT_ASSERT_EQUAL( true, boolVar.get() );

   // indirect value access
   BlendTreeVariable* variable = &floatVar;
   CPPUNIT_ASSERT_EQUAL( ( ReflectionTypeID< float >() ).m_id, variable->getDataTypeID() );

   variable = &boolVar;
   CPPUNIT_ASSERT_EQUAL( ( ReflectionTypeID< bool >() ).m_id, variable->getDataTypeID() );
}

///////////////////////////////////////////////////////////////////////////////

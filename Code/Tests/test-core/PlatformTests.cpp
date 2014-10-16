#include "core-TestFramework\TestFramework.h"
#include "core\types.h"


///////////////////////////////////////////////////////////////////////////////

TEST( Types, typesSizeCheck )
{
   CPPUNIT_ASSERT_EQUAL( (uint)1, sizeof( byte ) );
   CPPUNIT_ASSERT_EQUAL( (uint)2, sizeof( word ) );   // if this type's not 2 bytes long, index buffers allocated with format D3DFMT_INDEX16 will not work the way we currently handle them 
   CPPUNIT_ASSERT_EQUAL( (uint)4, sizeof( uint ) );
   CPPUNIT_ASSERT_EQUAL( (uint)4, sizeof( ulong ) );
}

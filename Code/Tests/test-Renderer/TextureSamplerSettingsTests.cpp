#include "core-TestFramework\TestFramework.h"
#include "core-Renderer\TextureSamplerSettings.h"


///////////////////////////////////////////////////////////////////////////////

TEST( TextureSamplerSettings, comparison )
{
   TextureSamplerSettings settings1;
   TextureSamplerSettings settings2;

   // equal values
   {
      settings1.m_addressU = 1;
      settings1.m_addressV = 2;
      settings1.m_addressW = 3;
      settings1.m_minFilter = 4;
      settings1.m_magFilter = 5;
      settings1.m_mipFilter = 6;

      settings2.m_addressU = 1;
      settings2.m_addressV = 2;
      settings2.m_addressW = 3;
      settings2.m_minFilter = 4;
      settings2.m_magFilter = 5;
      settings2.m_mipFilter = 6;

      CPPUNIT_ASSERT( settings1 == settings2 );
   }

   // different values
   {
      settings1.m_addressU = 1;
      settings1.m_addressV = 2;
      settings1.m_addressW = 3;
      settings1.m_minFilter = 4;
      settings1.m_magFilter = 5;
      settings1.m_mipFilter = 6;

      settings2.m_addressU = 6;
      settings2.m_addressV = 5;
      settings2.m_addressW = 4;
      settings2.m_minFilter = 3;
      settings2.m_magFilter = 2;
      settings2.m_mipFilter = 1;

      CPPUNIT_ASSERT( settings1 != settings2 );
   }
}

///////////////////////////////////////////////////////////////////////////////

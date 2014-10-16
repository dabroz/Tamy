#include "core-AppFlow/GameConfig.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_RESOURCE( GameConfig, gcf, AM_BINARY )
   PROPERTY( FilePath, m_renderingPipelinePath )
   PROPERTY( FilePath, m_worldModelPath )
END_RESOURCE()

///////////////////////////////////////////////////////////////////////////////

void GameConfig::replaceContents( Resource& rhs )
{
   GameConfig& rhsGameConfig = static_cast< GameConfig& >( rhs );

   m_renderingPipelinePath = rhsGameConfig.m_renderingPipelinePath;
   m_worldModelPath = rhsGameConfig.m_worldModelPath;
}

///////////////////////////////////////////////////////////////////////////////

#include "GameDeploymentInfo.h"


///////////////////////////////////////////////////////////////////////////////

bool GameDeploymentInfo::isOk() const
{
   if ( m_targetDir.empty() )
   {
      return false;
   }

   if ( m_projectDirectories.size() < 2 )
   {
      // there have to be at least 2 directories there - the engine resources dir and the project dir
      return false;
   }

   if ( m_renderingPipelinePath.empty() )
   {
      return false;
   }

   if ( m_worldModelPath.empty() )
   {
      return false;
   }
   
   return true;
}

///////////////////////////////////////////////////////////////////////////////

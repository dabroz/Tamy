/// @file   TamyEditor/GameDeploymentInfo.h
/// @brief  game deployment info structure
#pragma once

#include "core/FilePath.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

/**
 * Supported deployment platforms
 */
enum GameDeploymentPlatform
{
   GDP_WINDOWS_DX9,
   GDP_WINDOWS_OPENGL,
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A structure that holds vital information telling how the game should be deployed.
 */
struct GameDeploymentInfo
{
   std::string                   m_targetDir;
   GameDeploymentPlatform        m_platform;

   std::vector< FilePath >       m_projectDirectories;
   FilePath                      m_renderingPipelinePath;
   FilePath                      m_worldModelPath;

   bool                          m_runAfterDeployment;

   /**
    * Checks if all the data is there and ready for the game to be deployed.
    */
   bool isOk() const;
};

///////////////////////////////////////////////////////////////////////////////

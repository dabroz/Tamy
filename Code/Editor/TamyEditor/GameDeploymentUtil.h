/// @file   TamyEditor/GameDeploymentUtil.h
/// @brief  utility for deploying a game
#pragma once

#include <vector>
#include <string>
#include "core/FilePath.h"


///////////////////////////////////////////////////////////////////////////////

struct GameDeploymentInfo;
class IProgressObserver;
class GameRunner;

///////////////////////////////////////////////////////////////////////////////

/**
 * Utility for deploying a game.
 */
class GameDeploymentUtil
{
public:
   static GameRunner* deployGame( const GameDeploymentInfo& info, IProgressObserver& progressDialog );

private:
   static bool recreateDirectoriesStructures( const std::string& targetDir, const std::vector< FilePath >& projectDirectories );
   static bool copyProjectFiles( const std::string& targetDir, const std::vector< FilePath >& projectDirectories );
   static bool createDirectory( const std::string& dir );

   static bool createGameConfig( const std::string& targetDir, const GameDeploymentInfo& info );

   static GameRunner* createRunner( const GameDeploymentInfo& info );
   static GameRunner* deployWindowsGame( const GameDeploymentInfo& info );
};

///////////////////////////////////////////////////////////////////////////////

/// @file   TamyEditor/GameRunner.h
/// @brief  platform-specific game runner
#pragma once

#include <string>


///////////////////////////////////////////////////////////////////////////////

/**
 * Gamer runner interface.
 */
class GameRunner
{
public:
   virtual ~GameRunner() {}

   /**
    * Runs the game.
    */
   virtual void run() = 0;
};

///////////////////////////////////////////////////////////////////////////////

class WindowsGameRunner : public GameRunner
{
private:
   std::string       m_exePath;

public:
   /**
    * Constructor.
    *
    * @param exePath    path to the exe file
    */
   WindowsGameRunner( const std::string& exePath );

   // -------------------------------------------------------------------------
   // GameRunner implementation
   // -------------------------------------------------------------------------
   void run();
};

///////////////////////////////////////////////////////////////////////////////

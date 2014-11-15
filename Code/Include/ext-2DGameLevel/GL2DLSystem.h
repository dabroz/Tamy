/// @file   ext-2DGameLevel\GL2DLSystem.h
/// @Brief  an L-system generator
#pragma once

#include "core\MemoryRouter.h"
#include "core\Array.h"
#include "core\List.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////

class FilePath;
class Prefab;

///////////////////////////////////////////////////////////////////////////////

class GL2DLSystem
{
   DECLARE_ALLOCATOR( GL2DLSystem, AM_DEFAULT );

private:
   class Rule
   {
      DECLARE_ALLOCATOR( Rule, AM_DEFAULT );

   public:
      std::string          m_pattern;
      std::string          m_replacement;

   public:
      /**
       * Constructor.
       *
       * @param pattern
       * @param replacement
       */
      Rule( const char* pattern, const char* replacement );

      /**
       * Tries to apply the rule to the specified input string, starting with the character located at the specified offset.
       * If the rule can be applied, the result is appended to the output, and a skip offset is returned. 
       *
       * @param input
       * @param firstCharOffset
       * @return           skip offset or -1 in case the rule didn't apply
       */
      int matches( const std::string& input, uint firstCharOffset ) const;
   };

private:
   Array< Rule* >          m_rules;
   Array< Rule* >          m_prefabBindings;

public:
   ~GL2DLSystem();

   /**
    * Loads rule definitions from the specified XML file.
    *
    * @param filePath
    * REturn 'true' if the file was loaded correctly,'false' otherwise
    */
   bool configureFromXML( const FilePath& filePath );

   /**
    * Adds a new rule.
    *
    * @param pattern
    * @param replacement
    */
   void addRule( const char* pattern, const char* replacement );

   /**
    * Binds a specific pattern to a prefab.
    *
    * @param pattern
    * @param prefab
    */
   void addBinding( const char* pattern, const char* prefab );

   /**
    * Processes the specified input the specified number of times before producing the output.
    *
    * @param input
    * @param iterationsCount
    * @param output
    */
   void process( const std::string& input, uint iterationsCount, std::string& output ) const;

   /**
    * Takes a string produced by the 'process' method and translates it to a list of corresponding prefabs.
    *
    * @param processOutput
    * @Param prefabsDir
    * @param outPrefabsList
    */
   void interpret( const std::string& processOutput, const FilePath& prefabsDir, List< Prefab* >& outPrefabsList ) const;

private:
   /**
    * Processes the input a single time.
    *
    * @param input
    * @param output
    */
   void processSingleIteration( const std::string& input, std::string& output ) const;
};

///////////////////////////////////////////////////////////////////////////////

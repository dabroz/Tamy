#include "ext-2DGameLevel\GL2DLSystem.h"
#include "tinyxml\tinyxml.h"
#include "core\FilePath.h"
#include "core\Filesystem.h"
#include "core\ResourcesManager.h"
#include "core\StreamBuffer.h"
#include "core-MVC\Prefab.h"
#include "core\Log.h"


///////////////////////////////////////////////////////////////////////////////

GL2DLSystem::~GL2DLSystem()
{
   const uint rulesCount = m_rules.size();
   for ( uint ruleIdx = 0; ruleIdx < rulesCount; ++ruleIdx )
   {
      delete m_rules[ruleIdx];
   }

   const uint bindingsCount = m_prefabBindings.size();
   for ( uint bindingIdx = 0; bindingIdx < bindingsCount; ++bindingIdx )
   {
      delete m_prefabBindings[bindingIdx];
   }
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DLSystem::configureFromXML( const FilePath& filePath )
{
   Filesystem& fs = TSingleton< Filesystem >::getInstance();
   File* xmlFile = fs.open( filePath );
   if ( !xmlFile )
   {
      WARNING( "GL2DLSystem::configureFromXML : L-system config file %s couldn't be opened", filePath.c_str() );
      return false;
   }

   StreamBuffer<char> xmlFileReader( *xmlFile );
   std::string xmlFileContents = xmlFileReader.getBuffer();
   delete xmlFile;

   // read the XML file
   TiXmlDocument* document = new TiXmlDocument( "LSystem" );
   bool result = document->LoadFile( ( char* ) xmlFileContents.c_str(), xmlFileContents.length(), TIXML_DEFAULT_ENCODING );
   if ( !result )
   {
      WARNING( "GL2DLSystem::configureFromXML : The XML definition in file %s has invalid syntax", filePath.c_str() );
      delete document;
      return false;
   }

   TiXmlNode* lSystemNode = document->FirstChild( "LSystem" );
   if ( !lSystemNode )
   {
      WARNING( "GL2DLSystem::configureFromXML : The XML definition in file %s doesn't contain any LSystem tag", filePath.c_str() );

      delete document;
      return false;
   }

   // load the rules
   for ( TiXmlElement* ruleElem = lSystemNode->FirstChildElement( "Rule" ); ruleElem != NULL; ruleElem = ruleElem->NextSiblingElement( "Rule" ) )
   {
      std::string pattern = ruleElem->Attribute( "pattern" );
      std::string replacement = ruleElem->Attribute( "replacement" );

      addRule( pattern.c_str(), replacement.c_str() );
   }

   // load the prefab bindings
   for ( TiXmlElement* bindingElem = lSystemNode->FirstChildElement( "Binding" ); bindingElem != NULL; bindingElem = bindingElem->NextSiblingElement( "Binding" ) )
   {
      std::string pattern = bindingElem->Attribute( "pattern" );
      std::string prefab = bindingElem->Attribute( "prefab" );

      addBinding( pattern.c_str(), prefab.c_str() );
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLSystem::addRule( const char* pattern, const char* replacement )
{
   m_rules.push_back( new Rule( pattern, replacement ) );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLSystem::addBinding( const char* pattern, const char* prefab )
{
   m_prefabBindings.push_back( new Rule( pattern, prefab ) );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLSystem::process( const std::string& input, uint iterationsCount, std::string& output ) const
{
   std::string tmpInput = input;
   for ( uint i = 0; i < iterationsCount; ++i )
   {
      processSingleIteration( tmpInput, output );
      tmpInput = output;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLSystem::processSingleIteration( const std::string& input, std::string& output ) const
{
   output = "";

   const uint charsCount = input.length();
   const uint rulesCount = m_rules.size();

   for ( uint charIdx = 0; charIdx < charsCount; )
   {
      // compare the input against the rules that we have defined
      for ( uint ruleIdx = 0; ruleIdx < rulesCount; ++ruleIdx )
      {
         Rule* rule = m_rules[ruleIdx];
         int offset = rule->matches( input, charIdx );
         if ( offset > 0 )
         {
            // we found a rule that applies
            charIdx += offset;
            output += rule->m_replacement;
            break;
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DLSystem::interpret( const std::string& processOutput, const FilePath& prefabsDir, List< Prefab* >& outPrefabsList ) const
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   const uint charsCount = processOutput.length();
   const uint bindingsCount = m_prefabBindings.size();

   for ( uint charIdx = 0; charIdx < charsCount; )
   {
      // compare the input against the rules that we have defined
      for ( uint bindingIdx = 0; bindingIdx < bindingsCount; ++bindingIdx )
      {
         Rule* binding = m_prefabBindings[bindingIdx];
         int offset = binding->matches( processOutput, charIdx );
         if ( offset > 0 )
         {
            // we found a rule that applies
            charIdx += offset;

            FilePath prefabsPath = prefabsDir + binding->m_replacement;
            Prefab* prefab = resMgr.create< Prefab >( prefabsPath );
            if ( prefab != NULL )
            {
               outPrefabsList.pushBack( prefab );
            }
            else
            {
               WARNING( "GL2DLSystem::interpret : Can't load prefab %s", prefabsPath.c_str() );
            }

            break;
         }
      }
   }

}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GL2DLSystem::Rule::Rule( const char* pattern, const char* replacement )
   : m_pattern( pattern )
   , m_replacement( replacement )
{
}

///////////////////////////////////////////////////////////////////////////////

int GL2DLSystem::Rule::matches( const std::string& input, uint firstCharOffset ) const
{
   std::string inputCopy = input.substr( firstCharOffset, m_pattern.length() );
   if ( inputCopy != m_pattern )
   {
      // the input string don't match the pattern
      return 0;
   }
   else
   {
      return m_pattern.length();
   }
}

///////////////////////////////////////////////////////////////////////////////

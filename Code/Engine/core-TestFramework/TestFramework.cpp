#include "core-TestFramework\TestFramework.h"
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include "core\dostream.h"

///////////////////////////////////////////////////////////////////////////////

// type includes
#include "core/TypesRegistry.cpp"
#include "core-MVC/TypesRegistry.cpp"
#include "core-AI/TypesRegistry.cpp"
#include "core-Renderer/TypesRegistry.cpp"
#include "core-AppFlow/TypesRegistry.cpp"
#include "core-Physics/TypesRegistry.cpp"
#include "ext-StoryTeller/TypesRegistry.cpp"
#include "ext-2DGameLevel/TypesRegistry.cpp"
#include "ext-ProceduralAnimation/TypesRegistry.cpp"

///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   typedef std::map<const char*, CppUnit::TestSuite*> T_SuiteRegistry;
   T_SuiteRegistry* g_SuiteRegistry = NULL;

} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////

CppUnit::TestSuite& registerSuite(const char* name)
{
   if (g_SuiteRegistry == NULL)
   {
      g_SuiteRegistry = new T_SuiteRegistry;
   }

   T_SuiteRegistry::iterator i = g_SuiteRegistry->find(name);
   if (i == g_SuiteRegistry->end())
   {
      CppUnit::TestSuite* suite = new CppUnit::TestSuite(name);
      (*g_SuiteRegistry)[name] = suite;
      return *suite;
   }
   else
   {
      return *i->second;
   }
}

///////////////////////////////////////////////////////////////////////////////

static void testsRunnerAssertCallback( const char* msg, AssertionHandle& assertHandle )
{
   CPPUNIT_ASSERT_MESSAGE( msg, false );
}

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
   if (g_SuiteRegistry == NULL)
   {
      return 0;
   }

   // initialize subsystems
   TSingleton< ThreadSystem >::initialize();
   TSingleton< Filesystem >::initialize().changeRootDir( "../Data/" );
   TSingleton< ResourcesManager >::initialize();

   // register an assert callback
   registerAssertCallback( &testsRunnerAssertCallback );

   // informs test-listener about test results
   CppUnit::TestResult testResult;

   // register listener for collecting the test-results
   CppUnit::TestResultCollector collectedResults;
   testResult.addListener(&collectedResults);

   // register listener for per-test progress output
   CppUnit::BriefTestProgressListener progress;
   testResult.addListener (&progress);

   // insert test-suite at test-runner by registry
   CppUnit::TestRunner testRunner;
   for (T_SuiteRegistry::const_iterator it = g_SuiteRegistry->begin(); 
        it != g_SuiteRegistry->end(); ++it)
   {
      testRunner.addTest(it->second);
   }
   testRunner.run(testResult);

   // output results in compiler-format
   dostream debugOutput;
   debugOutput << std::endl << std::endl << "============================= TEST RESULTS =============================" << std::endl << std::endl;
   CppUnit::CompilerOutputter compilerOutputter (&collectedResults, debugOutput);
   compilerOutputter.write ();
   debugOutput <<              std::endl << "========================================================================" << std::endl << std::endl;

   delete g_SuiteRegistry;
   g_SuiteRegistry = NULL;

   // deinitialize
   SingletonsManager::deinitialize();
   
   // return 0 if tests were successful
   return collectedResults.wasSuccessful () ? 0 : 1;
}

///////////////////////////////////////////////////////////////////////////////

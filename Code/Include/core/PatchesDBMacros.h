/// @file   core/PatchesDBMacros.h
/// @brief  macros related to the patching mechanism
#pragma once


///////////////////////////////////////////////////////////////////////////////

/**
 * Reflection type registration macros.
 */
#ifndef IMPORT_RTTI_REGISTRATIONS
   #define REGISTER_PATCH_FUNCTION( FuncName )
#else
   #define REGISTER_PATCH_FUNCTION( FuncName )                                \
   struct FuncName##PatchFunctionImporter                                     \
   {                                                                          \
      FuncName##PatchFunctionImporter()                                       \
      {                                                                       \
         PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();                     \
         patchesDB.registerMigrationFunc( #FuncName, &FuncName );             \
      }                                                                       \
   } registerPatchFunction##FuncName;
#endif

///////////////////////////////////////////////////////////////////////////////

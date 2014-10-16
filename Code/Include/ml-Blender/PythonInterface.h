/// @file   ml-Blender/PythonInterface.h
/// @brief  a C-interface that can be accessed from a Python code using 'ctypes' tools
#pragma once

#include "ml-Blender\DllDefines.h"


///////////////////////////////////////////////////////////////////////////////

struct TamyTexture;
struct TamyMaterial;
struct TamyScene;
struct TamyEntity;
struct TamyGeometry;
struct TamyMesh;
struct TamyLight;
struct TamySkeletonComponent;
struct TamyWorld;
struct TamySkeleton;
struct TamyAnimation;
struct TamyExportSettings;

///////////////////////////////////////////////////////////////////////////////

/**
 * Begins the export procedure.
 */
ML_BLENDER_API void begin_export( const char* filesystemRoot, const char* exportDir, int entitiesCount, TamyExportSettings exportSettings );

///////////////////////////////////////////////////////////////////////////////

/**
 * Exports a set of textures from Blender to Tamy.
 */
ML_BLENDER_API void export_textures( TamyTexture* arrTextures, int texturesCount );

///////////////////////////////////////////////////////////////////////////////

/**
 * Exports a material from Blender to Tamy.
 */
ML_BLENDER_API void export_materials( TamyMaterial* arrMaterials, int materialsCount );

///////////////////////////////////////////////////////////////////////////////

/**
 * Exports a set of meshes from Blender to Tamy.
 */
ML_BLENDER_API void export_meshes( TamyMesh* arrMeshes, int meshesCount );

///////////////////////////////////////////////////////////////////////////////

/**
 * Exports a set of meshes from Blender to Tamy.
 */
ML_BLENDER_API void export_skeletons( TamySkeleton* arrSkeletons, int skeletonsCount );

///////////////////////////////////////////////////////////////////////////////

/**
 * Exports a set of animations from Blender to Tamy.
 */
ML_BLENDER_API void export_animations( TamyAnimation* arrAnimations, int animationsCount );

///////////////////////////////////////////////////////////////////////////////

/**
 * Exports a single Entity from Blender to Tamy.
 */
ML_BLENDER_API void export_basic_entity( TamyEntity entity );

///////////////////////////////////////////////////////////////////////////////

/**
 * Exports a single geometry component from Blender to Tamy.
 */
ML_BLENDER_API void export_geometry_component( TamyGeometry geometry );

///////////////////////////////////////////////////////////////////////////////

/**
 * Exports a single Light entity from Blender to Tamy.
 */
ML_BLENDER_API void export_light_entity( TamyLight light );

///////////////////////////////////////////////////////////////////////////////

/**
 * Exports a skeleton component from Blender to Tamy
 */
ML_BLENDER_API void export_skeleton_component( TamySkeletonComponent skeletonComponent );

///////////////////////////////////////////////////////////////////////////////

/**
 * Exports the settings of the world ( ambient lighting setup, fog, sky data and so on )
 */
ML_BLENDER_API void export_world_settings( TamyWorld world );

///////////////////////////////////////////////////////////////////////////////

/**
 * Called once all entities have been exported. It assembles the scene using those exported entities.
 */
ML_BLENDER_API void assemble_scene( const char* sceneName );

///////////////////////////////////////////////////////////////////////////////

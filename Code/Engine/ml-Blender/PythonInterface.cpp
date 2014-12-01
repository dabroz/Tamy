#include "ml-Blender\PythonInterface.h"
#include "ml-Blender\BlenderSceneExporter.h"
#include "ml-Blender\TamyMaterial.h"
#include "ml-Blender\TamyMesh.h"
#include "ml-Blender\TamyScene.h"
#include "ml-Blender\TamyAnimation.h"
#include "core.h"
#include "core-MVC\Component.h"
#include "core-MVC\Entity.h"


///////////////////////////////////////////////////////////////////////////////

void begin_export( const char* filesystemRoot, const char* exportDir, int entitiesCount, TamyExportSettings exportSettings )
{
   // reset the export tool's contents, so that we start fresh
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();
   exporter.initialize( filesystemRoot, exportDir, entitiesCount, exportSettings );
}

///////////////////////////////////////////////////////////////////////////////

void export_textures( TamyTexture* arrTextures, int texturesCount )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();

   for ( int i = 0; i < texturesCount; ++i )
   {
      exporter.createTexture( arrTextures[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

void export_materials( TamyMaterial* arrMaterials, int materialsCount )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();
   exporter.storeMaterials( arrMaterials, materialsCount );
}

///////////////////////////////////////////////////////////////////////////////

void export_meshes( TamyMesh* arrMeshes, int meshesCount )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();

   for ( int i = 0; i < meshesCount; ++i )
   {
      exporter.createMesh( arrMeshes[i] );
   }
}

///////////////////////////////////////////////////////////////////////////////

void export_skeletons( TamySkeleton* arrSkeletons, int skeletonsCount )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();
   exporter.storeSkeletons( arrSkeletons, skeletonsCount );
}

///////////////////////////////////////////////////////////////////////////////

void export_animations( TamyAnimation* arrAnimations, int animationsCount )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();
   exporter.storeAnimations( arrAnimations, animationsCount );
}

///////////////////////////////////////////////////////////////////////////////

void export_basic_entity( TamyEntity basicEntity )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();
   Entity* entity = new Entity( basicEntity.name );

   Matrix globalMtx;
   basicEntity.localMatrix.applyTo( globalMtx );
   exporter.addChild( entity, globalMtx, basicEntity.parentIdx );
}

///////////////////////////////////////////////////////////////////////////////

void export_geometry_component( TamyGeometry geometry )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();

   Component* component = exporter.createGeometryComponent( geometry );
   exporter.addChild( component, Matrix::IDENTITY, geometry.parentIdx );
}

///////////////////////////////////////////////////////////////////////////////

void export_light_entity( TamyLight light )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();
   Entity* entity = exporter.createLightEntity( light );

   Matrix globalMtx;
   light.localMatrix.applyTo( globalMtx );

   Matrix invRotation;
   invRotation.set( 
      -1.0f,  0.0f,  0.0f,  0.0f,
       0.0f,  1.0f,  0.0f,  0.0f,
       0.0f,  0.0f, -1.0f,  0.0f,
       0.0f,  0.0f,  0.0f,  1.0f );

   Matrix rotatedLightMtx;
   rotatedLightMtx.setMul( invRotation, globalMtx );

   exporter.addChild( entity, rotatedLightMtx, light.parentIdx );
}

///////////////////////////////////////////////////////////////////////////////

void export_skeleton_component( TamySkeletonComponent skeletonComponent )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();
   Component* component = exporter.createSkeletonComponent( skeletonComponent );

   Matrix globalMtx;
   skeletonComponent.localMatrix.applyTo( globalMtx );
   exporter.addChild( component, globalMtx, skeletonComponent.parentIdx );
}

///////////////////////////////////////////////////////////////////////////////

void export_world_settings( TamyWorld world )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();
   exporter.setupWorld( world );
}

///////////////////////////////////////////////////////////////////////////////

void assemble_scene( const char* sceneName )
{
   BlenderSceneExporter& exporter = TSingleton< BlenderSceneExporter >::getInstance();
   exporter.assembleScene( sceneName );
}

///////////////////////////////////////////////////////////////////////////////

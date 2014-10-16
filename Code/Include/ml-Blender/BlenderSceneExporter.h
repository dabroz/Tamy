/// @file   ml-Blender/BlenderSceneExporter.h
/// @brief  this class exports blender scenes to Tamy
#pragma once

#include <vector>
#include "core\types.h"
#include "core\Singleton.h"
#include "ml-Blender\TamyMaterial.h"
#include "ml-Blender\BaseExportStructs.h"
#include "ml-Blender\TamyExportSettings.h"
#include "core\Matrix.h"
#include "core\FilePath.h"
#include "core\Array.h"


///////////////////////////////////////////////////////////////////////////////

struct TamyMesh;
struct TamyScene;
struct TamyGeometry;
struct TamyTexture;
struct TamyLight;
struct TamySkeletonComponent;
struct TamyWorld;
struct TamySkeleton;
struct TamyAnimation;
class Component;
class SceneNode;
class TriangleMesh;
class GeometryComponent;
class Texture;
class Material;
class MaterialProfile;
class Entity;
class Skeleton;

///////////////////////////////////////////////////////////////////////////////

class BlenderSceneExporter
{
   REGULAR_SINGLETON();

private:
   struct MaterialDefinition
   {
      std::string    m_materialName;
      FilePath       m_path;
   };

   ALIGN_16 struct ExportedNode
   {
      SceneNode*                             m_node;
      Matrix                                 m_globalMtx;
      int                                    m_parentIndex;
   };

private:
   TamyExportSettings                        m_exportSettings;

   std::string                               m_texturesExportDir;
   std::string                               m_meshesExportDir;
   std::string                               m_scenesExportDir;
   std::string                               m_materialsExportDir;
   std::string                               m_animationsExportDir;
   std::string                               m_prefabsExportDir;

   std::vector< Texture* >                   m_textures;
   std::vector< MaterialDefinition >         m_materialDefinitions;
   std::vector< Skeleton* >                  m_exportedSkeletons;
   Array< ExportedNode >                     m_exportedNodes;
   uint                                      m_nextNodeIdx;
   Entity*                                   m_worldEntity;


public:
   /**
    * Constructor.
    */
   BlenderSceneExporter( const SingletonConstruct& );

   /**
    * Initializes the exporter.
    *
    * @param filesystemRoot
    * @param exportDir
    * @param entitiesCount
    * @param exportSettings
    */
   void initialize( const char* filesystemRoot, const char* exportDir, int entitiesCount, const TamyExportSettings& exportSettings );

   /**
    * Purges all temporary buffers, preparing the singleton instance for the next use.
    */
   void reset();

   /**
    * Creates a texture resource.
    *
    * @param exportedTexture
    */
   void createTexture( const TamyTexture& exportedTexture );

   /**
    * Stores the materials for further lookups.
    *
    * @param arrMaterials
    * @param materialsCount
    */
   void storeMaterials( TamyMaterial* arrMaterials, int materialsCount );

   /**
    * Stores skeletons for further lookups.
    *
    * @param arrSkeletons
    * @param skeletonsCount
    */
   void storeSkeletons( TamySkeleton* arrSkeletons, int skeletonsCount );

   /**
    * Stores exported animations for further lookups.
    *
    * @param arrAnimations
    * @param animationsCount
    */
   void storeAnimations( TamyAnimation* arrAnimations, int animationsCount );

   /**
    * Adds a scene node to the collection of exported nodes under the correct index.
    *
    * @param node
    * @param transform
    * @param parentIdx
    */
   void addChild( SceneNode* node, const Matrix& transform, int parentIdx );

   /**
    * Creates a mesh resource.
    *
    * @param exportedMesh
    */
   TriangleMesh* createMesh( const TamyMesh& exportedMesh );

   /**
    * Creates a geometry component
    *
    * @param exportedGeometry
    */
   Component* createGeometryComponent( const TamyGeometry& exportedGeometry );

   /**
    * Creates a light entity.
    *
    * @param exportedLightEntity
    */
   Entity* createLightEntity( const TamyLight& exportedLightEntity );

   /**
    * Creates an animated entity.
    *
    * @param exportedSkeletonComponent
    */
   Component* createSkeletonComponent( const TamySkeletonComponent& exportedSkeletonComponent );

   /**
    * Creates a collision geometry component.
    *
    * @param exportedCollisionGeometry
    */
   Component* createCollisionGeometry( const TamyGeometry& exportedCollisionGeometry );

   /**
    * Sets up global rendering data.
    *
    * @param worldSettings
    */
   void setupWorld( const TamyWorld& worldSettings );

   /**
    * Creates a scene resource.
    *
    * @param sceneName
    */
   void assembleScene( const char* sceneName );

private:
   /**
    * Creates and attaches a material entity to the specified geometry entity.
    * The material attached is taken from the specified definition that ought to be 
    * loaded before a call to this method was made.
    *
    * @param geometryComponent
    * @param materialIdx
    */
   void attachMaterialToGeometry( GeometryComponent* geometryComponent, int materialIdx ) const;

   /**
    * Instantiates a single directional light entity.
    *
    * @param exportedLightEntity
    */
   Entity* instantiateDirectionalLight( const TamyLight& exportedLightEntity );
   
   /**
    * Instantiates a single point light entity.
    *
    * @param exportedLightEntity
    */
   Entity* instantiatePointLight( const TamyLight& exportedLightEntity );
};

///////////////////////////////////////////////////////////////////////////////

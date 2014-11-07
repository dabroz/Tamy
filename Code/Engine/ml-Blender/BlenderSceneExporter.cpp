#include "ml-Blender\BlenderSceneExporter.h"
#include "core.h"
#include "core-Renderer.h"
#include "core-MVC.h"
#include "core-AI.h"
#include "ext-RenderingPipeline\LocationRenderSettings.h"
#include "ml-Blender\BlenderMaterialBuilder.h"

#include "ml-Blender\TamyMaterial.h"
#include "ml-Blender\TamyMesh.h"
#include "ml-Blender\TamyScene.h"
#include "ml-Blender\TamyAnimation.h"


///////////////////////////////////////////////////////////////////////////////

#define TEXTURES_EXPORT_DIR      std::string( "/Textures/" )
#define MATERIALS_EXPORT_DIR     std::string( "/Material/" )
#define MESHES_EXPORT_DIR        std::string( "/Meshes/" )
#define SCENES_EXPORT_DIR        std::string( "/Scenes/" )
#define ANIMATIONS_EXPORT_DIR    std::string( "/Animations/" )
#define PREFABS_EXPORT_DIR       std::string( "/Prefabs/" )

///////////////////////////////////////////////////////////////////////////////

INIT_SINGLETON( BlenderSceneExporter )

///////////////////////////////////////////////////////////////////////////////

BlenderSceneExporter::BlenderSceneExporter( const SingletonConstruct& )
{
}

///////////////////////////////////////////////////////////////////////////////

void BlenderSceneExporter::initialize( const char* filesystemRoot, const char* exportDir, int entitiesCount, const TamyExportSettings& exportSettings )
{ 
   // store the export settings
   m_exportSettings = exportSettings;

   // set the filesystem path
   std::string normalizedFilesystemRoot;
   FilesystemUtils::normalize( filesystemRoot, normalizedFilesystemRoot );
   TSingleton< Filesystem >::initialize().changeRootDir( normalizedFilesystemRoot );
   TSingleton< ResourcesManager>::initialize();

   Filesystem& fs = TSingleton< Filesystem >::getInstance();

   // build the types registry 
   {
      // load patching definitions
      FilePath patchesDBDefPath( "/Renderer/patchesDB.xml" );
      File* patchesDBFile = fs.open( patchesDBDefPath, std::ios_base::in );
      StreamBuffer< char > fileBuf( *patchesDBFile );
      std::string patchesDBContents = fileBuf.getBuffer();
      delete patchesDBFile;

      PatchesDB& patchesDB = TSingleton< PatchesDB >::getInstance();
      PatchesDBSerializer::load( patchesDB, patchesDBContents );

      TSingleton< ReflectionTypesRegistry >::getInstance().build( patchesDB );
   }

   // setup the export directories
   std::string normalizedExportDir;
   FilesystemUtils::normalize( exportDir, normalizedExportDir );
   std::string relativeExportDir = fs.toRelativePath( normalizedExportDir );

   m_texturesExportDir = relativeExportDir + TEXTURES_EXPORT_DIR;
   m_meshesExportDir = relativeExportDir + MESHES_EXPORT_DIR;
   m_scenesExportDir = relativeExportDir + SCENES_EXPORT_DIR;
   m_materialsExportDir = relativeExportDir + MATERIALS_EXPORT_DIR;
   m_animationsExportDir = relativeExportDir + ANIMATIONS_EXPORT_DIR;
   m_prefabsExportDir = relativeExportDir + PREFABS_EXPORT_DIR;

   // physically create the directories
   FilesystemUtils::mkdir( fs.toAbsolutePath( m_texturesExportDir ).c_str() ); 
   FilesystemUtils::mkdir( fs.toAbsolutePath( m_meshesExportDir ).c_str() );
   FilesystemUtils::mkdir( fs.toAbsolutePath( m_scenesExportDir ).c_str() );
   FilesystemUtils::mkdir( fs.toAbsolutePath( m_materialsExportDir ).c_str() );
   FilesystemUtils::mkdir( fs.toAbsolutePath( m_animationsExportDir ).c_str() );
   FilesystemUtils::mkdir( fs.toAbsolutePath( m_prefabsExportDir ).c_str() );


   // clear the storages
   m_textures.clear();
   m_materialDefinitions.clear();

   m_exportedSkeletons.clear();

   m_exportedNodes.clear();
   m_exportedNodes.resizeWithoutInitializing( entitiesCount );
   m_nextNodeIdx = 0;

   m_worldEntity = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlenderSceneExporter::reset()
{
   m_textures.clear();

   m_materialDefinitions.clear();

   m_exportedSkeletons.clear();

   m_exportedNodes.clear();
   m_worldEntity = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void BlenderSceneExporter::createTexture( const TamyTexture& exportedTexture )
{
   Texture* texture = NULL;
   if ( exportedTexture.path != NULL )
   {
      ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

      // copy the actual image to the filesystem root
      std::string normalizedTexturePath;
      FilesystemUtils::normalize( exportedTexture.path, normalizedTexturePath );
      std::string imgFileName = FilesystemUtils::extractNodeName( normalizedTexturePath );
      FilePath newImagePath( m_texturesExportDir + imgFileName );

      std::string absImagePath = newImagePath.toAbsolutePath( resMgr.getFilesystem() );
      FilesystemUtils::copyFile( exportedTexture.path, absImagePath.c_str() );

      // create and setup a texture resource for it
      texture = resMgr.create< Texture >( newImagePath );
      texture->setImagePath( newImagePath );

      // save the texture
      if ( m_exportSettings.saveMaterials )
      {
         texture->saveResource();
      }
   }

   // add the texture to the lookups storage - we're adding NULLs as well here,
   // because the indexing order is crucial
   m_textures.push_back( texture );
}

///////////////////////////////////////////////////////////////////////////////

void BlenderSceneExporter::storeMaterials( TamyMaterial* arrMaterials, int materialsCount )
{
   ASSERT_MSG( m_materialDefinitions.empty(), "This method is going to override the existing materials definitions!" );

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   // delete previous definitions
   m_materialDefinitions.clear();
   m_materialDefinitions.resize( materialsCount );

   BlenderMaterialBuilder builder( m_textures );

   char tmpMatPath[128];
   for( int i = 0; i < materialsCount; ++i )
   {
      const TamyMaterial& exportedMaterial = arrMaterials[i];
      m_materialDefinitions[i].m_materialName = exportedMaterial.name;

      sprintf_s( tmpMatPath , "%s%s.%s", m_materialsExportDir.c_str(), exportedMaterial.name, Material::getExtension() );
      m_materialDefinitions[i].m_path = FilePath( tmpMatPath );

      Material* material = resMgr.create< Material >( m_materialDefinitions[i].m_path );

      // allocate a single profile
      builder.buildMaterial( exportedMaterial, material );

      // save the material
      if ( m_exportSettings.saveMaterials )
      {
         material->saveResource();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlenderSceneExporter::storeSkeletons( TamySkeleton* arrSkeletons, int skeletonsCount )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   m_exportedSkeletons.clear();
   m_exportedSkeletons.resize( skeletonsCount );

   Matrix boneLocalMtx;
   for ( int skeletonIdx = 0; skeletonIdx < skeletonsCount; ++skeletonIdx )
   {
      const TamySkeleton& exportedSkeleton = arrSkeletons[skeletonIdx];

      FilePath skeletonPath( m_animationsExportDir + exportedSkeleton.name + "." + Skeleton::getExtension() );
      Skeleton* skeleton = resMgr.create< Skeleton >( skeletonPath );
      skeleton->clear();
      m_exportedSkeletons[skeletonIdx] = skeleton;

      // set skeleton bones
      for ( int boneIdx = 0; boneIdx < exportedSkeleton.bonesCount; ++boneIdx )
      {
         const TamyBone& exportedBone = exportedSkeleton.bones[boneIdx];
         
         Matrix boneLocalTransform, invBoneMSTransform;
         if ( exportedBone.parentBoneIdx >= 0 )
         {
            const TamyBone& parentBone = exportedSkeleton.bones[exportedBone.parentBoneIdx];

            parentBone.modelSpaceTransform.applyTo( invBoneMSTransform );
            invBoneMSTransform.invert();

            Matrix boneMSTransform;
            exportedBone.modelSpaceTransform.applyTo( boneMSTransform );

            boneLocalTransform.setMul( boneMSTransform, invBoneMSTransform );
         }
         else
         {
            exportedBone.modelSpaceTransform.applyTo( boneLocalTransform );
         }

         skeleton->addBone( exportedBone.name, boneLocalTransform, exportedBone.parentBoneIdx, exportedBone.boneLength );
      }

      // calculate skeleton's bind pose
      skeleton->buildSkeleton();

      // save the skeleton
      if ( m_exportSettings.saveAnimations )
      {
         skeleton->saveResource();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlenderSceneExporter::storeAnimations( TamyAnimation* arrAnimations, int animationsCount )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   for ( int i = 0; i < animationsCount; ++i )
   {
      TamyAnimation& exportedAnimation = arrAnimations[i];

      Skeleton* skeleton = NULL;
      if ( exportedAnimation.skeletonIdx >= 0 )
      {
         skeleton = m_exportedSkeletons[exportedAnimation.skeletonIdx];
      }

      // create pose tracks array
      Transform* poseTransforms = new Transform[ exportedAnimation.framesCount * exportedAnimation.poseTracksCount ];
      for ( int trackIdx = 0; trackIdx < exportedAnimation.poseTracksCount; ++trackIdx )
      {
         TamyAnimationTrack& exportedTrack = exportedAnimation.poseTracks[trackIdx];

         // find the bone this track corresponds to
         uint boneTrackIdx = 0;
         if ( skeleton )
         {
            boneTrackIdx = skeleton->getBoneIndex( exportedTrack.boneName );
         }

         for ( int frameIdx = 0; frameIdx < exportedAnimation.framesCount; ++frameIdx )
         {
            uint transformIdx = frameIdx * exportedAnimation.poseTracksCount + boneTrackIdx;
            exportedTrack.frameKeys[frameIdx].applyTo( poseTransforms[transformIdx] );
         }
      }

      // create the motion track transforms array
      Transform* motionTransforms = new Transform[ exportedAnimation.framesCount ];
      for ( int frameIdx = 0; frameIdx < exportedAnimation.framesCount; ++frameIdx )
      {
         exportedAnimation.motionTrack.frameKeys[frameIdx].applyTo( motionTransforms[frameIdx] );
      }

      FilePath animationPath( m_animationsExportDir + exportedAnimation.name + "." + SnapshotAnimation::getExtension() );
      SnapshotAnimation* animation = resMgr.create< SnapshotAnimation >( animationPath );
      SnapshotAnimation::build( *animation, exportedAnimation.playbackSpeed, exportedAnimation.framesCount, exportedAnimation.poseTracksCount, poseTransforms, motionTransforms );

      delete [] poseTransforms;
      delete [] motionTransforms;

      // save the animation
      if ( m_exportSettings.saveAnimations )
      {
         animation->saveResource();
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BlenderSceneExporter::addChild( SceneNode* node, const Matrix& transform, int parentIdx )
{
   ExportedNode& exportedNode = m_exportedNodes[m_nextNodeIdx];
   exportedNode.m_node = node;
   exportedNode.m_parentIndex = parentIdx;
   exportedNode.m_globalMtx = transform;
   ++m_nextNodeIdx;
}

///////////////////////////////////////////////////////////////////////////////

TriangleMesh* BlenderSceneExporter::createMesh( const TamyMesh& exportedMesh )
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   // create the resource
   FilePath meshPath( m_meshesExportDir + exportedMesh.name + "." + TriangleMesh::getExtension() );
   TriangleMesh* triangleMesh = resMgr.create< TriangleMesh >( meshPath );

   // set mesh data
   triangleMesh->setVertices( exportedMesh.verticesList, exportedMesh.verticesCount );
   triangleMesh->setFaces( exportedMesh.facesList, exportedMesh.facesCount );
   triangleMesh->calculateTangents();

   if ( exportedMesh.hasVertexWeights )
   {
      triangleMesh->setVertexWeights( exportedMesh.vertexWeightsList, exportedMesh.verticesCount );
   }

   // save the resource
   if ( m_exportSettings.saveMeshes )
   {
      triangleMesh->saveResource();
   }

   return triangleMesh;
}

///////////////////////////////////////////////////////////////////////////////

Component* BlenderSceneExporter::createGeometryComponent( const TamyGeometry& exportedGeometry ) 
{
   // create the mesh
   const TamyMesh& exportedMesh = exportedGeometry.mesh;

   TriangleMesh* mesh = createMesh( exportedMesh );
   ASSERT_MSG( mesh != NULL, "Specified mesh wasn't correctly loaded" );
   if ( !mesh )
   {
      return NULL;
   }

   // if the exported mesh contains vertex weights, create a SkinnedGeometry. Otherwise consider it a static geometry
   GeometryComponent* geometry = new GeometryComponent( *mesh, exportedMesh.name );

   // now attach the defined materials to it
   for ( int i = 0; i < exportedMesh.materialsCount; ++i )
   {
      int materialIdx = exportedMesh.materialsList[i];
      attachMaterialToGeometry( geometry, materialIdx );
   }

   return geometry;
}

///////////////////////////////////////////////////////////////////////////////

void BlenderSceneExporter::attachMaterialToGeometry( GeometryComponent* geometryComponent, int materialIdx ) const
{
   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   if ( materialIdx < 0 || materialIdx >= (int)m_materialDefinitions.size() )
   {
      // use the default material
      static FilePath defMatPath( "/Renderer/Materials/Default/defaultMaterial.tmat" ); 
      Material* material = resMgr.create< Material >( defMatPath, false );
      ASSERT_MSG( material, "Default material resource is missing" );

      geometryComponent->setMaterial( material );
   }
   else
   {  
      const MaterialDefinition& materialDef = m_materialDefinitions[materialIdx];
      Material* material = resMgr.create< Material >( materialDef.m_path, true );
      geometryComponent->setMaterial( material );
   }
}

///////////////////////////////////////////////////////////////////////////////

Entity* BlenderSceneExporter::createLightEntity( const TamyLight& exportedLightEntity )
{
   Entity* lightEntity = NULL;

   switch( exportedLightEntity.type )
   {
   case LT_Directional:
      {
         lightEntity = instantiateDirectionalLight( exportedLightEntity );
         break;
      }

   case LT_Point:
      {
         lightEntity = instantiatePointLight( exportedLightEntity );
         break;
      }

   case LT_Spotlight:
      {
         // not implemented yet
         break;
      }

   case LT_Hemisphere:
      {
         // not implemented yet
         break;
      }

   case LT_Area:
      {
         // not implemented yet
         break;
      }

   }

   return lightEntity;
}

///////////////////////////////////////////////////////////////////////////////

Entity* BlenderSceneExporter::instantiateDirectionalLight( const TamyLight& exportedLightEntity )
{
   Entity* lightEntity = new Entity( exportedLightEntity.name );
   DirectionalLight* light = new DirectionalLight();
   lightEntity->addChild( light );

   light->m_color = exportedLightEntity.lightColor;
   light->m_strength = exportedLightEntity.energy;
   light->setShadowsCaster( exportedLightEntity.castShadows );

   return lightEntity;
}

///////////////////////////////////////////////////////////////////////////////

Entity* BlenderSceneExporter::instantiatePointLight( const TamyLight& exportedLightEntity )
{
   Entity* lightEntity = new Entity( exportedLightEntity.name );
   PointLight* light = new PointLight();
   lightEntity->addChild( light );

   light->m_color = exportedLightEntity.lightColor;
   light->m_strength = exportedLightEntity.energy;
   light->m_radius = exportedLightEntity.distance;
   light->m_falloff = exportedLightEntity.quadraticAttenuation;
   light->setShadowsCaster( exportedLightEntity.castShadows );

   return lightEntity;
}

///////////////////////////////////////////////////////////////////////////////

Component* BlenderSceneExporter::createSkeletonComponent( const TamySkeletonComponent& exportedSkeletonComponent )
{
   SkeletonComponent* skeletonComponent = new SkeletonComponent( exportedSkeletonComponent.name );

   Matrix skeletonLocalMtx;
   exportedSkeletonComponent.localMatrix.applyTo( skeletonLocalMtx );
   skeletonComponent->setLocalMtx( skeletonLocalMtx );

   Skeleton* skeleton = m_exportedSkeletons[exportedSkeletonComponent.skeletonIdx];
   skeletonComponent->setSkeleton( skeleton );

   return skeletonComponent;
}

///////////////////////////////////////////////////////////////////////////////

void BlenderSceneExporter::setupWorld( const TamyWorld& worldSettings )
{
   m_worldEntity = new Entity( "WorldSettings" );

   LocationRenderSettings* renderSettings = new LocationRenderSettings( "World settings" );
   m_worldEntity->addChild( renderSettings );

   renderSettings->m_ambientLight = worldSettings.ambientLightColor;
}

///////////////////////////////////////////////////////////////////////////////

void BlenderSceneExporter::assembleScene( const char* sceneName )
{
   if ( !m_exportSettings.saveObjects )
   {
      return;
   }

   ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();

   FilePath scenePath( m_scenesExportDir + sceneName );
   Model* scene = resMgr.create< Model >( scenePath );
   scene->clear();

   Array< Entity* > entitiesToTurnToPrefabs;

   // create a scene transaction to which we'll add all created entities
   // setup the parenting hierarchy
   Matrix localMtx, invParentGlobalMtx;
   uint entitiesCount = m_exportedNodes.size();
   for ( uint i = 0; i < entitiesCount; ++i )
   {
      ExportedNode& exportedNode = m_exportedNodes[i];
      if ( exportedNode.m_node == NULL )
      {
         continue;
      }

      if ( exportedNode.m_parentIndex >= 0 )
      {
         // this one has a parent
         ExportedNode& exportedParentNode = m_exportedNodes[ exportedNode.m_parentIndex ];
         SceneNode* parentNode = exportedParentNode.m_node;
         ASSERT_MSG( parentNode != exportedNode.m_node, "Hierarchy of entities is corrupt - an entity is attempting to be its own parent" );

         ASSERT_MSG( parentNode->isA< Entity >(), "Parent node needs to be an Entity" );
         Entity* parentEntity = static_cast< Entity* >( parentNode );

         parentEntity->addChild( exportedNode.m_node );
      }
      else
      {
         // this is the top level entity
         scene->addChild( exportedNode.m_node );

         // if it's not a light, trun it into a prefab
         if ( exportedNode.m_node->isA< Entity >() && !exportedNode.m_node->isA< Light >() )
         {
            entitiesToTurnToPrefabs.push_back( static_cast< Entity* >( exportedNode.m_node ) );
         }
      }

      // if the exported node is an entity, set its transform
      if ( exportedNode.m_node->isA< Entity >() )
      {
         Entity* exportedEntity = static_cast< Entity* >( exportedNode.m_node );
         exportedEntity->setLocalMtx( exportedNode.m_globalMtx );
      }
   }

   // add the world-related entities
   if ( m_worldEntity )
   {
      scene->addChild( m_worldEntity );
   }

   // turn all root level entities ( except for the world settings and lights ) into prefabs
   entitiesCount = entitiesToTurnToPrefabs.size();
   for ( uint i = 0; i < entitiesCount; ++i )
   {
      Entity* entity = entitiesToTurnToPrefabs[i];

      FilePath prefabPath( m_prefabsExportDir + entity->getSceneNodeName() );
      Prefab* prefab = resMgr.create< Prefab >( prefabPath );

      prefab->setEntity( entity );
      if ( m_exportSettings.savePrefabs )
      {
         prefab->saveResource();
      }
   }

   // save the resource
   if ( m_exportSettings.saveObjects )
   {
      scene->saveResource();
   }

   // cleanup
   reset();
}

///////////////////////////////////////////////////////////////////////////////

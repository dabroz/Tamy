#include "DRSkeletonEntity.h"
#include "core-AI\SkeletonComponent.h"
#include "core-AI\Skeleton.h"
#include "core-Renderer\TriangleMesh.h"
#include "core-Renderer\GeometryComponent.h"
#include "core-Renderer\RenderState.h"
#include "core-Renderer\Material.h"
#include "DebugGeometryBuilder.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT( DRSkeletonEntity )
   PARENT( DebugGeometry )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

DRSkeletonEntity::DRSkeletonEntity( SkeletonComponent& skeletonComponent )
   : DebugGeometry( skeletonComponent.getParent() )
   , m_skeletonComponent( skeletonComponent )
   , m_boneMesh( NULL )
   , m_boneRepresentationsCount( 0 )
   , m_boneRepresentations( NULL )
{
   m_skeletonComponent.attachListener( *this );

   m_boneMesh = DebugGeometryBuilder::createBone();

   initialize();
}

///////////////////////////////////////////////////////////////////////////////

DRSkeletonEntity::~DRSkeletonEntity()
{
   m_skeletonComponent.detachListener( *this );

   deinitialize();

   m_boneMesh->removeReference();
   m_boneMesh = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void DRSkeletonEntity::initialize()
{
   if ( m_skeletonComponent.m_skeleton )
   {
      ResourcesManager& resMgr = TSingleton< ResourcesManager >::getInstance();
      Material* material = resMgr.create< Material >( FilePath( "/Editor/Materials/Bone.tmat" ), false );

      m_boneRepresentationsCount = m_skeletonComponent.m_skeleton->getBoneCount();
      m_boneRepresentations = new GeometryPtr[m_boneRepresentationsCount];
      for ( uint boneIdx = 0; boneIdx < m_boneRepresentationsCount; ++boneIdx )
      {
         DebugGeometryComponent* boneGeometry = new DebugGeometryComponent( *m_boneMesh ); 
         m_boneRepresentations[boneIdx] = boneGeometry;

         boneGeometry->setMaterial( material );

         addChild( boneGeometry );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void DRSkeletonEntity::deinitialize()
{
   if ( m_boneRepresentationsCount > 0 )
   {
      for ( uint boneIdx = 0; boneIdx < m_boneRepresentationsCount; ++boneIdx )
      {
         GeometryComponent* boneGeometry = m_boneRepresentations[boneIdx];
         boneGeometry->remove();
      }
   }

   m_boneRepresentationsCount = 0;

   delete [] m_boneRepresentations;
   m_boneRepresentations = NULL;

}

///////////////////////////////////////////////////////////////////////////////

void DRSkeletonEntity::updateDebugData( float timeElapsed )
{
   if ( !m_skeletonComponent.m_skeleton )
   {
      return;
   }


   Vector boneDir;
   for ( uint boneIdx = 0; boneIdx < m_boneRepresentationsCount; ++boneIdx )
   {
      ASSERT( m_boneRepresentationsCount == m_skeletonComponent.m_skeleton->getBoneCount() );

      Matrix scaleMtx;
      scaleMtx.scaleUniform( m_skeletonComponent.m_skeleton->m_boneLengths[boneIdx] );

      Matrix boneRepModelMtx;
      boneRepModelMtx.setMul( scaleMtx, m_skeletonComponent.m_boneModelMtx[boneIdx] );

      m_boneRepresentations[boneIdx]->setExtraTransform( boneRepModelMtx );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DRSkeletonEntity::enableBoundingBox( bool enable )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void DRSkeletonEntity::enableDebugShape( bool enable )
{
   for ( uint boneIdx = 0; boneIdx < m_boneRepresentationsCount; ++boneIdx )
   {
      m_boneRepresentations[boneIdx]->enableRendering( enable );
   }
}

///////////////////////////////////////////////////////////////////////////////

void DRSkeletonEntity::onObservedPropertyChanged( ReflectionProperty& property )
{
   // switch the skeleton representation
   if ( property.getName() == "m_skeleton" )
   {
      deinitialize();
      initialize();
   }
}

///////////////////////////////////////////////////////////////////////////////

void DRSkeletonEntity::onObservedObjectDeleted( ReflectionObject* deletedObject )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

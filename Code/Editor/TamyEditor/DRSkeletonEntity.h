/// @file   TamyEditor/DRSkeletonEntity.h
/// @brief  debug representation of an animated entity
#pragma once

#include "DebugGeometry.h"
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class SkeletonComponent;
class RenderState;
class TriangleMesh;

///////////////////////////////////////////////////////////////////////////////

/**
 * Debug representation of an animated entity.
 */
class DRSkeletonEntity : public DebugGeometry, public ReflectionObjectChangeListener
{
   DECLARE_ALLOCATOR( DRSkeletonEntity, AM_DEFAULT );
   DECLARE_CLASS()

private:
   SkeletonComponent&               m_skeletonComponent;

   TriangleMesh*                    m_boneMesh;

   typedef DebugGeometryComponent* GeometryPtr;
   uint                             m_boneRepresentationsCount;
   GeometryPtr*                     m_boneRepresentations;

public:
   /**
    * Constructor.
    *
    * @param skeletonComponent
    */
   DRSkeletonEntity( SkeletonComponent& skeletonComponent );
   ~DRSkeletonEntity();

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedPropertyChanged( ReflectionProperty& property );
   void onObservedObjectDeleted( ReflectionObject* deletedObject );

   // -------------------------------------------------------------------------
   // DebugGeometry implementation
   // -------------------------------------------------------------------------
   void updateDebugData( float timeElapsed );
   void enableBoundingBox( bool enable );
   void enableDebugShape( bool enable );

private:
   void initialize();
   void deinitialize();
};

///////////////////////////////////////////////////////////////////////////////

#include "Gizmo.h"
#include "core-Renderer.h"
#include "core.h"
#include "core-MVC.h"
#include "GizmoAxis.h"
#include "DebugGeometryBuilder.h"
#include "SelectionManager.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ABSTRACT_OBJECT(Gizmo)
   PARENT( Entity )
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

Gizmo::Gizmo( SelectionManager* selectionMgr, Mode mode, GizmoReferenceFrame refFrame )
   : m_selectionMgr( selectionMgr )
   , m_mode( ( Mode ) -1 )
   , m_gizmoReferenceFrame( refFrame )
{
   
   // create gizmo axes
   for ( byte i = 0; i < 3; ++i )
   {
      m_meshes[i] = NULL;

      m_geometry[i] = new GizmoAxis( i, *this );
      addChild( m_geometry[i] );
   }

   // set custom bounds for this entity
   m_boundingVolume.set( Vector( -100000.0f, -100000.0f, -100000.0f ), Vector( 100000.0f, 100000.0f, 100000.0f ) );
   addBoundingVolume( &m_boundingVolume );

   // set the initial mode
   setMode( mode );

   // attach self to the selection manager
   m_selectionMgr->attach( *this );
}

///////////////////////////////////////////////////////////////////////////////

Gizmo::~Gizmo()
{
   // detach self to the selection manager
   m_selectionMgr->detach( *this );

   // geometries will be removed by this entity
   for ( byte i = 0; i < 3; ++i )
   {
      SAFE_DELETE( m_meshes[i] );

      m_geometry[i] = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::setMode( Mode mode )
{
   if ( m_mode == mode )
   {
      return;
   }

   // set the new mode and the related geometry
   m_mode = mode;

   switch( m_mode )
   {
   case GM_TRANSLATION:
      {
         createTranslationGizmo();
         break;
      }

   case GM_ROTATION:
      {
         createRotationGizmo();
         break;
      }

   case GM_SCALING:
      {
         createScalingGizmo();
         break;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::createTranslationGizmo()
{
   static FastFloat armLength = FastFloat::fromFloat( 0.2f );
   float armHalfDim = 0.003f;
   Vector ox, oy, oz;
   ox.setMul( Vector_OX, armLength );
   oy.setMul( Vector_OY, armLength );
   oz.setMul( Vector_OZ, armLength );

   // OX
   SAFE_DELETE( m_meshes[0] );
   m_meshes[0] = DebugGeometryBuilder::createArrow( armHalfDim, Vector_ZERO, ox );
   m_geometry[0]->setMesh( *m_meshes[0] );

   // OY
   SAFE_DELETE( m_meshes[1] );
   m_meshes[1] = DebugGeometryBuilder::createArrow( armHalfDim, Vector_ZERO, oy );
   m_geometry[1]->setMesh( *m_meshes[1] );

   // OZ
   SAFE_DELETE( m_meshes[2] );
   m_meshes[2] = DebugGeometryBuilder::createArrow( armHalfDim, Vector_ZERO, oz );
   m_geometry[2]->setMesh( *m_meshes[2] );
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::createRotationGizmo()
{
   Quaternion rotQ;

   float ringRadius = 0.2f;
   float ringCircumferenceWidth = 0.008f;

   Matrix transformOX, transformOY, transformOZ;

   static FastFloat rotAngle = FastFloat::fromFloat( DEG2RAD( 90.0f ) );

   // calculate transform OX
   {
      rotQ.setAxisAngle( Vector_OY, rotAngle );
      transformOX.setRotation( rotQ );
   }

   // calculate transform OY
   {
      rotQ.setAxisAngle( Vector_OX, rotAngle );
      transformOY.setRotation( rotQ );
   }

   // calculate transform OZ
   {
      rotQ.setAxisAngle( Vector_OZ, rotAngle );
      transformOZ.setRotation( rotQ );
   }

   const uint segmentsCount = 56;
   const uint segmentVerticesCount = 3;
   float innerRadius = ringRadius - ringCircumferenceWidth * 0.5f;
   float outerRadius = ringRadius + ringCircumferenceWidth * 0.5f;

   // OX
   SAFE_DELETE( m_meshes[0] );
   m_meshes[0] = DebugGeometryBuilder::createTorus( innerRadius, outerRadius, transformOX, segmentsCount, segmentVerticesCount );
   m_geometry[0]->setMesh( *( m_meshes[0] ) );
   
   // OY
   SAFE_DELETE( m_meshes[1] );
   m_meshes[1] = DebugGeometryBuilder::createTorus( innerRadius, outerRadius, transformOY, segmentsCount, segmentVerticesCount );
   m_geometry[1]->setMesh( *( m_meshes[1] ) );

   // OZ
   SAFE_DELETE( m_meshes[2] );
   m_meshes[2] = DebugGeometryBuilder::createTorus( innerRadius, outerRadius, transformOZ, segmentsCount, segmentVerticesCount );
   m_geometry[2]->setMesh( *( m_meshes[2] ) );
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::createScalingGizmo()
{
   static FastFloat armLength = FastFloat::fromFloat( 0.2f );
   float armHalfDim = 0.003f;
   Vector ox, oy, oz;
   ox.setMul( Vector_OX, armLength );
   oy.setMul( Vector_OY, armLength );
   oz.setMul( Vector_OZ, armLength );

   // OX
   SAFE_DELETE( m_meshes[0] );
   m_meshes[0] = DebugGeometryBuilder::createBoxHeadedArrow( armHalfDim, Vector_ZERO, ox );
   m_geometry[0]->setMesh( *m_meshes[0] );

   // OY
   SAFE_DELETE( m_meshes[1] );
   m_meshes[1] = DebugGeometryBuilder::createBoxHeadedArrow( armHalfDim, Vector_ZERO, oy );
   m_geometry[1]->setMesh( *m_meshes[1] );

   // OZ
   SAFE_DELETE( m_meshes[2] );
   m_meshes[2] = DebugGeometryBuilder::createBoxHeadedArrow( armHalfDim, Vector_ZERO, oz );
   m_geometry[2]->setMesh( *m_meshes[2] );
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::show( bool visible )
{
   for ( byte i = 0; i < 3; ++i )
   {
      m_geometry[i]->show( visible );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::update( float timeElapsed )
{
   // adjust its position
   Matrix newGizmoTransform;
   if ( m_selectedRootEntities.size() == 1 )
   {
      // only one node's selected - use its transform
      newGizmoTransform = m_selectedRootEntities.front()->getGlobalMtx();
   }
   else
   {
      // calculate a bounding box around the selection and put the gizmo inside of it, without any rotation
      AxisAlignedBox bounds;
      calculateSelectionBounds( bounds );

      Vector selectionCenter;
      bounds.getCenter( selectionCenter );

      newGizmoTransform.setTranslation( selectionCenter );
   }

   switch ( m_gizmoReferenceFrame )
   {
   case GRF_Local:
   {
      setLocalMtx( newGizmoTransform );
      break;
   }

   case GRF_World:
   {
      accessLocalMtx().setTranslation( newGizmoTransform.position() );
      break;
   }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::onSelectionChanged( SelectionManager* selectionMgr )
{
   const List< SceneNode* >& nodes = selectionMgr->getSelectedNodes();

   bool hasSelectionChanged = updateSelectedNodesList( nodes );
   if ( hasSelectionChanged )
   {
      // toggle gizmo's visibility
      show( !m_selectedRootEntities.empty() );
   }
}

///////////////////////////////////////////////////////////////////////////////

bool Gizmo::updateSelectedNodesList( const List< SceneNode* >& nodes )
{
   // We can only manipulate top level prefabs embedded in a scene.
   // So we want to look for the topmost entity with a prefab.
   // Unless of course this entity is not prefabbed at all, in which case grab the parent entity and go :)

   List< Entity* > newSelection;
   for ( List< SceneNode* >::const_iterator it = nodes.begin(); !it.isEnd(); ++it )
   {
      SceneNode* node = *it;
      Entity* prefabbedEntity = EntityUtils::getTopmostPrefabbedParent( node );
      if ( prefabbedEntity )
      {
         newSelection.pushBack( prefabbedEntity );
      }
      else
      {
         Entity* parentEntity = EntityUtils::getFirstParent< Entity >( node );
         if ( parentEntity )
         {
            newSelection.pushBack( parentEntity );
         }
      }
   }

   List< Entity* > addedNodes;
   List< Entity* > removedNodes;
   bool hasSelectionChanged = ListUtils::compareWithPrevious( m_selectedRootEntities, newSelection, addedNodes, removedNodes );
   if ( !hasSelectionChanged )
   {
      // nothing has changed
      return false;
   }

   // memorize new selection
   m_selectedRootEntities = newSelection;

   return true;
}

///////////////////////////////////////////////////////////////////////////////

void Gizmo::calculateSelectionBounds( AxisAlignedBox& outBounds ) const
{
   for ( List< Entity* >::const_iterator it = m_selectedRootEntities.begin(); !it.isEnd(); ++it )
   {
      Entity* node = *it;

      const Vector& pos = node->getGlobalMtx().position();
      outBounds.include( pos );
   }
}

///////////////////////////////////////////////////////////////////////////////

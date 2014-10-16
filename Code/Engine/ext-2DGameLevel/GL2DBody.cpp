#include "ext-2DGameLevel\GL2DBody.h"
#include "ext-2DGameLevel\GL2DWorld.h"
#include "ext-2DGameLevel\GL2DLineSegmentsShape.h"
#include "core-MVC\Entity.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( GL2DBody );
   PARENT( Component );
   PROPERTY_EDIT( "Shape", GL2DShape*, m_shape );
   PROPERTY_EDIT( "Is movable", bool, m_isMovable );
END_OBJECT( );

///////////////////////////////////////////////////////////////////////////////

GL2DBody::GL2DBody( const char* name )
   : Component( name )
   , m_isMovable( true )
   , m_shape( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

GL2DBody::GL2DBody( const GL2DBody& rhs )
   : Component( rhs )
   , m_isMovable( rhs.m_isMovable )
   , m_shape( NULL )
{
   if ( rhs.m_shape )
   {
      m_shape = rhs.m_shape->getVirtualRTTI( ).instantiate( *rhs.m_shape );
   }
}

///////////////////////////////////////////////////////////////////////////////

GL2DBody::~GL2DBody( )
{
   if ( m_shape )
   {
      m_shape->removeReference( );
      m_shape = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBody::onPrePropertyChanged( ReflectionProperty& property )
{
   Component::onPrePropertyChanged( property );

   if ( property.getName( ) == "m_shape" )
   {
      if ( m_shape )
      {
         // we don't delete the reference here - it will either be handled
         // by the ReflectionObjectEditor, or the setShape. Here, we simply clean up
         m_shape = NULL;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBody::setShape( GL2DShape* shape )
{
   // delete the old shape
   if ( m_shape )
   {
      m_shape->removeReference( );
      m_shape = NULL;
   }

   NOTIFY_PROPERTY_CHANGE( m_shape );
   m_shape = shape;

   // linear segment shapes make static bodies by default
   if ( m_shape )
   {
      bool movable = !m_shape->isExactlyA< GL2DLineSegmentsShape >();
      m_isMovable &= movable;
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBody::setMovable( bool movable )
{
   // linear segment shapes make static bodies by default - if one is set, don't change the setting
   if ( m_shape && m_shape->isExactlyA< GL2DLineSegmentsShape >( ) )
   {
      ASSERT( !m_isMovable ); // just verifying that the flag is set to a correct value
      return;
   }


   NOTIFY_PROPERTY_CHANGE( m_isMovable );
   m_isMovable = movable;

   if ( !m_isMovable )
   {
      m_linearVelocity.setZero();
      m_angularVelocity.setZero();
   }
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBody::initialize( GL2DWorld& world )
{
   // reset runtime data before we start simulating the body
   m_linearVelocity.setZero( );
   m_angularVelocity.setZero( );

   m_newIntegratedTransform = getGlobalMtx( );
   updateTransforms( );
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBody::deinitialize( GL2DWorld& world )
{
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBody::integrate( const FastFloat& timeElapsed, const Matrix& entityMtx )
{
   Quaternion orient;
   entityMtx.getRotation( orient );
   orient.addScaledVector( m_angularVelocity, timeElapsed );

   Vector displacement;
   displacement.setMulAdd( m_linearVelocity, timeElapsed, entityMtx.position( ) );

   m_newIntegratedTransform.setRotation( orient );
   m_newIntegratedTransform.setPosition<3>( displacement );
}

///////////////////////////////////////////////////////////////////////////////

bool GL2DBody::rayCast( const Ray& ray, const Matrix& transform, RaycastResult& outResult ) const
{
   bool didCollide = m_shape->rayCast( ray, transform, outResult );
   return didCollide;
}

///////////////////////////////////////////////////////////////////////////////

void GL2DBody::updateTransforms( )
{
}

///////////////////////////////////////////////////////////////////////////////

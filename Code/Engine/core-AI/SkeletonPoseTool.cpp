#include "core-AI\SkeletonPoseTool.h"
#include "core-AI\Skeleton.h"


///////////////////////////////////////////////////////////////////////////////

SkeletonPoseTool::SkeletonPoseTool( Skeleton& skeleton )
   : m_skeleton( skeleton )
   , m_poseLocal( skeleton.getBoneCount() )
   , m_poseModel( skeleton.getBoneCount() )
   , m_modelDirty( false )
   , m_localDirty( false )
{
   m_poseLocal.resize( skeleton.getBoneCount(), Transform::IDENTITY );
   m_poseModel.resize( skeleton.getBoneCount(), Transform::IDENTITY );
}

///////////////////////////////////////////////////////////////////////////////

SkeletonPoseTool& SkeletonPoseTool::startSkeleton( const char* boneName, const Vector& rotationAxis, float angle, const Vector& translation )
{
   Transform boneModelSpace;
   boneModelSpace.set( rotationAxis, angle, translation );
   m_boneParents.pushBack( std::string( "" ) );
   m_skeleton.addBone( boneName, boneModelSpace, -1, 1.0f );

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonPoseTool& SkeletonPoseTool::bone( const char* boneName, const char* parentBoneName, const Vector& rotationAxis, float angle, const Vector& translation )
{
   Transform boneModelSpace;
   boneModelSpace.set( rotationAxis, angle, translation );
   m_boneParents.pushBack( std::string( parentBoneName ) );
   m_skeleton.addBone( boneName, boneModelSpace, -1, 1.0f );

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonPoseTool::buildSkeleton()
{
   // first - define the order of bones
   uint boneIdx = 0;
   for ( List< std::string >::iterator it = m_boneParents.begin(); !it.isEnd(); ++it, ++boneIdx )
   {
      const std::string& parentBoneName = *it;
      if ( parentBoneName.length() > 0 )
      {
         const int parentBoneIdx = m_skeleton.getBoneIndex( parentBoneName.c_str() );
         m_skeleton.m_boneParentIndices[boneIdx] = parentBoneIdx;
      }
   }

   // resize our local arrays
   const uint boneCount = m_skeleton.getBoneCount();
   m_poseLocal.resize( boneCount, Transform::IDENTITY );
   m_poseModel.resize( boneCount, Transform::IDENTITY );

   // next - calculate the local space transform from the model transforms we set on the skeleton
   for ( uint i = 0; i < boneCount; ++i )
   {
      m_poseModel[i].set( m_skeleton.m_boneLocalMatrices[i] );
   }
   m_skeleton.calculateModelToLocal( m_poseModel.getRaw(), m_poseLocal.getRaw() );

   // set the local pose on the skeleton
   for ( uint i = 0; i < boneCount; ++i )
   {
      m_poseLocal[i].toMatrix( m_skeleton.m_boneLocalMatrices[i] );
   }

   // compile the skeleton
   m_skeleton.buildSkeleton();

   // cleanup
   m_boneParents.clear();
}

///////////////////////////////////////////////////////////////////////////////

SkeletonPoseTool& SkeletonPoseTool::start()
{
   const uint boneCount = m_poseLocal.size();
   for ( uint i = 0; i < boneCount; ++i )
   {
      m_poseLocal[i].set( m_skeleton.m_boneLocalMatrices[i] );
   }

   m_skeleton.calculateLocalToModel( m_poseLocal.getRaw(), m_poseModel.getRaw() );
   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonPoseTool& SkeletonPoseTool::rotate( const char* boneName, const Vector& rotationAxis, float angle )
{
   const int boneIdx = m_skeleton.getBoneIndex( boneName );
   ASSERT( boneIdx >= 0 );

   Transform t;
   t.m_rotation.setAxisAngle( rotationAxis, angle );
   m_poseLocal[boneIdx].preMul( t );

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonPoseTool& SkeletonPoseTool::translate( const char* boneName, const Vector& translation )
{
   const int boneIdx = m_skeleton.getBoneIndex( boneName );
   ASSERT( boneIdx >= 0 );

   Transform t;
   t.m_translation = translation;
   m_poseLocal[boneIdx].preMul( t );

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

SkeletonPoseTool& SkeletonPoseTool::rotateAndTranslate( const char* boneName, const Vector& rotationAxis, float angle, const Vector& translation )
{
   const int boneIdx = m_skeleton.getBoneIndex( boneName );
   ASSERT( boneIdx >= 0 );

   Transform t;
   t.set( rotationAxis, angle, translation );
   m_poseLocal[boneIdx].mul( t );

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonPoseTool::end()
{
   // it's the local transform we need to synchronize here
   m_modelDirty = true;
   syncPoses();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonPoseTool::syncPoses() const
{
   ASSERT_MSG( !( m_modelDirty && m_localDirty ), "Bonth the model and the local poses can't be out of date at the same time" );
   if ( m_modelDirty )
   {
      m_skeleton.calculateLocalToModel( m_poseLocal.getRaw(), m_poseModel.getRaw() );
      m_modelDirty = false;
   }
   else if ( m_localDirty )
   {
      m_skeleton.calculateModelToLocal( m_poseModel.getRaw(), m_poseLocal.getRaw() );
      m_localDirty = false;
   }
}

///////////////////////////////////////////////////////////////////////////////

const Transform* SkeletonPoseTool::getLocal() const
{
   syncPoses();
   return m_poseLocal.getRaw();
}

///////////////////////////////////////////////////////////////////////////////

Transform* SkeletonPoseTool::accessLocal()
{
   syncPoses();
   m_modelDirty = true;
   return m_poseLocal.getRaw();
}

///////////////////////////////////////////////////////////////////////////////

const Transform* SkeletonPoseTool::getModel() const
{
   syncPoses();
   return m_poseModel.getRaw();
}

///////////////////////////////////////////////////////////////////////////////

Transform* SkeletonPoseTool::accessModel()
{
   syncPoses();
   m_localDirty = true;
   return m_poseModel.getRaw();
}

///////////////////////////////////////////////////////////////////////////////

bool SkeletonPoseTool::testBoneModel( const char* boneName, const Vector& rotationAxis, float angle, const Vector& translation )
{
   syncPoses();

   const int boneIdx = m_skeleton.getBoneIndex( boneName );
   if ( boneIdx < 0 )
   {
      char tmpStr[256];
      sprintf_s( tmpStr, "Bone %s doesn't exist", boneName );
      m_errorMsg = tmpStr;

      return false;
   }

   const Transform& actual = m_poseModel[boneIdx];

   Transform expected;
   expected.set( rotationAxis, angle, translation );

   if ( ( fabs( expected.m_translation[0] - actual.m_translation[0] ) > 0.0001f ) ||
        ( fabs( expected.m_translation[1] - actual.m_translation[1] ) > 0.0001f ) ||
        ( fabs( expected.m_translation[2] - actual.m_translation[2] ) > 0.0001f ) )
   {
      char tmpStr[256];
      sprintf_s( tmpStr, "%s translation is incorrect. Expected [%.4f, %.4f, %.4f] != Actual [%.4f, %.4f, %.4f]",
                 boneName, 
                 expected.m_translation[0],
                 expected.m_translation[1],
                 expected.m_translation[2],
                 actual.m_translation[0],
                 actual.m_translation[1],
                 actual.m_translation[2] );
      m_errorMsg = tmpStr;
      return false;
   }

   if ( ( fabs( expected.m_rotation[0] - actual.m_rotation[0] ) > 0.0001f ) ||
        ( fabs( expected.m_rotation[1] - actual.m_rotation[1] ) > 0.0001f ) ||
        ( fabs( expected.m_rotation[2] - actual.m_rotation[2] ) > 0.0001f ) ||
        ( fabs( expected.m_rotation[3] - actual.m_rotation[3] ) > 0.0001f ) )
   {
      char tmpStr[256];
      sprintf_s( tmpStr, "%s rotation is incorrect. Expected [%.4f, %.4f, %.4f, %.4f] != Actual [%.4f, %.4f, %.4f, %.4f]",
                 boneName,
                 expected.m_rotation[0],
                 expected.m_rotation[1],
                 expected.m_rotation[2],
                 expected.m_rotation[3],
                 actual.m_rotation[0],
                 actual.m_rotation[1],
                 actual.m_rotation[2],
                 actual.m_rotation[3] );
      m_errorMsg = tmpStr;
      return false;
   }

   return true;
}

///////////////////////////////////////////////////////////////////////////////

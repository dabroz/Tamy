/// @file   core-Physics\PxMathConverter.h
/// @brief  math types conversion tool
#pragma once


///////////////////////////////////////////////////////////////////////////////

struct Vector;
struct Quaternion;
struct Matrix;

namespace physx
{
   class PxVec3;
   struct PxExtendedVec3;
   class PxQuat;
   class PxTransform;
}

///////////////////////////////////////////////////////////////////////////////

class PxMathConverter
{
public:
   /**
    * Vector conversion Booster -> PhysX
    *
    * @param boosterVec
    * @param outPhysXVec
    */
   static void convert( const Vector& boosterVec, physx::PxVec3& outPhysXVec );

   /**
   * Vector conversion PhysX -> Booster
   *
   * @param physXVec
   * @param outBoosterVec
   */
   static void convert( const physx::PxVec3& physXVec, Vector& outBoosterVec );

   /**
   * Vector conversion Booster -> PhysX
   *
   * @param boosterVec
   * @param outPhysXVec
   */
   static void convert( const Vector& boosterVec, physx::PxExtendedVec3& outPhysXVec );

   /**
   * Vector conversion PhysX -> Booster
   *
   * @param physXVec
   * @param outBoosterVec
   */
   static void convert( const physx::PxExtendedVec3& physXVec, Vector& outBoosterVec );

   /**
   * Quaternion conversion Booster -> PhysX
   *
   * @param boosterQuat
   * @param outPhysXQuat
   */
   static void convert( const Quaternion& boosterQuat, physx::PxQuat& outPhysXQuat );

   /**
   * Quaternion conversion PhysX -> Booster
   *
   * @param physXQuat
   * @param outBoosterQuat
   */
   static void convert( const physx::PxQuat& physXQuat, Quaternion& outBoosterQuat );

   /**
   * Transform conversion Booster -> PhysX
   *
   * @param boosterMtx
   * @param outPhysXTransform
   */
   static void convert( const Matrix& boosterMtx, physx::PxTransform& outPhysXTransform );

   /**
   * Transform conversion PhysX -> Booster
   *
   * @param physXTransform
   * @param outBoosterMtx
   */
   static void convert( const physx::PxTransform& physXTransform, Matrix& outBoosterMtx );
};

///////////////////////////////////////////////////////////////////////////////

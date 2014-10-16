/// @file   core-Renderer/CascadedShadowsUtils.h
/// @brief  a set of tools used to calculate a cascaded shadow map
#pragma once

#include "core/AxisAlignedBox.h"
#include "core/Matrix.h"
#include "core/MemoryRouter.h"
#include "core-Renderer/Viewport.h"


///////////////////////////////////////////////////////////////////////////////

class Camera;
struct Vector;

///////////////////////////////////////////////////////////////////////////////

struct CascadesConfig
{
   DECLARE_ALLOCATOR( CascadesConfig, AM_ALIGNED_16 );

   // runtime data
   Camera*           m_activeCamera;
   Matrix            m_lightRotationMtx;
   AxisAlignedBox     m_sceneBounds;

   // static data
   int               m_numCascades;
   int               m_numCascadesPerTextureRow;
   float             m_pcfBlurSize;
   float             m_cascadeDimensions;
   float*            m_cascadeIntervals;

   CascadesConfig( int numCascades, int numCascadesPerTextureRow, float pcfBlurSize );
   ~CascadesConfig();
};

///////////////////////////////////////////////////////////////////////////////

struct CascadeStage
{
   DECLARE_ALLOCATOR( CascadeStage, AM_ALIGNED_16 );

   Viewport             m_viewport;
   AxisAlignedBox        m_objectsQueryBounds;
   AxisAlignedBox        m_lightFrustumBounds;

   // temporary position and rotation we should set for the light for the time of cascade rendering
   Matrix               m_lightMtx;

   float                m_cameraNearZ;
   float                m_cameraFarZ;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A set of tools used to calculate a cascaded shadow map.
 */
class CascadedShadowsUtils
{
public:
   /**
    * Calculates the cascaded shadow rendering settings.
    *
    * @param config
    * @param outArrCascadeStages
    */
   static void calculateCascadesBounds( const CascadesConfig& config, CascadeStage* outArrCascadeStages);

private:
   static void calculateCameraCascadeFrustumBounds( Camera* activeCamera, float intervalBegin, float intervalEnd, AxisAlignedBox& outFrustumPart );

   static void createBBPoints( const AxisAlignedBox& inAABB, Vector* outBBPoints );

   static void calculateLightClippingPlanes( const Vector* sceneBBInLightSpace, AxisAlignedBox& inOutLightFrustumBounds );
};

///////////////////////////////////////////////////////////////////////////////

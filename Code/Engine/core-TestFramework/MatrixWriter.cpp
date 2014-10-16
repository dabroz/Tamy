#include "core-TestFramework\MatrixWriter.h"
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream &os, const D3DXMATRIX& mtx)
{
   os << "[[" << mtx._11 << ", " << mtx._12 << ", " << mtx._13 << ", " << mtx._14 <<
        "] [" << mtx._21 << ", " << mtx._22 << ", " << mtx._23 << ", " << mtx._24 <<
        "] [" << mtx._31 << ", " << mtx._32 << ", " << mtx._33 << ", " << mtx._34 <<
        "] [" << mtx._41 << ", " << mtx._42 << ", " << mtx._43 << ", " << mtx._44 << "]]";
   return os;
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream &os, const D3DXVECTOR2& vec)
{
   os << "[" << vec.x << ", " << vec.y << "]";
   return os;
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream &os, const D3DXVECTOR3& vec)
{
   os << "[" << vec.x << ", " << vec.y << ", " << vec.z << "]";
   return os;
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream &os, const D3DXQUATERNION& quat)
{
   float x = (fabs(quat.x) < 0.00001f) ? 0 : quat.x;
   float y = (fabs(quat.y) < 0.00001f) ? 0 : quat.y;
   float z = (fabs(quat.z) < 0.00001f) ? 0 : quat.z;
   float w = (fabs(quat.w) < 0.00001f) ? 0 : quat.w;
   os << "[" << x << ", " << y << ", " << z << ", " << w << "]";
   return os;
}

///////////////////////////////////////////////////////////////////////////////

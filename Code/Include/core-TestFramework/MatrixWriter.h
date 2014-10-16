#pragma once

#include <iostream>
#include <d3dx9.h>


///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream &os, const D3DXMATRIX& mtx);
std::ostream& operator<<(std::ostream &os, const D3DXVECTOR3& vec);
std::ostream& operator<<(std::ostream &os, const D3DXVECTOR2& vec);
std::ostream& operator<<(std::ostream &os, const D3DXQUATERNION& quat);

///////////////////////////////////////////////////////////////////////////////

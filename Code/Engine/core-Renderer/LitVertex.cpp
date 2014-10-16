#include "core-Renderer\LitVertex.h"
#include "core\InStream.h"
#include "core\OutStream.h"


///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const LitVertex& vtx )
{
   stream << vtx.m_coords;
   stream << vtx.m_normal;
   stream << vtx.m_tangent;
   stream << vtx.m_textureCoords;
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, LitVertex& vtx )
{
   stream >> vtx.m_coords;
   stream >> vtx.m_normal;
   stream >> vtx.m_tangent;
   stream >> vtx.m_textureCoords;
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

OutStream& operator<<( OutStream& stream, const VertexWeight& weight )
{
   stream << weight.m_weights;
   stream << weight.m_indices;
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

InStream& operator>>( InStream& stream, VertexWeight& weight )
{
   stream >> weight.m_weights;
   stream >> weight.m_indices;
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& stream, const LitVertex& vertex)
{
   stream << "[" << vertex.m_coords << "; " << vertex.m_normal << "; " << vertex.m_tangent <<"]";

   return stream;
}

///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& stream, const VertexWeight& vertex)
{
   stream << "[" << vertex.m_indices << "; " << vertex.m_weights << "]";
   return stream;
}

///////////////////////////////////////////////////////////////////////////////

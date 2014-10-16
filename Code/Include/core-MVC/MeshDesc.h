#pragma once

/// @file   core-Scene\MeshDesc.h
/// @brief  description of a mesh

#include "core\Serializable.h"
#include <vector>
#include <list>
#include <d3dx9.h>
#include <string>
#include "core-Renderer\LitVertex.h"
#include "core-Renderer\Face.h"


///////////////////////////////////////////////////////////////////////////////

typedef std::vector<std::string> BonesInfluenceDefinition;

///////////////////////////////////////////////////////////////////////////////

struct SkinBoneDefinition : public Serializable
{
   std::string name;
   // the bone offset matrix is unique for a bone, and not for a vertex that
   // references the bone in the skin
   D3DXMATRIX boneOffset;

   SkinBoneDefinition() 
   {
      D3DXMatrixIdentity(&boneOffset);
   }

   SkinBoneDefinition(const std::string& _name,
      const D3DXMATRIX& _boneOffset)
      : name(_name), boneOffset(_boneOffset)
   {
   }

   // -------------------------------------------------------------------------
   // Serializable implementation
   // -------------------------------------------------------------------------
   void save(Serializer& serializer)
   {
      serializer.saveString(name);
      serializer.saveMatrix(boneOffset);
   }

   void load(Serializer& serializer)
   {
      serializer.loadString(name);
      serializer.loadMatrix(boneOffset);
   }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This class describes a mesh.
 */
struct MeshDesc : public Serializable
{
   std::string m_name;
   bool m_isSkin;

   std::vector<std::string> m_materials;
   std::vector<LitVertex> m_vertices;
   std::vector<Face> m_faces;

   // these two attributes have meaning only if the mesh is a skin
   std::vector<BonesInfluenceDefinition> m_bonesInfluencingAttribute;
   std::vector<SkinBoneDefinition> m_skinBones;

   D3DXMATRIX m_localMtx;

   MeshDesc* m_parent;
   std::list<MeshDesc*> m_children;

   /**
    * Default constructor.
    */
   MeshDesc();

   /**
    * Copy constructor.
    */
   MeshDesc(const MeshDesc& rhs);

   ~MeshDesc();

   MeshDesc& operator=(const MeshDesc& rhs);

   // -------------------------------------------------------------------------
   // Serializable implementation
   // -------------------------------------------------------------------------
   void save(Serializer& serializer);

   void load(Serializer& serializer);
};

///////////////////////////////////////////////////////////////////////////////

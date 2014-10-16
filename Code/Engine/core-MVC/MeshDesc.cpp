#include "core-Scene\MeshDesc.h"
#include "core\Serializer.h"


///////////////////////////////////////////////////////////////////////////////

MeshDesc::MeshDesc() 
: m_isSkin(false)
, m_parent(NULL) 
{
   D3DXMatrixIdentity(&m_localMtx);
}

///////////////////////////////////////////////////////////////////////////////

MeshDesc::MeshDesc(const MeshDesc& rhs)
: m_name(rhs.m_name)
, m_isSkin(rhs.m_isSkin)
, m_materials(rhs.m_materials)
, m_vertices(rhs.m_vertices)
, m_faces(rhs.m_faces)
, m_bonesInfluencingAttribute(rhs.m_bonesInfluencingAttribute)
, m_skinBones(rhs.m_skinBones)
, m_localMtx(rhs.m_localMtx)
, m_parent(rhs.m_parent)
{
   for (std::list<MeshDesc*>::const_iterator it = rhs.m_children.begin();
      it != rhs.m_children.end(); ++it)
   {
      m_children.push_back(new MeshDesc(**it));
   }
}

///////////////////////////////////////////////////////////////////////////////

MeshDesc::~MeshDesc()
{
   for (std::list<MeshDesc*>::iterator it = m_children.begin();
      it != m_children.end(); ++it)
   {
      delete *it;
   }
   m_children.clear();
}

///////////////////////////////////////////////////////////////////////////////

MeshDesc& MeshDesc::operator=(const MeshDesc& rhs)
{
   m_name = rhs.m_name;
   m_isSkin = rhs.m_isSkin;
   m_materials = rhs.m_materials;
   m_vertices = rhs.m_vertices;
   m_faces = rhs.m_faces;
   m_bonesInfluencingAttribute = rhs.m_bonesInfluencingAttribute;
   m_skinBones = rhs.m_skinBones;
   m_localMtx = rhs.m_localMtx;
   m_parent = rhs.m_parent;

   for (std::list<MeshDesc*>::iterator it = m_children.begin();
      it != m_children.end(); ++it)
   {
      delete *it;
   }
   m_children.clear();

   for (std::list<MeshDesc*>::const_iterator it = rhs.m_children.begin();
      it != rhs.m_children.end(); ++it)
   {
      m_children.push_back(new MeshDesc(**it));
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

void MeshDesc::save(Serializer& serializer)
{
   serializer.saveString(m_name);
   serializer.saveBool(m_isSkin);

   unsigned int count = m_materials.size();
   serializer.saveInt(count);
   for (unsigned int i = 0; i < count; ++i)
   {
      serializer.saveString(m_materials[i]);
   }

   count = m_vertices.size();
   serializer.saveInt(count);
   for (unsigned int i = 0; i < count; ++i)
   {
      m_vertices[i].save(serializer);
   }

   count = m_faces.size();
   serializer.saveInt(count);
   for (unsigned int i = 0; i < count; ++i)
   {
      m_faces[i].save(serializer);
   }

   count = m_bonesInfluencingAttribute.size();
   serializer.saveInt(count);
   for (unsigned int i = 0; i < count; ++i)
   {
      unsigned int namesCount = m_bonesInfluencingAttribute[i].size();
      for (unsigned int j = 0; j < namesCount; ++j)
      {
         serializer.saveString(m_bonesInfluencingAttribute[i][j]);
      }
   }

   count = m_skinBones.size();
   serializer.saveInt(count);
   for (unsigned int i = 0; i < count; ++i)
   {
      m_skinBones[i].save(serializer);
   }

   serializer.saveMatrix(m_localMtx);

   serializer.saveInt(m_children.size());
   for (std::list<MeshDesc*>::iterator it = m_children.begin();
        it != m_children.end();
        ++it)
   {
      (*it)->save(serializer);
   }
}

///////////////////////////////////////////////////////////////////////////////

void MeshDesc::load(Serializer& serializer)
{
   std::string tmpStr;

   serializer.loadString(m_name);
   m_isSkin = serializer.loadBool();

   unsigned int count = serializer.loadInt();
   for (unsigned int i = 0; i < count; ++i)
   {
      serializer.loadString(tmpStr);
      m_materials.push_back(tmpStr);
   }

   count = serializer.loadInt();
   for (unsigned int i = 0; i < count; ++i)
   {
      m_vertices.push_back(LitVertex());
      m_vertices.back().load(serializer);
   }

   count = serializer.loadInt();
   for (unsigned int i = 0; i < count; ++i)
   {
      m_faces.push_back(Face());
      m_faces.back().load(serializer);
   }

   count = serializer.loadInt();
   for (unsigned int i = 0; i < count; ++i)
   {
      BonesInfluenceDefinition& influencingBones = m_bonesInfluencingAttribute[i];

      unsigned int namesCount = serializer.loadInt();
      for (unsigned int j = 0; j < namesCount; ++j)
      {
         serializer.loadString(tmpStr);
         influencingBones.push_back(tmpStr);
      }
   }

   count = serializer.loadInt();
   for (unsigned int i = 0; i < count; ++i)
   {
      m_skinBones.push_back(SkinBoneDefinition());
      m_skinBones.back().load(serializer);
   }

   serializer.loadMatrix(m_localMtx);

   count = serializer.loadInt();
   for (unsigned int i = 0; i < count; ++i)
   {
      MeshDesc* newChild = new MeshDesc();
      m_children.push_back(newChild);
      newChild->load(serializer);
      newChild->m_parent = this;
   }
}

///////////////////////////////////////////////////////////////////////////////

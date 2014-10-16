/// @file   TamyEditor\PhysicsMaterialEditor.h
/// @brief  physics material editor
#pragma once

#include "ResourceEditor.h"


///////////////////////////////////////////////////////////////////////////////

class PhysicsMaterial;

///////////////////////////////////////////////////////////////////////////////

class PhysicsMaterialEditor : public ResourceEditor
{
   Q_OBJECT

private:
   PhysicsMaterial&           m_material;

public:
   /**
    * Constructor.
    *
    * @param material
    */
   PhysicsMaterialEditor( PhysicsMaterial& material );

protected:
   // -------------------------------------------------------------------------
   // ResourceEditor implementation
   // -------------------------------------------------------------------------
   void onInitialize();
   void onDeinitialize( bool saveProgress );

public slots:
   void save();
};

///////////////////////////////////////////////////////////////////////////////


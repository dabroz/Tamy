/// @file   TamyEditor\ProceduralLevelGenerator.h
/// #brief  procedural level generator front end
#pragma once

#include <QtWidgets\QFrame>
#include "core\ReflectionObject.h"


///////////////////////////////////////////////////////////////////////////////

class GL2DProceduralLevel;
class Entity;
class QLabel;

///////////////////////////////////////////////////////////////////////////////

class ProceduralLevelGenerator : public QFrame
{
   Q_OBJECT

private:
   GL2DProceduralLevel*                m_params;
   Entity*                             m_root;

   QLabel*                             m_status;

public:
   /**
   * Constructor.
   *
   * @param parent
   * @param params
   */
   ProceduralLevelGenerator( QWidget* parent, GL2DProceduralLevel* params );
   ~ProceduralLevelGenerator();

public slots:
   void generateScene();
};

///////////////////////////////////////////////////////////////////////////////

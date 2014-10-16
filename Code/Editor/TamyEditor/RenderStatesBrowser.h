/// @file   TamyEditor/RenderStatesBrowser.h
/// @brief  browses and edits RenderStates attached to a GeometryComponent
#pragma once

#include <QtWidgets\QFrame>
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class GeometryComponent;
class QPropertiesView;

///////////////////////////////////////////////////////////////////////////////

/**
 * Browses and edits RenderStates attached to a GeometryComponent.
 */
class RenderStatesBrowser : public QFrame
{
   Q_OBJECT

private:
   GeometryComponent&      m_geometryComponent;

   QPropertiesView*        m_renderStateView;

public:
   /**
    * Constructor.
    *
    * @param parent
    * @param geometryComponent
    */
   RenderStatesBrowser( QWidget* parent, GeometryComponent& geometryComponent );
   ~RenderStatesBrowser();
};

///////////////////////////////////////////////////////////////////////////////

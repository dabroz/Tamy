/// @file   TamyEditor\QPropertiecView.h
/// @brief  properties browser widget
#pragma once

#include "core\ReflectionPropertiesView.h"
#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class TamyEditor;
class QVBoxLayout;
class QReflectionObjectEditor;

///////////////////////////////////////////////////////////////////////////////

/**
 * Properties browser widget.
 */
class QPropertiesView : public QFrame, public ReflectionPropertiesView
{
   Q_OBJECT

private:
   QVBoxLayout*                  m_layout;

   QReflectionObjectEditor*      m_rootEditor;

public:
   /**
    * Constructor.
    */
   QPropertiesView();
   ~QPropertiesView();

   /**
    * Returns a pointer to the edited object.
    */
   ReflectionObject* getEditedObject();

protected:
   void onSet( ReflectionObjectEditor* rootEditor );

private:
   void initFactory();
};

///////////////////////////////////////////////////////////////////////////////

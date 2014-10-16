/// @file   TamyEditor/SerializableWidget.h
/// @brief  marker interface of serializable widgets
#pragma once


///////////////////////////////////////////////////////////////////////////////

class QSettings;

///////////////////////////////////////////////////////////////////////////////

/**
 * Marker interface of serializable widgets.
 */
class SerializableWidget
{
public:
   virtual ~SerializableWidget() {}

   /**
    * Save the widget's layout.
    *
    * @param settings
    */
   virtual void saveLayout( QSettings& settings ) = 0;

   /**
    * Restore the widget's layout.
    *
    * @param settings
    */
   virtual void loadLayout( QSettings& settings ) = 0;
};

///////////////////////////////////////////////////////////////////////////////

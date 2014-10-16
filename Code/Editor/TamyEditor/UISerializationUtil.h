/// @file   TamyEditor/UISerializationUtil.h
/// @brief  serialization tool for the user interface
#pragma once


///////////////////////////////////////////////////////////////////////////////

class TamyEditor;
class QWidget;
class QDockWidget;
class QTreeWidget;
class QTabWidget;
class QSettings;
class MainEditorPanel;

///////////////////////////////////////////////////////////////////////////////

class UISerializationUtil
{
public:
   static void serialize( TamyEditor* rootWidget, MainEditorPanel* editorsPanel, bool save );

protected:
   static void serializeWidgetSettings( QWidget& widget, QSettings& outSettings, bool save );
   static void serializeDockWidgetSettings( QDockWidget& widget, QSettings& outSettings, bool save );
   static void serializeTreeWidgetSettings( QTreeWidget& widget, QSettings& outSettings, bool save );
   static void serializeTabWidgetSettings( QTabWidget& widget, QSettings& outSettings, bool save );
};


///////////////////////////////////////////////////////////////////////////////

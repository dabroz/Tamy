/// @file   TamyEditor\ResourceEditor.h
/// @brief  common interface for all abstract editors
#pragma once

#include <QtWidgets\QFrame>
#include <QtGui\QIcon>

///////////////////////////////////////////////////////////////////////////////

class TamyEditor;
class AsyncOperation;
class CriticalSection;
class QTimer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Common interface for all abstract editors.
 */
class ResourceEditor : public QFrame
{
   Q_OBJECT

private:
   QTimer*           m_updateTimer;

   QString           m_label;
   QIcon             m_icon;
   bool              m_enabled;

public:
   /**
    * Constructor.
    */
   ResourceEditor();
   virtual ~ResourceEditor();

   /**
    * This method will be called once the editor is created to initialize
    * and start it up.
    */
   void initialize( const QString& label, const QIcon& icon );

   /**
    * Called when the editor is about to be destroyed, or when we generally
    * want it to save its state, 'cause it may be killed.
    *
    * @param saveProgress     is it safe for the editor to save its contents?
    */
   void deinitialize( bool saveProgress );

   /**
    * Returns an icon assigned to the editor.
    */
   inline QIcon getIcon() const { return m_icon; }

   /**
    * Returns the editor's label.
    */
   inline QString getLabel() const { return m_label; }

   /**
    * Assigns the editor a new label.
    */
   inline void setLabel( const char* label ) {
      m_label = label;
   }

protected:
   // -------------------------------------------------------------------------
   // Initialization callbacks
   // -------------------------------------------------------------------------
   /**
    * Editor specific initialization.
    */
   virtual void onInitialize() = 0;

   /**
    * Editor specific de-initialization.
    */
   virtual void onDeinitialize( bool saveProgress ) = 0;

   /**
    * Called when an async operation is finished
    */
   virtual void onAsyncOperationFinished( bool flag ) {}

   /**
    * Toggles the editor between the enabled and disabled states.
    *
    * @param flag
    */
   void enable( bool flag );
};

///////////////////////////////////////////////////////////////////////////////

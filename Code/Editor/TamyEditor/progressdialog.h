/// @file   TamyEditor\progressdialog.h
/// @brief  This class operates the main window's progress bar
#pragma once

#include "core\IProgressObserver.h"
#include <QtWidgets\QFrame>


///////////////////////////////////////////////////////////////////////////////

class QProgressBar;
class QLabel;
class CriticalSection;
class ToolsWindow;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class operates the main window's progress bar.
 */
class ProgressDialog : public QFrame, public IProgressObserver
{
   Q_OBJECT

private:
   QLabel*           m_statusLabel;
   QProgressBar*     m_progressBar;

   std::string       m_status;
   uint              m_range;
   uint              m_value;
   CriticalSection*  m_dataLock;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    */
   ProgressDialog( QWidget* parentWidget );
   ~ProgressDialog();

   /**
    * Updates the values on the widgets, synchronizing them with those
    * set from the worker thread.
    *
    * @return  'true' if the progress has reached 100%, 'false' otherwise
    */
   bool update();

   // -------------------------------------------------------------------------
   // IProgressObserver implementation
   // -------------------------------------------------------------------------
   void initialize( unsigned int elemsCount );
   void setStatus( const char* status );
   void advance();
   void cancel();
};

///////////////////////////////////////////////////////////////////////////////

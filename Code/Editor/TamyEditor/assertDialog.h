/// @file   TamyEditor/assertDialog.h
/// @brief  a dialog box with an info about an assertion

#include <QtWidgets\QDialog>
#include "ui_assertdialog.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

class AssertDialog : public QDialog
{
   Q_OBJECT

private:
   Ui::AssertDialog              m_ui;
   AssertionHandle&              m_assertHandle;

public:
   AssertDialog( const char* msg, AssertionHandle& assertHandle );

   static void createDialog( const char* msg, AssertionHandle& assertHandle );

public slots:
   void onBreak();
   void onIgnore();
   void onIgnoreAll();
};

///////////////////////////////////////////////////////////////////////////////

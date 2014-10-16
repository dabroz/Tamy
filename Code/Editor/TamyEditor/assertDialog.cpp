#include "assertDialog.h"
#include <QtWidgets\QApplication>


///////////////////////////////////////////////////////////////////////////////

AssertDialog::AssertDialog( const char* msg, AssertionHandle& assertHandle )
   : QDialog( NULL, 0 )
   , m_assertHandle( assertHandle )
{
   m_ui.setupUi( this );

   setModal( true );

   static char whereaboutsStr[1024];
   sprintf_s( whereaboutsStr, "( %s : %d ) %s", assertHandle.getFile(), assertHandle.getLine(), assertHandle.getFunction() );
   m_ui.whereabouts->setText( whereaboutsStr );

   m_ui.errorMessage->setText( msg );

   connect( m_ui.breakButton, SIGNAL( clicked() ), this, SLOT( onBreak() ) );
   connect( m_ui.ignoreButton, SIGNAL( clicked() ), this, SLOT( onIgnore() ) );
   connect( m_ui.ignoreAllButton, SIGNAL( clicked() ), this, SLOT( onIgnoreAll() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AssertDialog::onBreak()
{
   __asm int 3;
}

///////////////////////////////////////////////////////////////////////////////

void AssertDialog::onIgnore()
{
   close();
}

///////////////////////////////////////////////////////////////////////////////

void AssertDialog::onIgnoreAll()
{
   m_assertHandle.ignore();
   close();
}

///////////////////////////////////////////////////////////////////////////////

void AssertDialog::createDialog( const char* msg, AssertionHandle& assertHandle )
{
   AssertDialog dlg( msg, assertHandle );
   dlg.exec();
}

///////////////////////////////////////////////////////////////////////////////

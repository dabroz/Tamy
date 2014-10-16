#include "AssertFrame.h"
#include <QtWidgets\QApplication>
#include <QtWidgets\QTabWidget>
#include <QtGui\QCloseEvent>


///////////////////////////////////////////////////////////////////////////////

AssertFrame::AssertFrame( QTabWidget* parent, const char* whereabouts, const char* msg, TThreadSafe< EventAssert::Choice >& choiceFlag )
: QFrame( parent, 0 )
, m_parent( parent )
, m_choiceFlag( choiceFlag )
, m_answered( false )
{
   m_ui.setupUi( this );

   m_ui.whereabouts->setText( whereabouts );
   m_ui.errorMessage->setText( msg );

   connect( m_ui.breakButton, SIGNAL( clicked() ), this, SLOT( onBreak() ) );
   connect( m_ui.ignoreButton, SIGNAL( clicked() ), this, SLOT( onIgnore() ) );
   connect( m_ui.ignoreAllButton, SIGNAL( clicked() ), this, SLOT( onIgnoreAll() ) );
}

///////////////////////////////////////////////////////////////////////////////

AssertFrame::~AssertFrame()
{
   // if the user closed the event box with an X button, choose to ignore that one assertion only
   if ( !m_answered )
   {
      m_choiceFlag = EventAssert::EAC_IGNORE;
   }
}

///////////////////////////////////////////////////////////////////////////////

void AssertFrame::onBreak()
{
   m_answered = true;
   m_choiceFlag = EventAssert::EAC_BREAK;

   int idx = m_parent->indexOf( this );
   m_parent->removeTab( idx );
   delete this;
}

///////////////////////////////////////////////////////////////////////////////

void AssertFrame::onIgnore()
{
   m_answered = true;
   m_choiceFlag = EventAssert::EAC_IGNORE;
   
   int idx = m_parent->indexOf( this );
   m_parent->removeTab( idx );
   delete this;
}

///////////////////////////////////////////////////////////////////////////////

void AssertFrame::onIgnoreAll()
{
   m_answered = true;
   m_choiceFlag = EventAssert::EAC_IGNORE_ALL;
   
   int idx = m_parent->indexOf( this );
   m_parent->removeTab( idx );
   delete this;
}

///////////////////////////////////////////////////////////////////////////////

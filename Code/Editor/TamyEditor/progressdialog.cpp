#include "progressdialog.h"
#include "tamyeditor.h"
#include <QtWidgets\QProgressBar>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QLabel>
#include "core\CriticalSection.h"


///////////////////////////////////////////////////////////////////////////////

ProgressDialog::ProgressDialog( QWidget* parentWidget )
   : QFrame( parentWidget )
   , m_range( 0 )
   , m_value( 0 )
   , m_dataLock( new CriticalSection() )
{
   QHBoxLayout* layout = new QHBoxLayout( this );
   setLayout( layout );

   m_progressBar = new QProgressBar( this );
   layout->addWidget( m_progressBar );
   
   m_statusLabel = new QLabel( this );
   layout->addWidget( m_statusLabel );
}

///////////////////////////////////////////////////////////////////////////////

ProgressDialog::~ProgressDialog()
{
   delete m_dataLock;
   m_dataLock = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ProgressDialog::initialize( unsigned int elemsCount )
{
   m_dataLock->enter();
   m_range = elemsCount;
   m_value = 0;
   m_dataLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

void ProgressDialog::setStatus( const char* status )
{
   m_dataLock->enter();
   m_status = status;
   m_dataLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

void ProgressDialog::advance()
{
   m_dataLock->enter();
   ++m_value;
   m_dataLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

void ProgressDialog::cancel()
{
   m_dataLock->enter();
   m_value = m_range;
   m_dataLock->leave();
}

///////////////////////////////////////////////////////////////////////////////

bool ProgressDialog::update()
{
   m_dataLock->enter();

   m_statusLabel->setText( m_status.c_str() );
   m_progressBar->setRange( 0, m_range );
   m_progressBar->setValue( m_value );

   m_dataLock->leave();

   return m_range > 0 && m_value >= m_range;
}

///////////////////////////////////////////////////////////////////////////////

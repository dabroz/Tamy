#include "DebugMenuActions.h"
#include "DebugEntitiesManager.h"


///////////////////////////////////////////////////////////////////////////////

DebugEntitiesAction::DebugEntitiesAction( const char* label, DebugFeature toggledFeature, DebugEntitiesManager& debugEntitiesManager, QWidget* parentWidget )
   : QAction( label, parentWidget )
   , m_toggledFeature( toggledFeature )
   , m_debugEntitiesManager( debugEntitiesManager )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );

   bool featureState = m_debugEntitiesManager.getDebugDisplayState( m_toggledFeature );
   setCheckable( true );
   setChecked( featureState );
}

///////////////////////////////////////////////////////////////////////////////

void DebugEntitiesAction::onTriggered()
{
   bool newState = m_debugEntitiesManager.toggleDebugDisplay( m_toggledFeature );
   setChecked( newState );
}

///////////////////////////////////////////////////////////////////////////////

#include "AnimationEventsEditor.h"
#include "core-AI\BlendTreeAnimation.h"
#include "core-AI\BlendTreeAnimationEvent.h"
#include "core-AI\BlendTreeSyncPoint.h"
#include "core-AI\BlendTreeEvent.h"
#include "core-AI\BlendTree.h"
#include "core-AI\SnapshotAnimation.h"
#include "core-AI\SkeletonAnimationPlayer.h"
#include "BlendTreeEditor.h"
#include <QtWidgets\QSlider>
#include <QtGui\QPainter>
#include <QtGui\QPainterPath>
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\QGraphicsScene>
#include <QtWidgets\QMenu>
#include <QtGui\QMouseEvent>
#include <QtWidgets\QPushButton>
#include "QPropertiesView.h"
#include "core\ReflectionProperty.h"

#include "BTAEventItem.h"
#include "AnimationEventsTimeline.h"

#include "core\TimeController.h"


///////////////////////////////////////////////////////////////////////////////

AnimationEventsEditor::AnimationEventsEditor( BlendTreeAnimation& animationNode )
   : BTBFEditor( animationNode )
   , m_animationNode( animationNode )
   , m_animationToggled( false )
   , m_mainEditor( NULL )
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );
   setLayout( layout );

   // event properties view
   {
      m_eventProperties = new QPropertiesView();
      layout->addWidget( m_eventProperties );
   }

   // slider control frame
   QFrame* sliderControlFrame = new QFrame( this );
   {
      layout->addWidget( sliderControlFrame );

      QHBoxLayout* sliderControlFrameLayout = new QHBoxLayout( sliderControlFrame );
      sliderControlFrameLayout->setSpacing( 0 );
      sliderControlFrameLayout->setMargin( 0 );
      sliderControlFrame->setLayout( sliderControlFrameLayout );

      // simulation/animation toggle button
      m_animationToggleButton = new QPushButton( sliderControlFrame );
      {
         sliderControlFrameLayout->addWidget( m_animationToggleButton );
         m_animationToggleButton->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/linkInactive.png" ) );
         m_animationToggleButton->setMinimumSize( 54, 54 );
         m_animationToggleButton->setMaximumSize( 54, 54 );
         connect( m_animationToggleButton, SIGNAL( clicked( bool ) ), this, SLOT( onAnimationToggled( bool ) ) );
      }

      // slider frame
      QFrame* sliderFrame = new QFrame( sliderControlFrame );
      {
         sliderControlFrameLayout->addWidget( sliderFrame );

         QVBoxLayout* sliderFrameLayout = new QVBoxLayout( sliderFrame );
         sliderFrameLayout->setSpacing( 0 );
         sliderFrameLayout->setMargin( 0 );
         sliderFrame->setLayout( sliderFrameLayout );

         // events display
         {
            m_view = new AnimationEventsTimeline( sliderFrame );
            connect( m_view, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( onShowContextMenu( const QPoint& ) ) );
            sliderFrameLayout->addWidget( m_view );

            m_eventsScene = new QGraphicsScene();
            m_view->setScene( m_eventsScene );

            connect( m_eventsScene, SIGNAL( selectionChanged() ), this, SLOT( onItemSelected() ) );
         }

         // slider 
         m_slider = new QSlider( Qt::Horizontal, sliderFrame );
         {
            sliderFrameLayout->addWidget( m_slider, 0 );
         }
      }
   }

   // listen to changes in the node
   m_animationNode.attachListener( *this );

   m_animationNode.attachEventsListener( this );
   m_animationNode.pullEvents( this );

   // create a custom animation player set up to play the animation stored in the node
   m_customAnimationPlayer = new SkeletonAnimationPlayer();
   {
      SnapshotAnimation* anim = m_animationNode.getAnimation();
      m_customAnimationPlayer->setAnimationSource( anim );
   }

   // this class needs to be updated every frame to track the progress
   // of the animation player and display it on the time slider
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   timeController.add( *this );
}

///////////////////////////////////////////////////////////////////////////////

AnimationEventsEditor::~AnimationEventsEditor()
{
   TimeController& timeController = TSingleton< TimeController >::getInstance();
   timeController.remove( *this );

   m_animationNode.detachEventsListener( this );
   m_animationNode.detachListener( *this );

   m_customAnimationPlayer->removeReference();
   m_customAnimationPlayer = NULL;

   delete m_eventsScene;
   m_eventsScene = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::initialize( BlendTreeEditor& mainEditor )
{
   // setup the time slider
   {
      SnapshotAnimation* animation = m_animationNode.getAnimation();
      if ( animation )
      {
         // convert to milliseconds
         uint durationInMillisecs = animation->m_duration * 100.0f;
         m_slider->setMaximum( durationInMillisecs );
      }
      else
      {
         m_slider->setMaximum( 0 );
      }
    
      m_slider->setSliderPosition( 0 );
      
      connect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( timeValueChanged( int ) ) );

      m_view->setSlider( m_slider );
   }

   m_mainEditor = &mainEditor;
   m_animationToggled = false;
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::deinitialize( BlendTreeEditor& mainEditor )
{
   if ( m_animationToggled )
   {
      mainEditor.setAnimationPlayer( NULL );
   }
   m_mainEditor = NULL;
   m_animationToggled = false;

   m_view->setSlider( NULL );
   disconnect( m_slider, SIGNAL( valueChanged( int ) ), this, SLOT( timeValueChanged( int ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::onObservedPropertyChanged( ReflectionProperty& property )
{
   if ( property.getName() == "m_animation" )
   {
      SnapshotAnimation* animation = m_animationNode.getAnimation();
      if ( animation )
      {
         // convert to milliseconds
         uint durationInMillisecs = animation->m_duration * 100.0f;
         m_slider->setMaximum( durationInMillisecs );
      }
      else
      {
         m_slider->setMaximum( 0 );
      }

      // reset slider's position
      m_slider->setSliderPosition( 0 );

      // update the source on our custom player
      m_customAnimationPlayer->setAnimationSource( animation );
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::onEventAdded( BlendTreeAnimationEvent* btaEvent )
{
   BTAEAnimationEventItem* eventItem = new BTAEAnimationEventItem( btaEvent, m_view );
   m_eventsScene->addItem( eventItem );

   m_view->adjustRect();
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::onEventRemoved( BlendTreeAnimationEvent* btaEvent )
{
   QList< QGraphicsItem* > items = m_eventsScene->items();
   foreach( QGraphicsItem* item, items )
   {
      BTAEventItem* eventItem = static_cast< BTAEventItem* >( item );
      if ( eventItem->getEventPtr() == btaEvent )
      {
         m_eventsScene->removeItem( item );
         m_view->adjustRect();
         delete item;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::onSyncPointAdded( BlendTreeSyncPoint* syncPoint )
{
   BTASyncPointItem* syncPointItem = new BTASyncPointItem( syncPoint, m_view );
   m_eventsScene->addItem( syncPointItem );

   m_view->adjustRect();
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::onSyncPointRemoved( BlendTreeSyncPoint* syncPoint )
{
   QList< QGraphicsItem* > items = m_eventsScene->items();
   foreach( QGraphicsItem* item, items )
   {
      BTAEventItem* eventItem = static_cast< BTAEventItem* >( item );
      if ( eventItem->getEventPtr() == syncPoint )
      {
         m_eventsScene->removeItem( item );
         m_view->adjustRect();
         delete item;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::onShowContextMenu( const QPoint& pos )
{
   QMenu* popupMenu = new QMenu( this );
   QPointF scenePos = m_view->mapToScene( pos );

   // list all events currently defined in the blend tree, so that the user can immediately add them
   QMenu* addAnimEventMenu = popupMenu->addMenu( QIcon( ":/TamyEditor/Resources/Icons/Editor/plus.png" ), "Add anim event" );
   QMenu* addSyncPointMenu = popupMenu->addMenu( QIcon( ":/TamyEditor/Resources/Icons/Editor/plus.png" ), "Add sync point" );
   {
      // calculate the time of the event
      float eventTime = (float)pos.x() / (float)m_view->rect().width();
      ASSERT( eventTime >= 0.0f && eventTime <= 1.0f );

      // first - add an option that will add an empty animation event
      addAnimEventMenu->addAction( new AddAnimationEventAction( m_animationNode, NULL, eventTime ) );
      addSyncPointMenu->addAction( new AddAnimationSyncPointAction( m_animationNode, NULL, eventTime ) );

      addAnimEventMenu->addSeparator();
      addSyncPointMenu->addSeparator();

      // next add all events currently defined in the blend tree
      BlendTree* blendTree = m_animationNode.getBlendTree();
      uint eventsCount = blendTree->m_events.size();
      for ( uint i = 0; i < eventsCount; ++i )
      {
         BlendTreeEvent* btEvent = blendTree->m_events[i];
         addAnimEventMenu->addAction( new AddAnimationEventAction( m_animationNode, btEvent, eventTime ) );
         addSyncPointMenu->addAction( new AddAnimationSyncPointAction( m_animationNode, btEvent, eventTime ) );
      }
   }

   // if we clicked on another event item, allow to remove it
   {   
      QTransform identityTrans;
      QGraphicsItem* item = m_eventsScene->itemAt( scenePos, identityTrans );
      if ( item )
      {
         BTAEventItem* eventItem = static_cast< BTAEventItem* >( item );
         QAction* action = eventItem->createRemovalAction( m_animationNode );
         popupMenu->addAction( action );
      }
   }

   popupMenu->popup( mapToGlobal( pos ) );
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::onItemSelected()
{
   QList< QGraphicsItem* > items = m_eventsScene->selectedItems();
   if ( items.empty() )
   {
      m_eventProperties->reset();
   }
   else
   {
      BTAEventItem* eventItem = static_cast< BTAEventItem* >( items.front() );
      eventItem->viewProperties( m_eventProperties );
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::update( float timeElapsed )
{
   if ( m_customAnimationPlayer->isPlaying() )
   {
      float timeInSecs = m_customAnimationPlayer->getTrackTime();
      uint timeInMillisecs = (uint)( timeInSecs * 100.0f );
      m_slider->setSliderPosition( timeInMillisecs );
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::timeValueChanged( int timeInMillisecs )
{
   if ( m_animationToggled && !m_customAnimationPlayer->isPlaying() )
   {
      m_customAnimationPlayer->setTrackTime( (float)timeInMillisecs * 0.01f );

      // we'll do the manual animation stepping here, to get the poses through
      m_customAnimationPlayer->samplePoses( 0.0f );
   }
}

///////////////////////////////////////////////////////////////////////////////

void AnimationEventsEditor::onAnimationToggled( bool )
{
   m_animationToggled = !m_animationToggled;

   if ( m_animationToggled )
   {
      m_customAnimationPlayer->addReference();
      m_mainEditor->setAnimationPlayer( m_customAnimationPlayer );

      m_animationToggleButton->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/linkActive.png" ) );
   }
   else
   {
      m_mainEditor->setAnimationPlayer( NULL );

      m_animationToggleButton->setIcon( QIcon( ":/TamyEditor/Resources/Icons/Editor/linkInactive.png" ) );
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddAnimationEventAction::AddAnimationEventAction( BlendTreeAnimation& animationNode, BlendTreeEvent* btEvent, float eventTime )
   : QAction( QIcon(), btEvent ? btEvent->getName().c_str() : "Empty event", NULL )
   , m_animationNode( animationNode )
   , m_btEvent( btEvent )
   , m_eventTime( eventTime )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddAnimationEventAction::onTriggered()
{
   BlendTreeAnimationEvent* btaEvent = new BlendTreeAnimationEvent( m_btEvent, m_eventTime );
   m_animationNode.addEvent( btaEvent );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

AddAnimationSyncPointAction::AddAnimationSyncPointAction( BlendTreeAnimation& animationNode, BlendTreeEvent* btEvent, float eventTime )
   : QAction( QIcon(), btEvent ? btEvent->getName().c_str() : "Empty sync point", NULL )
   , m_animationNode( animationNode )
   , m_btEvent( btEvent )
   , m_eventTime( eventTime )
{
   connect( this, SIGNAL( triggered() ), this, SLOT( onTriggered() ) );
}

///////////////////////////////////////////////////////////////////////////////

void AddAnimationSyncPointAction::onTriggered()
{
   BlendTreeSyncPoint* btaSyncPoint = new BlendTreeSyncPoint( m_btEvent, m_eventTime );
   m_animationNode.addSyncPoint( btaSyncPoint );
}

///////////////////////////////////////////////////////////////////////////////

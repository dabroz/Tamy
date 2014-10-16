#include "ProfilerChart.h"
#include <QtWidgets\QGraphicsScene>
#include <QtOpenGL\QGLWidget>
#include <QtWidgets\QGraphicsLineItem>
#include <QtWidgets\QGraphicsSimpleTextItem>
#include "core\Algorithms.h"


///////////////////////////////////////////////////////////////////////////////

ProfilerChart::ProfilerChart( QWidget* parentWidget, uint profilerId )
   : QGraphicsView( parentWidget )
   , m_profilerId( profilerId )
   , m_chartPen( QColor( 255, 132, 17 ) )
   , DISPLAYED_DURATION( 10.0f )
   , SAMPLING_INTERVAL( 0.5f )
   , m_samplingTime( 0.0f )
   , m_sampleValue( 0.0f )
   , m_topValue( 0.0f )
{
   if ( QGLFormat::hasOpenGL() )
   {
      // use an openGL renderer if possible
      QGLWidget* renderer = new QGLWidget( QGLFormat( QGL::DoubleBuffer | QGL::NoDepthBuffer | QGL::NoAlphaChannel ), this );
      setViewport( renderer );
   }

   QColor bgColor( 17, 81, 255 );
   setBackgroundBrush( QBrush( bgColor ) );
   setViewportUpdateMode( QGraphicsView::BoundingRectViewportUpdate );
   setRenderHints( 0 );
   scale( 1.0f, -1.0f );

   m_scene = new QGraphicsScene( this );
   setScene( m_scene );

   // add a number of chart lines that will fill the plot
   {
      uint linesCount = (uint)( DISPLAYED_DURATION / SAMPLING_INTERVAL );
      m_samplesCount = linesCount + 1;

      // create the points list head
      m_chartPointsHead = new PointsListNode();
      PointsListNode* currSampleNode = m_chartPointsHead;

      for ( int i = 0; i < linesCount; ++i )
      {
         QGraphicsLineItem* line =  m_scene->addLine( (float)i * SAMPLING_INTERVAL, 0, (float)( i + 1 ) * SAMPLING_INTERVAL, 0, m_chartPen );
         m_chartLines.push_back( line );

         PointsListNode* newSampleNode = new PointsListNode();
         currSampleNode->m_nextNode = newSampleNode;
         newSampleNode->m_prevNode = currSampleNode;
         currSampleNode = newSampleNode;
      }

      // loop the samples list
      currSampleNode->m_nextNode = m_chartPointsHead;
      m_chartPointsHead->m_prevNode = currSampleNode;

   }
}

///////////////////////////////////////////////////////////////////////////////

ProfilerChart::~ProfilerChart()
{
   // delete the looped points list by unrolling it and deleting its members individually
   {
      std::vector< PointsListNode* > nodes( m_samplesCount );
      PointsListNode* currSampleNode = m_chartPointsHead;
      for ( uint i = 0; i < m_samplesCount; ++i, currSampleNode = currSampleNode->m_nextNode )
      {
         nodes[i] = currSampleNode;
      }

      for ( uint i = 0; i < m_samplesCount; ++i )
      {
         delete nodes[i];
      }
      m_chartPointsHead = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

float ProfilerChart::addTimeSample( float elapsedTime, float sampleValue )
{
   // add the sample to the chart
   m_sampleValue += sampleValue;
   commitSample( elapsedTime );

   return m_topValue;
}

///////////////////////////////////////////////////////////////////////////////

float ProfilerChart::addValueSample( float elapsedTime, float sampleValue )
{
   // add the sample to the chart
   m_sampleValue = sampleValue;
   commitSample( elapsedTime );

   return m_topValue;
}

///////////////////////////////////////////////////////////////////////////////

void ProfilerChart::commitSample( float elapsedTime )
{
   m_samplingTime += elapsedTime;
   bool refreshLines = false;

   while( m_samplingTime > SAMPLING_INTERVAL )
   {
      refreshLines = true;

      // go to the next point
      m_chartPointsHead = m_chartPointsHead->m_nextNode;

      // calculate the elapsed time and sample value ( mind that they may be a part of a larger display sample
      // due to our internal sampling resolution )
      float timePercentage = ( SAMPLING_INTERVAL / m_samplingTime );
      m_samplingTime -= SAMPLING_INTERVAL;

      float partialSampleValue = timePercentage * m_sampleValue;
      m_sampleValue -= partialSampleValue;


      // update the total time and the corresponding sample point
      m_chartPointsHead->m_value = partialSampleValue;

   }

   // refresh the lines ( since head always points to the very last point on the list, refresh the lines backwards )
   if ( refreshLines )
   {
      m_topValue = 0.0f;

      PointsListNode* currNode = m_chartPointsHead;
      int lastLineIdx = m_samplesCount - 2;

      // set the last point of the last line
      QLineF line = m_chartLines[lastLineIdx]->line();
      line.setP2( QPointF( line.x2(), currNode->m_value ) );
      m_topValue = currNode->m_value;

      // iterate over the lines
      for ( int i = lastLineIdx; i >= 0; --i )
      {
         // go to the next node
         currNode = currNode->m_prevNode;

         // and update the line points
         line.setP1( QPointF( line.x1(), currNode->m_value ) );
         m_chartLines[i]->setLine( line );

         // record the top value
         m_topValue = max2( m_topValue, currNode->m_value );

         // and grab the next line for update  
         int prevLineIdx = i - 1;
         if ( prevLineIdx >= 0 )
         {
            line = m_chartLines[prevLineIdx]->line();
            line.setP2( QPointF( line.x2(), currNode->m_value ) );
         }
      }

      // update the scene rectangle
      QRectF sceneRect( 0, 0, m_samplesCount * SAMPLING_INTERVAL, m_topValue * 1.3f );
      setSceneRect( sceneRect );
      fitInView( sceneRect );
      m_scene->update();
   }
}

///////////////////////////////////////////////////////////////////////////////

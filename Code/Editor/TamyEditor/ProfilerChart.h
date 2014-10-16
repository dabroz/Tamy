/// @file   TamyEditor/ProfilerChart.h
/// @brief  a chart showing the runtime traces of a profiled timer
#pragma once

#include <QtWidgets\QGraphicsView>
#include <QtGui\QPen>
#include <QPointF>
#include "core/types.h"
#include <vector>


///////////////////////////////////////////////////////////////////////////////

class QGraphicsScene;
class QGraphicsSimpleTextItem;

///////////////////////////////////////////////////////////////////////////////

class ProfilerChart : public QGraphicsView
{
   Q_OBJECT

private:
   struct PointsListNode
   {
      float                            m_value;
      PointsListNode*                  m_nextNode;
      PointsListNode*                  m_prevNode;

      PointsListNode() : m_value( 0.0f ), m_nextNode( NULL ), m_prevNode( NULL ) {}
   };

public:
   uint                                m_profilerId;

   QGraphicsScene*                     m_scene;
   QPen                                m_chartPen;

   const float                         DISPLAYED_DURATION;
   const float                         SAMPLING_INTERVAL;

   float                               m_topValue;

   std::vector< QGraphicsLineItem* >   m_chartLines;
   PointsListNode*                     m_chartPointsHead;
   uint                                m_samplesCount;

   float                               m_sampleValue;
   float                               m_samplingTime;

public:
   /**
    * Constructor.
    *
    * @param parentWidget
    * @param profilerId          id of the charted profiler
    */
   ProfilerChart( QWidget* parentWidget, uint profilerId );
   ~ProfilerChart();

   /**
    * Adds a new time sample to the chart.
    *
    * @param elapsedTime      time elapsed since the last sample was added
    * @param sampleValue
    *
    * @return                 top displayed value
    */
   float addTimeSample( float elapsedTime, float sampleValue );

   /**
    * Adds a new value sample to the chart.
    *
    * @param elapsedTime      time elapsed since the last sample was added
    * @param sampleValue
    *
    * @return                 top displayed value
    */
   float addValueSample( float elapsedTime, float sampleValue );

private:
   void commitSample( float elapsedTime );
};

///////////////////////////////////////////////////////////////////////////////

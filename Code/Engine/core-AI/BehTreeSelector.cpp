#include "core-AI\BehTreeSelector.h"
#include "core-AI\BehTreeCondition.h"
#include "core-AI\BehaviorTreeRunner.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BehTreeSelector );
   PARENT( BehTreeComposite );
   PROPERTY_EDIT( "Condition", BehTreeCondition*, m_condition );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BehTreeSelector::BehTreeSelector()
   : m_condition( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////

BehTreeSelector::~BehTreeSelector()
{
   if ( m_condition )
   {
      m_condition->removeReference();
      m_condition = NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeSelector::setCondition( BehTreeCondition* condition )
{
   if ( m_condition )
   {
      m_condition->removeReference();
      m_condition = NULL;
   }

   m_condition = condition;
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeSelector::createLayout( BehaviorTreeRunner& runner ) const
{
   BehTreeComposite::createLayout( runner );

   RuntimeDataBuffer& data = runner.data();
   data.registerVar( m_activeIdx );
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeSelector::initialize( BehaviorTreeRunner& runner ) const
{
   // override the default functionality and don't initialize the child up front
   RuntimeDataBuffer& data = runner.data();
   data[m_activeIdx] = -1;
}

///////////////////////////////////////////////////////////////////////////////

void BehTreeSelector::deinitialize( BehaviorTreeRunner& runner ) const
{
   // override the default functionality and deinitialize the child only
   // if it was running 
   RuntimeDataBuffer& data = runner.data();
   const int activeIdx = data[m_activeIdx];
   if ( activeIdx >= 0 )
   {
      m_nodes[activeIdx]->deinitialize( runner );
      data[m_activeIdx] = -1;
   }
}

///////////////////////////////////////////////////////////////////////////////

BehTreeNode::Result BehTreeSelector::execute( BehaviorTreeRunner& runner ) const
{
   if ( !m_condition )
   {
      return BehTreeNode::FINISHED;
   }

   RuntimeDataBuffer& data = runner.data();
   const int activeIdx = data[m_activeIdx];

   int selectedIdx = m_condition->evaluate( runner );
   if ( selectedIdx >= m_nodes.size() )
   {
      // the user didn't connect a node to that index, so don't activate anything
      selectedIdx = -1;
   }

   if ( selectedIdx != activeIdx )
   {
      // deactivate the old node and activate the new one
      if ( activeIdx >= 0 )
      {
         m_nodes[activeIdx]->deinitialize( runner );
      }

      data[m_activeIdx] = selectedIdx;

      if ( selectedIdx >= 0 )
      {
         m_nodes[selectedIdx]->initialize( runner );
      }
   }

   if ( selectedIdx >= 0 )
   {
      BehTreeNode::Result result = m_nodes[selectedIdx]->execute( runner );
      return result;
   }
   else
   {
      return BehTreeNode::FINISHED;
   }
}

///////////////////////////////////////////////////////////////////////////////
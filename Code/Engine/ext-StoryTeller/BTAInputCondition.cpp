#include "ext-StoryTeller\BTAInputCondition.h"
#include "ext-StoryTeller\StoryBehTreeContext.h"
#include "ext-StoryTeller\StoryPlayer.h"
#include "core-AppFlow\UserInputController.h"
#include "core-AI\BehaviorTreeRunner.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTAInputCondition );
   PARENT( BehTreeCondition );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTAInputCondition::BTAInputCondition()
{
}

///////////////////////////////////////////////////////////////////////////////

BTAInputCondition::~BTAInputCondition()
{
}

///////////////////////////////////////////////////////////////////////////////

int BTAInputCondition::evaluate( BehaviorTreeRunner& runner, const BehTreeSelector& hostSelector ) const
{
   StoryBehTreeContext* context = ( StoryBehTreeContext* ) runner.getContext();
   UserInputController* inputCtrl = context->m_player.userInputController();

   return inputCtrl->isKeyPressed( 'Q' ) ? 0 : 1;
}

///////////////////////////////////////////////////////////////////////////////
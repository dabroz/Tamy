#include "core-AI\BTTTVariable.h"
#include "core-AI\BlendTreeVariable.h"
#include "core\ReflectionEnum.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_ENUM( ComparisonOp );
   ENUM_VAL( CO_Equal );
   ENUM_VAL( CO_NotEqual );
   ENUM_VAL( CO_Less );
   ENUM_VAL( CO_LessEqual );
   ENUM_VAL( CO_Greater );
   ENUM_VAL( CO_GreaterEqual );
END_ENUM();

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( BTTTVariable );
   PARENT( BlendTreeTransitionTrigger );
   PROPERTY_EDIT_REFERENCE( "Tested value", BlendTreeVariable*, m_testedValue );
   PROPERTY_EDIT_REFERENCE( "Reference value", BlendTreeVariable*, m_referenceValue );
   PROPERTY_EDIT( "Comparison op", ComparisonOp,  m_comparisonOp );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BTTTVariable::BTTTVariable( BlendTreeVariable* testedValue, BlendTreeVariable* desiredValue, ComparisonOp comparisonOp )
   : m_testedValue( testedValue )
   , m_referenceValue( desiredValue )
   , m_comparisonOp( comparisonOp )
{
   registerReferences();
}

///////////////////////////////////////////////////////////////////////////////

BTTTVariable::~BTTTVariable()
{
   // don't erase the pointers to m_testedValue & m_referenceValue, or they won't be unregistered properly by the reference tracking mechanism
}

///////////////////////////////////////////////////////////////////////////////

bool BTTTVariable::testActivation( BlendTreePlayer* player ) const
{
   if ( !m_referenceValue || !m_testedValue )
   {
      // the test is not configured
      return false;
   }

   bool result = false;

   switch( m_comparisonOp )
   {
   case CO_Equal:
      {
         result = m_testedValue->isEqual( player, *m_referenceValue );
         break;
      }

   case CO_NotEqual:
      {
         result = !m_testedValue->isEqual( player, *m_referenceValue );
         break;
      }

   case CO_Less:
      {
         result = m_testedValue->isLess( player, *m_referenceValue );
         break;
      }

   case CO_LessEqual:
      {
         result = m_testedValue->isLessEqual( player, *m_referenceValue );
         break;
      }

   case CO_Greater:
      {
         result = m_testedValue->isGreater( player, *m_referenceValue );
         break;
      }

   case CO_GreaterEqual:
      {
         result = m_testedValue->isGreaterEqual( player, *m_referenceValue );
         break;
      }
   }

   return result;
}

///////////////////////////////////////////////////////////////////////////////

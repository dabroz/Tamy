#ifndef _BLEND_TREE_H
#error "This file can only be included in BlendTree.h"
#else


///////////////////////////////////////////////////////////////////////////////

template< typename VarType >
const VarType* BlendTree::getVariable( const std::string& name ) const
{
   const uint count = m_variables.size();
   for ( uint i = 0; i < count; ++i )
   {
      const BlendTreeVariable* var = m_variables[i];
      if ( var->getName() != name )
      {
         continue;
      }

      if ( var->getDataTypeID() == VarType::s_dataType.m_id )
      {
         // found it
         return static_cast< const VarType* >( var );
      }
   }

   // such a variable doesn't exist
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

#endif // _BLEND_TREE_H

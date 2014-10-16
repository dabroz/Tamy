#ifndef _GRAPH_BUILDER_NODE_UTILS_H
#error "This file can only be included from GraphBuilderNodeUtils.h"
#else


///////////////////////////////////////////////////////////////////////////////

template< typename SocketType >
void GraphBuilderNodeUtils::mergeSocketsLists( const std::vector< SocketType* >& newSockets, const std::vector< SocketType* >& existingSockets, List< uint >& outSocketsToRemove, List< uint >& outSocketsToAdd )
{
   std::vector< bool > socketsToAdd( newSockets.size(), false );
   std::vector< bool > socketsToRemove( existingSockets.size(), true );

   int newSocketsCount = newSockets.size();
   for ( int newSocketIdx = newSocketsCount - 1; newSocketIdx >= 0; --newSocketIdx )
   {
      const std::string& newSocketName = newSockets[newSocketIdx]->getName();

      bool socketExists = false;
      int oldSocketsCount = existingSockets.size();
      for ( int oldSocketIdx = 0; oldSocketIdx < oldSocketsCount; ++oldSocketIdx )
      {
         if ( newSocketName == existingSockets[oldSocketIdx]->getName() )
         {
            // found it
            socketsToRemove[oldSocketIdx] = false;
            socketExists = true;
            break;
         }
      }

      if ( !socketExists )
      {
         socketsToAdd[newSocketIdx] = true;
      }
      else
      {
         // delete the socket that's not going to be used
         delete newSockets[newSocketIdx];
      }
   }

   // remove the outputs that remain on the list
   uint count = socketsToRemove.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( socketsToRemove[i] )
      {
         outSocketsToRemove.pushBack( i );
      }
   }

   // now add new outputs
   count = socketsToAdd.size();
   for ( uint i = 0; i < count; ++i )
   {
      if ( socketsToAdd[i] )
      {
         outSocketsToAdd.pushBack( i );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

#endif // _GRAPH_BUILDER_NODE_UTILS_H

#include "core\FilesystemSnapshot.h"
#include "core\Filesystem.h"
#include "core\List.h"
#include "core\ListUtils.h"
#include "core\Log.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

FilesystemSnapshot::FilesystemSnapshot( Filesystem& fs )
   : m_root( new FilesystemEntry( FilePath( "/" ), true ) )
   , m_fs( fs )
{
}

///////////////////////////////////////////////////////////////////////////////

FilesystemSnapshot::~FilesystemSnapshot()
{
   deleteEntry( NULL, m_root );
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSnapshot::add( const FilePath& path, ulong timestampLo, ulong timestampHi )
{
   // find a correct Entry to insert the new child into
   List< std::string > pathElements;
   path.getElements( pathElements );

   List< FilesystemEntry* >* entries = &m_root->m_children;
   FilePath aseembledPath;
   for ( List< std::string >::iterator it = pathElements.begin(); !it.isEnd(); ++it )
   {
      const std::string& pathElement = *it;
      aseembledPath += pathElement;

      List< FilesystemEntry* >::iterator matchingElementIt = ListUtils::find< FilesystemEntry*, std::string, FilesystemEntry >( *entries, pathElement );
      if ( matchingElementIt.isEnd() )
      {
         // this element doesn't exist in our snapshot - we need to add it
         std::string absPathStr = FilePath( aseembledPath ).toAbsolutePath( m_fs );
         DWORD attribs = GetFileAttributesA( absPathStr.c_str() );
         bool isDir = false;
         if ( attribs != INVALID_FILE_ATTRIBUTES )
         {
            isDir = ( attribs & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY;
         }
         else
         {
            LOG( "FilesystemSnapshot: can't open the file/dir '%s' - assuming it's a file", absPathStr.c_str() );
         }
            
         FilesystemEntry* newEntry = new FilesystemEntry( aseembledPath, isDir, timestampLo, timestampHi );
         ListUtils::insertSorted< FilesystemEntry*, FilesystemEntry >( *entries, newEntry );
         entries = &newEntry->m_children;
      }
      else 
      {
         // we already have the next segment
         FilesystemEntry* matchingEntry = *matchingElementIt;
         entries = &matchingEntry->m_children;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSnapshot::remove( const FilePath& path )
{
   // find a correct Entry to remove
   List< std::string > pathElements;
   path.getElements( pathElements );

   FilesystemEntry* toRemove = m_root;
   FilesystemEntry* parentEntry = NULL;

   List< FilesystemEntry* >* entries = &m_root->m_children;
   for ( List< std::string >::iterator it = pathElements.begin(); !it.isEnd(); ++it )
   {
      const std::string& pathElement = *it;

      List< FilesystemEntry* >::iterator matchingElementIt = ListUtils::find< FilesystemEntry*, std::string, FilesystemEntry >( *entries, pathElement );
      if ( matchingElementIt.isEnd() )
      {
         // We ran out of entries along the required path - means that the tree doesn't contain this entry,
         // so there's nothing to remove.
         // However - log this just in case.,
         LOG( "FilesystemSnapshot: Tried to remove an unmapped file '%s'", path.c_str() );
         toRemove = NULL;
      }
      else 
      {
         parentEntry = toRemove;
         toRemove = *matchingElementIt;
         entries = &toRemove->m_children;
      }
   }

   if ( toRemove )
   {
      deleteEntry( parentEntry, toRemove );
   }
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSnapshot::deleteEntry( FilesystemEntry* parentEntry, FilesystemEntry* toRemove )
{
   List< FilesystemEntry* > toDelete;
   toDelete.pushBack( toRemove );

   while ( !toDelete.empty() )
   {
      FilesystemEntry* nextEntry = toDelete.front();
      toDelete.popFront();

      for ( List< FilesystemEntry* >::iterator it = nextEntry->m_children.begin(); !it.isEnd(); ++it )
      {
         toDelete.pushBack( *it );
      }

      delete nextEntry;
   }

   if ( !parentEntry )
   {
      // we've just deleted the root
      m_root = NULL;
   }
   else
   {
      // erase the entry from the children's list
      List< FilesystemEntry* >::iterator it = ListUtils::find( parentEntry->m_children, toRemove );
      ASSERT( !it.isEnd() );
      it.markForRemoval();
   }
}

///////////////////////////////////////////////////////////////////////////////

const FilesystemEntry* FilesystemSnapshot::getEntry( const FilePath& path ) const
{
   // find a correct Entry to remove
   List< std::string > pathElements;
   path.getElements( pathElements );

   const List< FilesystemEntry* >* entries = &m_root->m_children;
   const FilesystemEntry* toReturn = m_root;
   for ( List< std::string >::iterator it = pathElements.begin(); !it.isEnd(); ++it )
   {
      const std::string& pathElement = *it;

      List< FilesystemEntry* >::const_iterator matchingElementIt = ListUtils::find< FilesystemEntry*, std::string, FilesystemEntry >( *entries, pathElement );
      if ( !matchingElementIt.isEnd() )
      {
         toReturn = *matchingElementIt;
         entries = &matchingElementIt->m_children;
      }
      else
      {
         // an entry this path points to doesn't exist
         return NULL;
      }
   }

   return toReturn;
}

///////////////////////////////////////////////////////////////////////////////

FilesystemEntry* FilesystemSnapshot::accessEntry( const FilePath& path )
{
   // find a correct Entry to remove
   List< std::string > pathElements;
   path.getElements( pathElements );

   List< FilesystemEntry* >* entries = &m_root->m_children;
   FilesystemEntry* toReturn = m_root;
   for ( List< std::string >::iterator it = pathElements.begin(); !it.isEnd(); ++it )
   {
      const std::string& pathElement = *it;

      List< FilesystemEntry* >::iterator matchingElementIt = ListUtils::find< FilesystemEntry*, std::string, FilesystemEntry >( *entries, pathElement );
      if ( !matchingElementIt.isEnd() )
      {
         toReturn = *matchingElementIt;
         entries = &matchingElementIt->m_children;
      }
      else
      {
         // an entry this path points to doesn't exist
         return NULL;
      }
   }

   return toReturn;
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSnapshot::compare( const FilesystemSnapshot& lhs, FilesystemChanges& outChanges ) const
{
   struct Tuple
   {
      const FilesystemEntry*      m_rhsEntry;
      const FilesystemEntry*      m_lhsEntry;

      Tuple( const FilesystemEntry* rhsEntry, const FilesystemEntry* lhsEntry )
         : m_rhsEntry( rhsEntry )
         , m_lhsEntry( lhsEntry )
      {
      }
   };

   List< Tuple > entries;
   entries.pushBack( Tuple( m_root, lhs.m_root ) );

   while ( !entries.empty() )
   {
      Tuple tuple = entries.back();
      entries.popBack();

      ASSERT( tuple.m_rhsEntry->m_path == tuple.m_lhsEntry->m_path );
      if ( tuple.m_rhsEntry->m_timestampLo != tuple.m_lhsEntry->m_timestampLo || tuple.m_rhsEntry->m_timestampHi != tuple.m_lhsEntry->m_timestampHi )
      {
         // the entry was modified
         outChanges.m_modifiedFiles.pushBack( tuple.m_lhsEntry );
      }

      // collect the children and find their matches
      const List< FilesystemEntry* >& rhsChildren = tuple.m_rhsEntry->m_children;
      const List< FilesystemEntry* >& lhsChildren = tuple.m_lhsEntry->m_children;

      List< FilesystemEntry* >::const_iterator rhsIt = rhsChildren.begin();
      List< FilesystemEntry* >::const_iterator lhsIt = lhsChildren.begin();
      for ( ; !rhsIt.isEnd() && !lhsIt.isEnd(); )
      {
         const FilesystemEntry* rhsEntry = *rhsIt;
         const FilesystemEntry* lhsEntry = *lhsIt;

         if ( rhsEntry->m_name == lhsEntry->m_name )
         {
            // analyze the children
            entries.pushBack( Tuple( rhsEntry, lhsEntry ) );
            ++rhsIt;
            ++lhsIt;
         }
         else
         {
            // the entires don't match. Because the entires in both lists were sorted according
            // to their names, this means that:
            // - a new entry has been added - in which case the entry appears in 'lhsChildren' list, but it's not on the 'rhsChildren' list
            // - an entry has been removed - in which case the entry appears in 'rhsChildren' list, but it's not on the 'lhsChildren' list
            List< FilesystemEntry* >::const_iterator removedEntryIt = ListUtils::find< FilesystemEntry*, std::string, FilesystemEntry >( rhsChildren, lhsEntry->m_name );
            if ( removedEntryIt.isEnd() )
            {
               // the entry was removed
               addSubtree( lhsEntry, outChanges.m_addedFiles );

               ++lhsIt; // we didn't find a match in the other list, which means that the entry from that list is still
                        // left to be checked, but we can safely proceed to the next entry on the 'rhsChildren' list
            }

            List< FilesystemEntry* >::const_iterator addedEntryIt = ListUtils::find< FilesystemEntry*, std::string, FilesystemEntry >( lhsChildren, rhsEntry->m_name );
            if ( addedEntryIt.isEnd() )
            {
               // the entry was added
               addSubtree( rhsEntry, outChanges.m_removedFiles );

               ++rhsIt; // we didn't find a match in the 'rhsEntries' list, so we're not in the position to change
                           // its iterator - that entry still needs to be assessed
            }
         }
      }

      // if any of the lists has remaining elements, add them to the respective change tracking list
      for ( ; !rhsIt.isEnd(); ++rhsIt )
      {
         const FilesystemEntry* rhsEntry = *rhsIt;
         addSubtree( rhsEntry, outChanges.m_removedFiles );
      }
      for ( ; !lhsIt.isEnd(); ++lhsIt )
      {
         const FilesystemEntry* lhsEntry = *lhsIt;
         addSubtree( lhsEntry, outChanges.m_addedFiles );
      }
   }

}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSnapshot::addSubtree( const FilesystemEntry* entry, List< const FilesystemEntry* >& outEntries ) const
{
   List< const FilesystemEntry* > entriesQueue;
   entriesQueue.pushBack( entry );

   while ( !entriesQueue.empty() )
   {
      const FilesystemEntry* entry = entriesQueue.front();
      outEntries.pushBack( entry );
      entriesQueue.popFront();

      for ( List< FilesystemEntry* >::const_iterator it = entry->m_children.begin(); !it.isEnd(); ++it )
      {
         entriesQueue.pushBack( *it );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

std::string FilesystemSnapshot::toString() const
{
   std::string str;

   struct Tuple
   {
      FilesystemEntry*      m_entry;
      uint                 m_depth;

      Tuple( FilesystemEntry* entry, uint depth )
         : m_entry( entry )
         , m_depth( depth )
      {
      }
   };

   List< Tuple > entries;
   for ( List< FilesystemEntry* >::iterator it = m_root->m_children.begin(); !it.isEnd(); ++it )
   {
      entries.pushBack( Tuple( *it, 0 ) );
   }

   char tmpStr[512];
   while ( !entries.empty() )
   {
      Tuple tuple = entries.back();
      entries.popBack();

      for ( List< FilesystemEntry* >::iterator it = tuple.m_entry->m_children.begin(); !it.isEnd(); ++it )
      {
         entries.pushBack( Tuple( *it, tuple.m_depth + 1 ) );
      }

      sprintf_s( tmpStr, "(%d)%s;", tuple.m_depth, tuple.m_entry->m_name.c_str() );
      str += tmpStr;
   }

   return str;
}

///////////////////////////////////////////////////////////////////////////////

void FilesystemSnapshot::swapContents( FilesystemSnapshot& rhs )
{
   FilesystemEntry* rhsRoot = rhs.m_root;
   rhs.m_root = m_root;
   m_root = rhsRoot;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FilesystemEntry::FilesystemEntry( const FilePath& path, bool isDir, ulong timestampLo, ulong timestampHi )
   : m_name( path.extractNodeName() )
   , m_path( path )
   , m_isDir( isDir )
   , m_timestampLo( timestampLo )
   , m_timestampHi( timestampHi )
{
}

///////////////////////////////////////////////////////////////////////////////

FilesystemEntry::~FilesystemEntry()
{
}

///////////////////////////////////////////////////////////////////////////////

bool FilesystemEntry::isEqual( FilesystemEntry* entry, const std::string& name )
{
   return entry->m_name == name;
}

///////////////////////////////////////////////////////////////////////////////

bool FilesystemEntry::isEqual( const FilesystemEntry* entry, const FilePath& path )
{
   return entry->m_path == path;
}

///////////////////////////////////////////////////////////////////////////////

bool FilesystemEntry::isLesser( FilesystemEntry* rhs, FilesystemEntry* lhs )
{
   return rhs->m_name < lhs->m_name;
}

///////////////////////////////////////////////////////////////////////////////

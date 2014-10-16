#include "ext-StoryTeller\StoryActionSockets.h"
#include "ext-StoryTeller\StoryAction.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SAVoidInput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( SAOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( SAVoidOutput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< StoryAction* >, m_connectedNodes );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT( SAAnnotationInput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( SAOutput*, m_connectedOutput );
END_OBJECT();

BEGIN_OBJECT( SAAnnotationOutput );
   PARENT( ReflectionObject );
   PROPERTY( std::string, m_name );
   PROPERTY( std::vector< StoryAction* >, m_connectedNodes );
END_OBJECT();

///////////////////////////////////////////////////////////////////////////////

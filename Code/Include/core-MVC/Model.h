/// @file   core-MVC\Model.h
/// @brief  MVC model
#ifndef _MODEL_H
#define _MODEL_H

#include "core\Delegate.h"
#include "core\Resource.h"
#include "core\Array.h"
#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class SceneNode;
class Component;
class Entity;
class ModelView;

///////////////////////////////////////////////////////////////////////////////

/**
 * Model stores entities and allows to serialize them. Model
 * can be observed and 'illustrated' using views.
 */
class Model : public Resource
{
   DECLARE_ALLOCATOR( Model, AM_ALIGNED_16 );
   DECLARE_RESOURCE();

private:
   typedef Delegate< void ( Entity& ) >            Functor;
   typedef Array< Entity* >                        Entities;
   typedef List< ModelView* >                      Views;

   // static data
   Entity*                                m_root;

   // runtime data
   Views                                  m_views;

public:
   /**
    * Constructor.
    *
    * @param resourceName    scene name
    */
   Model( const FilePath& resourceName = FilePath() );
   ~Model();

   /**
    * Returns the root entity.
    */
   inline Entity* getRoot() const { return m_root; }

   /**
    * Adds a new child to the scene's root.
    *
    * @param child     new child
    */
   void addChild( SceneNode* child );

   /**
    * Clears the scene's contents.
    */
   void clear();

   // -------------------------------------------------------------------------
   // Views management
   // -------------------------------------------------------------------------
   /**
    * Attaches a new view to the model.
    *
    * @param view                new model observer
    */
   void attachListener( ModelView* view );

   /**
    * Detaches a view from the model.
    *
    * @param view    observer we want to detach
    */
   void detachListener( ModelView* view );

   /**
    * Pulls the scene structure.
    *
    * @param view
    */
   void pullStructure( ModelView* view );

   // -------------------------------------------------------------------------
   // Resource implementation
   // -------------------------------------------------------------------------
   void replaceContents( Resource& rhs );
   void onResourceLoaded( ResourcesManager& mgr );

private:
// -------------------------------------------------------------------------
   // Befriended operations
   // -------------------------------------------------------------------------
   friend class SceneNode;
   void notifyNodeAdded( SceneNode* node );
   void notifyNodeRemoved( SceneNode* node );
   void notifyNodeChanged( SceneNode* node );
};

///////////////////////////////////////////////////////////////////////////////

#include "core-MVC\Model.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _MODEL_H

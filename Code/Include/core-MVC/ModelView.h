/// @file   core-MVC\ModelView.h
/// @brief  view that observes changes of a model
#pragma once

#include "core\List.h"


///////////////////////////////////////////////////////////////////////////////

class SceneNode;
class Model;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class is a model observer.
 */
class ModelView 
{
private:
   List< Model* >          m_models;

public:
   virtual ~ModelView();

   /**
    * This method is called when a new node is added to the observed model.
    *
    * @param node
    */
   virtual void onNodeAdded( SceneNode* node ) {}

   /**
    * This method is called just before a node is removed from the observed 
    * model.
    *
    * @param node
    */
   virtual void onNodeRemoved( SceneNode* node ) {}

   /**
    * This method is called to inform that some aspect of a node has changed.
    *
    * @param node
    */
   virtual void onNodeChanged( SceneNode* node ) {}

   /**
    * The method will should clear view contents.
    */
   virtual void resetContents( Model& model ) {}

   // -------------------------------------------------------------------------
   // Model-View link management
   // -------------------------------------------------------------------------
   /**
    * A Model instance calls this method to inform the view about the link
    * they now create.
    *
    * @param model   instance of Model the view is now observing
    */
   virtual void onAttachedToModel( Model& model );

   /**
    * A Model instance calls this method to inform the view that the link
    * they created was broken and they are no longer connected.
    *
    * @param model   instance of Model the view used to be observing.
    */
   virtual void onDetachedFromModel( Model& model );

   /**
    * The method can be used to check if the view is actively observing 
    * any model.
    *
    * @return     'true' if it's attached to a model, 'false' otherwise
    */
   bool isAttachedToModel() const;

   /**
    * Returns an array of observed models.
    */
   inline const List< Model* >& getObservedModels() const { return m_models; }

protected:
   /**
    * Constructor.
    */
   ModelView();
};

///////////////////////////////////////////////////////////////////////////////

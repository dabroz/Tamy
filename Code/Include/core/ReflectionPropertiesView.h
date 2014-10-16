/// @file   core\ReflectionPropertiesView.h
/// @brief  a generic factory of property editors

#ifndef REFLECTION_PROPERTIES_VIEW_H
#define REFLECTION_PROPERTIES_VIEW_H

#include <vector>
#include "core\MemoryRouter.h"
#include "core\GenericFactory.h"
#include "core\ReflectionPropertyEditor.h"
#include "core\Assert.h"
#include "core\ReflectionObjectChangeListener.h"


///////////////////////////////////////////////////////////////////////////////

class ReflectionProperty;

///////////////////////////////////////////////////////////////////////////////

/**
 * This view will create editors for the properties.
 * It's called a view, because it allows to 'visualize' the properties
 * using the editors, expose them to the user that way.
 *
 * It's generic - this level only allows to create editors.
 * However it's the extensions that will actually be passed to the created
 * instances of the editors, allowing those editors to interact with the system.
 * Such a derived view could be for instance a window - and editors
 * could be edit fields showing the values.
 *
 * Each editor should be derived from TPropertyEditor class and specialized
 * for a particular child view class. This way the child view class can contain
 * all the applicable implementation data (window details etc), and expose
 * them to the editors that are meant to operate with that view.
 */
class ReflectionPropertiesView : public ReflectionObjectChangeListener
{
   DECLARE_ALLOCATOR( ReflectionPropertiesView, AM_DEFAULT );

private:
   // -------------------------------------------------------------------------
   // internal types definitions
   // -------------------------------------------------------------------------

   class Creator
   {
   public:
      virtual ~Creator() {}

      virtual ReflectionPropertyEditor* operator()( ReflectionProperty* obj ) = 0;

      virtual bool doesMatch( const ReflectionType& checkedClassType ) const = 0;
   };

   // -------------------------------------------------------------------------

   template< class REPR_IMPL, class ENTITY_IMPL >
   class TStaticCreator : public Creator
   {
      DECLARE_ALLOCATOR( TStaticCreator, AM_DEFAULT );

   private:
      const ReflectionType&    m_refClass;

   public:
      TStaticCreator()
         : m_refClass( TReflectionProperty< ENTITY_IMPL >::getRTTIClass() )
      {
      }

      ReflectionPropertyEditor* operator()( ReflectionProperty* property )
      {
         TEditableReflectionProperty< ENTITY_IMPL >* editableProperty = new TEditableReflectionProperty< ENTITY_IMPL >( property );
         return new REPR_IMPL( editableProperty );
      }

      bool doesMatch( const ReflectionType& checkedClassType ) const
      {
         return m_refClass.isExactlyA( checkedClassType );
      }
   };

   // -------------------------------------------------------------------------

   template< class REPR_IMPL, class ENTITY_IMPL >
   class TPolymorphicCreator : public Creator
   {
      DECLARE_ALLOCATOR( TPolymorphicCreator, AM_DEFAULT );

   private:
      const ReflectionType&    m_refClass;

   public:
      TPolymorphicCreator()
         : m_refClass( TReflectionProperty< ENTITY_IMPL >::getRTTIClass() )
      {
      }

      ReflectionPropertyEditor* operator()( ReflectionProperty* property )
      {
         TEditableReflectionProperty< ENTITY_IMPL >* editableProperty = new TEditableReflectionProperty< ENTITY_IMPL >( property );
         return new REPR_IMPL( editableProperty );
      }

      bool doesMatch( const ReflectionType& checkedClassType ) const
      {
         return checkedClassType.isA( m_refClass );
      }
   };

   // -------------------------------------------------------------------------

   template< class REPR_IMPL >
   class TEnumCreator : public Creator
   {
      DECLARE_ALLOCATOR( TEnumCreator, AM_DEFAULT );

   private:
      const ReflectionType&      m_genericEnumType;

   public:
      TEnumCreator()
         : m_genericEnumType( TSingleton< ReflectionTypesRegistry >::getInstance().getGenericEnumType() )
      {
      }

      ReflectionPropertyEditor* operator()( ReflectionProperty* property )
      {
         TEditableReflectionProperty< ReflectionEnum >* editableProperty = new TEditableReflectionProperty< ReflectionEnum >( property );
         return new REPR_IMPL( editableProperty );
      }

      bool doesMatch( const ReflectionType& checkedClassType ) const
      {
         return checkedClassType.isA( m_genericEnumType );
      }
   };

   // -------------------------------------------------------------------------
   // -------------------------------------------------------------------------
   // -------------------------------------------------------------------------

   template< typename NodeType, typename ParamType >
   class EditorNodeCreator
   {
      DECLARE_ALLOCATOR( EditorNodeCreator, AM_DEFAULT );

   public:
      virtual ~EditorNodeCreator() {}

      virtual NodeType* create( ParamType* obj )
      {
         // we're providing this generic implementation, because this type will be used if the user doesn't
         // register his own ObjectNodeCreator or ArrayNodeCreator.
         return new NodeType( obj );
      }
   };

   // -------------------------------------------------------------------------

   typedef EditorNodeCreator< ReflectionPropertyEditorComposite, ReflectionPropertyArray > ArrayNodeCreator;
   typedef EditorNodeCreator< ReflectionObjectEditor, ReflectionObject > ObjectNodeCreator;

   // -------------------------------------------------------------------------

   template< class REPR_IMPL >
   class TArrayNodeCreator : public ArrayNodeCreator
   {
      DECLARE_ALLOCATOR( TArrayNodeCreator, AM_DEFAULT );

   public:
      ReflectionPropertyEditorComposite* create( ReflectionPropertyArray* property )
      {
         return new REPR_IMPL( property );
      }
   };

   // -------------------------------------------------------------------------

   template< class REPR_IMPL >
   class TObjectNodeCreator : public ObjectNodeCreator
   {
      DECLARE_ALLOCATOR( TObjectNodeCreator, AM_DEFAULT );

   public:
      ReflectionObjectEditor* create( ReflectionObject* property )
      {
         return new REPR_IMPL( property );
      }
   };

   // -------------------------------------------------------------------------

   typedef std::vector< Creator* > CreatorsVec;

private:
   ReflectionObject*                                        m_parentObject;
   ReflectionObjectEditor*				                        m_rootEditor;
   CreatorsVec                                              m_creators;
   ArrayNodeCreator*                                        m_arrayNodeCreator;
   ObjectNodeCreator*                                       m_objectNodeCreator;

   bool                                                     m_inspectEditableOnly;

public:
   /**
    * Constructor.
    *
    * @inspectEditableOnly       only editable properties will be listed
    */
   ReflectionPropertiesView( bool inspectEditableOnly = true );
   virtual ~ReflectionPropertiesView();

   /**
    * Returns the specified editor
    */
   inline ReflectionObjectEditor* getRootEditor() const { return m_rootEditor; }

   /**
    * Sets the new view contents.
    *
    * @param parentObject  
    */
   void set( ReflectionObject& parentObject );

   /**
    * Resets the view contents.
    */
   virtual void reset();

   // -------------------------------------------------------------------------
   // ReflectionObjectChangeListener implementation
   // -------------------------------------------------------------------------
   void onObservedObjectDeleted( ReflectionObject* deletedObject );

   // -------------------------------------------------------------------------
   // Factory related functionality
   // -------------------------------------------------------------------------
   /**
    * This method associates an entity class with another class representing it.
    * From now on whenever a create method on such 'entity' class is 
    * called, this very representation will be created for it.
    *
    * The call to this method can be chained, allowing to set up
    * the whole factory in a single code block.
    *
    * @param PROPERTY_IMPL             solid type property
    * @param EDITOR_IMPL               property editor class
    */
   template< class PROPERTY_IMPL, class EDITOR_IMPL >
   void associate()
   {
      m_creators.push_back( new TStaticCreator< EDITOR_IMPL, PROPERTY_IMPL >() );
   }

   /**
    * This method associates an abstract property with an editor.
    *
    * @param ABSTRACT_PROPERTY_IMPL    abstract type property
    * @param EDITOR_IMPL               property editor class
    */
   template< class ABSTRACT_PROPERTY_IMPL, class EDITOR_IMPL >
   void associateAbstract()
   {
      m_creators.push_back( new TPolymorphicCreator< EDITOR_IMPL, ABSTRACT_PROPERTY_IMPL >() );
   }

   /**
    * This method defines the enum properties with an editor.
    *
    * @param EDITOR_IMPL               property editor class
    */
   template< class EDITOR_IMPL >
   void defineEnums()
   {
      m_creators.push_back( new TEnumCreator< EDITOR_IMPL >() );
   }

   /**
    * This method defines the array properties with an editor.
    *
    * @param EDITOR_IMPL               array property editor class
    */
   template< class EDITOR_IMPL >
   void defineArray()
   {
      delete m_arrayNodeCreator;
      m_arrayNodeCreator = new TArrayNodeCreator< EDITOR_IMPL >();
   }

   /**
    * This method defines an object node with a concrete editor.
    *
    * @param EDITOR_IMPL               object node editor class
    */
   template< class EDITOR_IMPL >
   void defineObjectNode()
   {
      delete m_objectNodeCreator;
      m_objectNodeCreator = new TObjectNodeCreator< EDITOR_IMPL >();
   }

   /**
    * The method creates a representation for an entity class.
    * Such an entity class needs to be registered with the factory
    * prior to this call.
    *
    * @param property   property for which we want to create a representation
    * @return           representation
    */ 
   ReflectionPropertyEditor* create( ReflectionProperty* property );

protected:
   // -------------------------------------------------------------------------
   // Notifications
   // -------------------------------------------------------------------------

   /**
    * Called when new properties are being set in the editor.
    *
    * @param typeName   type of the class which properties are about to be edited
    */
   virtual void onSet( ReflectionObjectEditor* rootEditor ) {}

private:
   void analyzeSingleProperty( ReflectionProperty* analyzedProperty, ReflectionObjectEditor* populatedObjectEditor, std::vector< ReflectionObjectEditor* >& outObjectNodes );
};

///////////////////////////////////////////////////////////////////////////////

#include "core\ReflectionPropertiesView.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // REFLECTION_PROPERTIES_VIEW_H

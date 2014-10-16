/// @file   TamyEditor/TreeWidgetDescFactory.h
/// @brief  a common interface for the factories of TreeWidget item types descriptions
#pragma once


///////////////////////////////////////////////////////////////////////////////

class QIcon;
class QString;
class SerializableReflectionType;

///////////////////////////////////////////////////////////////////////////////

/**
 * A factory for creating the description of node types 
 * the tree will be able to create.
 */
class TreeWidgetDescFactory
{
public:
   virtual ~TreeWidgetDescFactory() {}

   /**
    * The number of different types can the factory create.
    */
   virtual unsigned int typesCount() const = 0;

   /**
    * Description of the specific type.
    *
    * @param idx        type index
    * @param outDesc    type class name
    * @param outGroup   type group name
    * @param outIcon    type icon
    */
   virtual void getDesc( unsigned int idx, QString& outDesc, QString& outGroup, QIcon& outIcon ) const = 0;

   /**
    * Description of the specific type.
    *
    * @param type
    * @param outDesc       type class name
    * @param outGroup      type group name
    * @param outIcon       type icon
    */
   virtual void getDesc( const SerializableReflectionType& type, QString& outDesc, QString& outGroup, QIcon& outIcon ) const = 0;

   /**
    * Returns the RTTI descriptor of the specified type ( if applicable )
    *
    * @param idx        type index
    */
   virtual const SerializableReflectionType* getClass( unsigned int idx ) const = 0;

};

///////////////////////////////////////////////////////////////////////////////

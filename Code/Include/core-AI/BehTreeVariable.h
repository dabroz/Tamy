/// @file   core-AI/BehaviorTreeVariable.h
/// @brief  variables used by behavior trees
#pragma once

#include "core\ReflectionObject.h"
#include "core\Vector.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BehaviorTreeRunner;

///////////////////////////////////////////////////////////////////////////////

/**
 * Variables used by the blend tree.
 */
class BehaviorTreeVariable : public ReflectionObject
{
   DECLARE_ALLOCATOR( BehaviorTreeVariable, AM_ALIGNED_16 );
   DECLARE_CLASS();

private:
   // static data
   std::string                m_name;

   // runtime data
   uint                       m_varIndex;

public:
   /**
    * Constructor.
    *
    * @param name
    */
   BehaviorTreeVariable( const char* name = "Variable" );
   virtual ~BehaviorTreeVariable();

   /**
    * Gives the variable a new name.
    *
    * @param name
    */
   void setName( const char* name );

   /**
    * Returns the name of the variable.
    */
   inline const std::string& getName() const { return m_name; }

   /**
    * Returns the variable's internal index.
    */
   inline uint getIndex() const { return m_varIndex; }

   /**
    * Assigns the variable an internal index in the host BlendTree's internal array.
    *
    * @param idx
    */
   inline void setIndex( uint idx ) { m_varIndex = idx; }

   /**
    * Initializes the runtime context.
    * 
    * @param player
    */
   virtual void initializeLayout( BehaviorTreeRunner* player ) const = 0;

   /**
    * Returns the type of represented data
    */
   virtual uint getDataTypeID() const = 0;

   /**
    * Returns the variable value in a text form.
    * The specified character buffer needs to be initialized and capable of storing
    * a string at least 24 characters long.
    *
    * @param outStrBuf
    */
   virtual void valueToString( char* outStrBuf ) const = 0;

   /**
    * Returns runtime value of the variable in a text form.
    * The specified character buffer needs to be initialized and capable of storing
    * a string at least 24 characters long.
    *
    */
   virtual void runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A variable that stores a float value.
 */
class BehTreeVarFloat : public BehaviorTreeVariable
{
   DECLARE_ALLOCATOR( BehTreeVarFloat, AM_ALIGNED_16 );
   DECLARE_CLASS();

public:
   static ReflectionTypeID< float >       s_dataType;

private:
   // static data
   float                                  m_value;

   // runtime data
   TRuntimeVar< float >                   m_runtimeValue;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param value
    */
   BehTreeVarFloat( const char* name = "floatVar", float value = 0.0f );

   /**
    * Sets the runtime value of the variable.
    *
    * @param val
    * @param player
    */
   void setRuntime( BehaviorTreeRunner* player, float val ) const;

   /**
    * Returns the runtime value of the variable.
    *
    * @param player
    */
   float getRuntime( BehaviorTreeRunner* player ) const;

   /**
    * Sets the variable value.
    *
    * @param val
    */
   void set( float val );

   /**
    * Returns the value of the variable
    */
   inline float get() const { return m_value; }

   // -------------------------------------------------------------------------
   // BehaviorTreeVariable implementation
   // -------------------------------------------------------------------------
   void valueToString( char* outStrBuf ) const;
   void runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const;
   void initializeLayout( BehaviorTreeRunner* player ) const;
   inline uint getDataTypeID() const { return s_dataType.m_id; }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A variable that stores a bool value.
 */
class BehTreeVarBool : public BehaviorTreeVariable
{
   DECLARE_ALLOCATOR( BehTreeVarBool, AM_ALIGNED_16 );
   DECLARE_CLASS();

public:
   static ReflectionTypeID< bool >       s_dataType;

private:
   // static data
   bool                                   m_value;

   // runtime data
   TRuntimeVar< bool >                    m_runtimeValue;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param value
    */
   BehTreeVarBool( const char* name = "boolVar", bool value = false );

   /**
    * Sets the runtime value of the variable.
    *
    * @param val
    * @param player
    */
   void setRuntime( BehaviorTreeRunner* player, bool val ) const;

   /**
    * Returns the runtime value of the variable.
    *
    * @param player
    */
   bool getRuntime( BehaviorTreeRunner* player ) const;

   /**
    * Sets the variable value.
    *
    * @param val
    */
   void set( bool val );

   /**
    * Returns the value of the variable
    */
   inline bool get() const { return m_value; }

   // -------------------------------------------------------------------------
   // BehaviorTreeVariable implementation
   // -------------------------------------------------------------------------
   void valueToString( char* outStrBuf ) const;
   void runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const;
   void initializeLayout( BehaviorTreeRunner* player ) const;
   inline  uint getDataTypeID() const { return s_dataType.m_id; }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A variable that stores an integer value.
 */
class BehTreeVarInt : public BehaviorTreeVariable
{
   DECLARE_ALLOCATOR( BehTreeVarInt, AM_ALIGNED_16 );
   DECLARE_CLASS();

public:
   static ReflectionTypeID< int >         s_dataType;

private:
   // static data
   int                                    m_value;

   // runtime data
   TRuntimeVar< int >                     m_runtimeValue;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param value
    */
   BehTreeVarInt( const char* name = "intVar", int value = 0 );

   /**
    * Sets the runtime value of the variable.
    *
    * @param val
    * @param player
    */
   void setRuntime( BehaviorTreeRunner* player, int val ) const;

   /**
    * Returns the runtime value of the variable.
    *
    * @param player
    */
   int getRuntime( BehaviorTreeRunner* player ) const;

   /**
    * Sets the variable value.
    *
    * @param val
    */
   void set( int val );

   /**
    * Returns the value of the variable
    */
   inline int get() const { return m_value; }

   // -------------------------------------------------------------------------
   // BehaviorTreeVariable implementation
   // -------------------------------------------------------------------------
   void valueToString( char* outStrBuf ) const;
   void runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const;
   void initializeLayout( BehaviorTreeRunner* player ) const;
   inline uint getDataTypeID() const { return s_dataType.m_id; }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A variable that stores a vector value.
 */
class BehTreeVarVector : public BehaviorTreeVariable
{
   DECLARE_ALLOCATOR( BehTreeVarVector, AM_ALIGNED_16 );
   DECLARE_CLASS();

public:
   static ReflectionTypeID< Vector >            s_dataType;

private:
   // static data
   Vector                                       m_value;

   // runtime data
   TRuntimeVar< Vector >                        m_runtimeValue;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param value
    */
   BehTreeVarVector( const char* name = "vectorVar", const Vector& value = Vector_ZERO );

   /**
    * Sets the runtime value of the variable.
    *
    * @param val
    * @param player
    */
   void setRuntime( BehaviorTreeRunner* player, const Vector& val ) const;

   /**
    * Returns the runtime value of the variable.
    *
    * @param player
    */
   const Vector& getRuntime( BehaviorTreeRunner* player ) const;

   /**
    * Sets the variable value.
    *
    * @param val
    */
   void set( const Vector& val );

   /**
    * Returns the value of the variable
    */
   inline const Vector& get() const { return m_value; }

   // -------------------------------------------------------------------------
   // BehaviorTreeVariable implementation
   // -------------------------------------------------------------------------
   void valueToString( char* outStrBuf ) const;
   void runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const;
   void initializeLayout( BehaviorTreeRunner* player ) const;
   inline uint getDataTypeID() const { return s_dataType.m_id; }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A variable that stores a pointer to a ReflectionObject
 */
class BehTreeVarReflectionObject : public BehaviorTreeVariable
{
   DECLARE_ALLOCATOR( BehTreeVarReflectionObject, AM_ALIGNED_16 );
   DECLARE_CLASS();

public:
   static ReflectionTypeID< ReflectionObject >  s_dataType;

private:
   // static data
   const ReflectionObject*                      m_value;

   // runtime data
   TRuntimeVar< const ReflectionObject* >       m_runtimeValue;

public:
   /**
    * Constructor.
    *
    * @param name
    * @param value
    */
   BehTreeVarReflectionObject( const char* name = "refObjVar", const ReflectionObject* value = NULL );

   /**
    * Sets the runtime value of the variable.
    *
    * @param player
    * @param val
    */
   void setRuntime( BehaviorTreeRunner* player, const ReflectionObject* val ) const;

   /**
    * Returns the runtime value of the variable.
    *
    * @param player
    */
   const ReflectionObject* getRuntime( BehaviorTreeRunner* player ) const;

   /**
    * Sets the variable value.
    *
    * @param val
    */
   void set( const ReflectionObject* val );

   /**
    * Returns the value of the variable
    */
   inline const ReflectionObject* get() const { return m_value; }

   // -------------------------------------------------------------------------
   // BehaviorTreeVariable implementation
   // -------------------------------------------------------------------------
   void valueToString( char* outStrBuf ) const;
   void runtimeValueToString( BehaviorTreeRunner* player, char* outStrBuf ) const;
   void initializeLayout( BehaviorTreeRunner* player ) const;
   inline uint getDataTypeID() const { return s_dataType.m_id; }
};

///////////////////////////////////////////////////////////////////////////////

/// @file   core-AI/BlendTreeVariable.h
/// @brief  variables used by blend trees
#ifndef _BLEND_TREE_VARIABLE_H
#define _BLEND_TREE_VARIABLE_H

#include "core\ReflectionObject.h"
#include "core\Vector.h"
#include "core\RuntimeData.h"


///////////////////////////////////////////////////////////////////////////////

class BlendTreePlayer;

///////////////////////////////////////////////////////////////////////////////

/**
 * Variables used by the blend tree.
 */
class BlendTreeVariable : public ReflectionObject
{
   DECLARE_ALLOCATOR( BlendTreeVariable, AM_ALIGNED_16 );
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
   BlendTreeVariable( const char* name = "Variable" );
   virtual ~BlendTreeVariable();

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
   virtual void initializeLayout( BlendTreePlayer* player ) const = 0;

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
   virtual void runtimeValueToString( BlendTreePlayer* player, char* outStrBuf ) const = 0;

   // -------------------------------------------------------------------------
   // Data comparison methods
   // -------------------------------------------------------------------------
   virtual bool isEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const = 0;
   virtual bool isLess( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const = 0;
   virtual bool isLessEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const = 0;
   virtual bool isGreater( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const = 0;
   virtual bool isGreaterEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A variable that stores a float value.
 */
class BTVarFloat : public BlendTreeVariable
{
   DECLARE_ALLOCATOR( BTVarFloat, AM_ALIGNED_16 );
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
   BTVarFloat( const char* name = "floatVar", float value = 0.0f );

   /**
    * Sets the runtime value of the variable.
    *
    * @param val
    * @param player
    */
   void setRuntime( BlendTreePlayer* player, float val ) const;

   /**
    * Returns the runtime value of the variable.
    *
    * @param player
    */
   float getRuntime( BlendTreePlayer* player ) const;

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
   // BlendTreeVariable implementation
   // -------------------------------------------------------------------------
   void valueToString( char* outStrBuf ) const;
   void runtimeValueToString( BlendTreePlayer* player, char* outStrBuf ) const;
   void initializeLayout( BlendTreePlayer* player ) const;
   inline uint getDataTypeID() const { return s_dataType.m_id; }
   bool isEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isLess( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isLessEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isGreater( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isGreaterEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A variable that stores a bool value.
 */
class BTVarBool : public BlendTreeVariable
{
   DECLARE_ALLOCATOR( BTVarBool, AM_ALIGNED_16 );
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
   BTVarBool( const char* name = "boolVar", bool value = false );

   /**
    * Sets the runtime value of the variable.
    *
    * @param val
    * @param player
    */
   void setRuntime( BlendTreePlayer* player, bool val ) const;

   /**
    * Returns the runtime value of the variable.
    *
    * @param player
    */
   bool getRuntime( BlendTreePlayer* player ) const;

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
   // BlendTreeVariable implementation
   // -------------------------------------------------------------------------
   void valueToString( char* outStrBuf ) const;
   void runtimeValueToString( BlendTreePlayer* player, char* outStrBuf ) const;
   void initializeLayout( BlendTreePlayer* player ) const;
   inline  uint getDataTypeID() const { return s_dataType.m_id; }
   bool isEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isLess( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isLessEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isGreater( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isGreaterEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A variable that stores an integer value.
 */
class BTVarInt : public BlendTreeVariable
{
   DECLARE_ALLOCATOR( BTVarInt, AM_ALIGNED_16 );
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
   BTVarInt( const char* name = "intVar", int value = 0 );

   /**
    * Sets the runtime value of the variable.
    *
    * @param val
    * @param player
    */
   void setRuntime( BlendTreePlayer* player, int val ) const;

   /**
    * Returns the runtime value of the variable.
    *
    * @param player
    */
   int getRuntime( BlendTreePlayer* player ) const;

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
   // BlendTreeVariable implementation
   // -------------------------------------------------------------------------
   void valueToString( char* outStrBuf ) const;
   void runtimeValueToString( BlendTreePlayer* player, char* outStrBuf ) const;
   void initializeLayout( BlendTreePlayer* player ) const;
   inline uint getDataTypeID() const { return s_dataType.m_id; }
   bool isEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isLess( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isLessEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isGreater( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isGreaterEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A variable that stores a vector value.
 */
class BTVarVector : public BlendTreeVariable
{
   DECLARE_ALLOCATOR( BTVarVector, AM_ALIGNED_16 );
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
   BTVarVector( const char* name = "vectorVar", const Vector& value = Vector_ZERO );

   /**
    * Sets the runtime value of the variable.
    *
    * @param val
    * @param player
    */
   void setRuntime( BlendTreePlayer* player, const Vector& val ) const;

   /**
    * Returns the runtime value of the variable.
    *
    * @param player
    */
   const Vector& getRuntime( BlendTreePlayer* player ) const;

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
   // BlendTreeVariable implementation
   // -------------------------------------------------------------------------
   void valueToString( char* outStrBuf ) const;
   void runtimeValueToString( BlendTreePlayer* player, char* outStrBuf ) const;
   void initializeLayout( BlendTreePlayer* player ) const;
   inline uint getDataTypeID() const { return s_dataType.m_id; }
   bool isEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isLess( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isLessEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isGreater( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
   bool isGreaterEqual( BlendTreePlayer* player, const BlendTreeVariable& rhs ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core-AI\BlendTreeVariable.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _BLEND_TREE_VARIABLE_H

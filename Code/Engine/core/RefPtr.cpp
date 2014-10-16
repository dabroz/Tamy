#include "core\RefPtr.h"
#include "core\ReflectionObject.h"
#include "core\OutStream.h"
#include "core\InStream.h"


///////////////////////////////////////////////////////////////////////////////

RefPtr::RefPtr( ReflectionObject* obj )
   : m_obj( obj )
{
   if ( m_obj )
   {
      m_obj->addReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

RefPtr::RefPtr( const RefPtr& rhs )
   : m_obj( rhs.m_obj )
{
   if ( m_obj )
   {
      m_obj->addReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

RefPtr::~RefPtr()
{
   if ( m_obj )
   {
      m_obj->removeReference();
   }
}

///////////////////////////////////////////////////////////////////////////////

void RefPtr::setDuringSerialization( ReflectionObject* obj )
{
   m_obj = obj;
}

///////////////////////////////////////////////////////////////////////////////

RefPtr& RefPtr::operator=( const RefPtr& rhs )
{
   // we're ditching the old object in favor of the new one, so we need to decrease
   // the old one's reference counter ( since we're not gonna be referencing it any longer )
   // and increase the new one's
   if ( m_obj )
   {
      m_obj->removeReference();
   }

   m_obj = rhs.m_obj;

   if ( m_obj )
   {
      m_obj->addReference();
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

RefPtr& RefPtr::operator=( ReflectionObject* obj )
{
   // we're ditching the old object in favor of the new one, so we need to decrease
   // the old one's reference counter ( since we're not gonna be referencing it any longer )
   // and increase the new one's
   if ( m_obj )
   {
      m_obj->removeReference();
   }

   m_obj = obj;

   if ( m_obj )
   {
      m_obj->addReference();
   }

   return *this;
}

///////////////////////////////////////////////////////////////////////////////

bool RefPtr::operator==( const RefPtr& rhs ) const
{
   return m_obj == rhs.m_obj;
}

///////////////////////////////////////////////////////////////////////////////

bool RefPtr::operator==( const ReflectionObject* obj ) const
{
   return m_obj == obj;
}

///////////////////////////////////////////////////////////////////////////////

bool RefPtr::operator!=( const RefPtr& rhs ) const
{
   return m_obj != rhs.m_obj;
}

///////////////////////////////////////////////////////////////////////////////

bool RefPtr::operator!=( const ReflectionObject* obj ) const
{
   return m_obj != obj;
}

///////////////////////////////////////////////////////////////////////////////

bool RefPtr::operator<( const RefPtr& rhs ) const
{
   return m_obj < rhs.m_obj;
}

///////////////////////////////////////////////////////////////////////////////

bool RefPtr::operator<( const ReflectionObject* obj ) const
{
   return m_obj < obj;
}

///////////////////////////////////////////////////////////////////////////////

bool RefPtr::operator>( const RefPtr& rhs ) const
{
   return m_obj > rhs.m_obj;
}

///////////////////////////////////////////////////////////////////////////////

bool RefPtr::operator>( const ReflectionObject* obj ) const
{
   return m_obj > obj;
}

///////////////////////////////////////////////////////////////////////////////

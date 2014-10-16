/// @file   core/Delegate.h
/// @brief  a fast delegate
#pragma once

#include "core\MemoryRouter.h"


///////////////////////////////////////////////////////////////////////////////

#define FROM_METHOD(ClassName, methodName, instance) from_method<ClassName, &ClassName::methodName>(instance)

///////////////////////////////////////////////////////////////////////////////

template <typename SIGNATURE>
class Delegate;

///////////////////////////////////////////////////////////////////////////////

template <typename R>
class Delegate< R ( ) >
{
   DECLARE_ALLOCATOR( Delegate, AM_DEFAULT );

public:
    
    typedef R return_type;

    Delegate(const Delegate<R()>& d)
    :    object_ptr(d.object_ptr)
    ,    stub_ptr(d.stub_ptr)
    {
    }
        
    Delegate(int = 0)
    :    object_ptr(0)
    ,    stub_ptr(0)
    {}

    template <class T, return_type (T::*METHOD)()>
    static Delegate<R()> from_method(T* object_ptr)
    {
        Delegate d;
        d.object_ptr = object_ptr;
        d.stub_ptr = &method_stub<T, METHOD>;
        return d;
    }

    template <return_type (*METHOD)()>
    static Delegate<R()> from_function()
    {
        Delegate d;
        d.object_ptr = 0;
        d.stub_ptr = &function_stub<METHOD>;
        return d;
    }
    
    template <typename T, return_type (T::*METHOD)()>
    void bind(void* t)
    {
        object_ptr = t;
        stub_ptr = &method_stub<T, METHOD>;
    }
    
    bool empty() const
    {
        return stub_ptr == NULL;
    }

    void clear()
    {
        stub_ptr = NULL;
        object_ptr = NULL;
    }

    return_type operator()() const
    {
        return (*stub_ptr)(object_ptr);
    }

    bool operator==(const Delegate< R ( ) >& other) const
    {
        return stub_ptr == other.stub_ptr  &&  object_ptr == other.object_ptr;
    }

    bool operator!=(const Delegate< R ( ) >& other) const
    {
        return !(*this == other);
    }
    
protected:
    typedef return_type (*stub_type)(void* object_ptr);

    void* object_ptr;
    stub_type stub_ptr;

    template <class T, return_type (T::*METHOD)()>
    static return_type method_stub(void* object_ptr)
    {
        T* p = static_cast<T*>(object_ptr);
        return (p->*METHOD)();
    }
    
    template <return_type (*METHOD)()>
    static return_type function_stub(void* object_ptr)
    {
        return (*METHOD)();
    }
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A one-param delegate.
 */
template <typename R, typename PARAM1>
class Delegate< R (PARAM1) >
{
   DECLARE_ALLOCATOR( Delegate, AM_DEFAULT );

public:
    
    typedef R return_type;
    typedef PARAM1 param_type;

    // Forced conversion to a different delegate type.  Use for derived classes.
    template <class OTHER>
    explicit Delegate(Delegate<OTHER>& other)
    :    object_ptr(other.object_ptr)
    ,    stub_ptr(reinterpret_cast<stub_type>(other.stub_ptr))
    {
    }

    Delegate(int = 0)
    :    object_ptr(0)
    ,    stub_ptr(0)
    {}

    template <class T, return_type (T::*METHOD)(PARAM1)>
    static Delegate<return_type (PARAM1)> from_method(T* object_ptr)
    {
        Delegate d;
        d.object_ptr = object_ptr;
        d.stub_ptr = &method_stub<T, METHOD>;
        return d;
    }
    
    template <return_type (*METHOD)(PARAM1)>
    static Delegate<return_type (PARAM1)> from_function()
    {
        Delegate d;
        d.object_ptr = 0;
        d.stub_ptr = &function_stub<METHOD>;
        return d;
    }

    template <typename T, return_type (T::*METHOD)(param_type)>
    void bind(void* t)
    {
        object_ptr = t;
        stub_ptr = &method_stub<T, METHOD>;
    }
    
    bool operator==(const Delegate< return_type (PARAM1) >& other) const
    {
        return stub_ptr == other.stub_ptr  &&  object_ptr == other.object_ptr;
    }
    
    bool operator!=(const Delegate< return_type (PARAM1) >& other) const
    {
        return !(*this == other);
    }

    bool empty() const
    {
        return stub_ptr == NULL;
    }

    void clear()
    {
        stub_ptr = NULL;
        object_ptr = NULL;
    }

    return_type operator()(PARAM1 p1) const
    {
        return (*stub_ptr)(object_ptr, p1);
    }

// protected:
    typedef return_type (*stub_type)(void* object_ptr, PARAM1 p1);

    void* object_ptr;
    stub_type stub_ptr;

    template <class T, return_type (T::*METHOD)(PARAM1)>
    static return_type method_stub(void* object_ptr, PARAM1 p1)
    {
        T* p = static_cast<T*>(object_ptr);
        return (p->*METHOD)(p1);
    }

    template <return_type (*METHOD)(PARAM1)>
    static return_type function_stub(void* object_ptr, PARAM1 p1)
    {
        return (*METHOD)(p1);
    }
};

///////////////////////////////////////////////////////////////////////////////


/**
 * A two-param delegate.
 */
template <typename R, typename PARAM1, typename PARAM2>
class Delegate< R (PARAM1, PARAM2) >
{
   DECLARE_ALLOCATOR( Delegate, AM_DEFAULT );

public:
    
    typedef R return_type;
    typedef PARAM1 param1_type;
    typedef PARAM2 param2_type;

    // Forced conversion to a different delegate type.  Use for derived classes.
    template <class OTHER>
    explicit Delegate(Delegate<OTHER>& other)
    :    object_ptr(other.object_ptr)
    ,    stub_ptr(reinterpret_cast<stub_type>(other.stub_ptr))
    {
    }

    Delegate(int = 0)
    :    object_ptr(0)
    ,    stub_ptr(0)
    {}

    template <class T, return_type (T::*METHOD)(PARAM1, PARAM2)>
    static Delegate<return_type (PARAM1, PARAM2)> from_method(T* object_ptr)
    {
        Delegate d;
        d.object_ptr = object_ptr;
        d.stub_ptr = &method_stub<T, METHOD>;
        return d;
    }
    
    template <return_type (*METHOD)(PARAM1, PARAM2)>
    static Delegate<return_type (PARAM1, PARAM2)> from_function()
    {
        Delegate d;
        d.object_ptr = 0;
        d.stub_ptr = &function_stub<METHOD>;
        return d;
    }

    template <typename T, return_type (T::*METHOD)(param1_type, param2_type)>
    void bind(void* t)
    {
        object_ptr = t;
        stub_ptr = &method_stub<T, METHOD>;
    }
    
    bool operator==(const Delegate< return_type (PARAM1, PARAM2) >& other) const
    {
        return stub_ptr == other.stub_ptr  &&  object_ptr == other.object_ptr;
    }
    
    bool operator!=(const Delegate< return_type (PARAM1, PARAM2) >& other) const
    {
        return !(*this == other);
    }

    bool empty() const
    {
        return stub_ptr == NULL;
    }

    void clear()
    {
        stub_ptr = NULL;
        object_ptr = NULL;
    }

    return_type operator()(PARAM1 p1, PARAM2 p2) const
    {
        return (*stub_ptr)(object_ptr, p1, p2);
    }

// protected:
    typedef return_type (*stub_type)(void* object_ptr, PARAM1 p1, PARAM2 p2);

    void* object_ptr;
    stub_type stub_ptr;

    template <class T, return_type (T::*METHOD)(PARAM1, PARAM2)>
    static return_type method_stub(void* object_ptr, PARAM1 p1, PARAM2 p2)
    {
        T* p = static_cast<T*>(object_ptr);
        return (p->*METHOD)(p1, p2);
    }

    template <return_type (*METHOD)(PARAM1, PARAM2)>
    static return_type function_stub(void* object_ptr, PARAM1 p1, PARAM2 p2)
    {
        return (*METHOD)(p1, p2);
    }
};

///////////////////////////////////////////////////////////////////////////////

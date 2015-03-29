//
// ThreadLocal.h
//
// $Id: //poco/1.4/Foundation/include/Poco/ThreadLocal.h#1 $
//
// Library: Foundation
// Package: Threading
// Module:  Thread
//
// Definition of the ThreadLocal template and related classes.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Foundation_ThreadLocal_INCLUDED
#define Foundation_ThreadLocal_INCLUDED


#include "Poco/Foundation.h"
#include <map>


namespace Poco {


class Foundation_API TLSAbstractSlot
	/// This is the base class for all objects
	/// that the ThreadLocalStorage class manages.
{
public:
	TLSAbstractSlot();
	virtual ~TLSAbstractSlot();
};


template <class C>
class TLSSlot: public TLSAbstractSlot
	/// The Slot template wraps another class
	/// so that it can be stored in a ThreadLocalStorage
	/// object. This class is used internally, and you
	/// must not create instances of it yourself.
{
public:
	TLSSlot():
		_value()
	{
	}
	
	~TLSSlot()
	{
	}
	
	C& value()
	{
		return _value;
	}
	
private:
	TLSSlot(const TLSSlot&);
	TLSSlot& operator = (const TLSSlot&);

	C _value;
};


class Foundation_API ThreadLocalStorage
	/// This class manages the local storage for each thread.
	/// Never use this class directly, always use the
	/// ThreadLocal template for managing thread local storage.
{
public:
	ThreadLocalStorage();
		/// Creates the TLS.
		
	~ThreadLocalStorage();
		/// Deletes the TLS.

	TLSAbstractSlot*& get(const void* key);
		/// Returns the slot for the given key.
		
	static ThreadLocalStorage& current();
		/// Returns the TLS object for the current thread
		/// (which may also be the main thread).
		
	static void clear();
		/// Clears the current thread's TLS object.
		/// Does nothing in the main thread.
	
private:
	typedef std::map<const void*, TLSAbstractSlot*> TLSMap;
	
	TLSMap _map;

	friend class Thread;
};


template <class C>
class ThreadLocal
	/// This template is used to declare type safe thread
	/// local variables. It can basically be used like
	/// a smart pointer class with the special feature
	/// that it references a different object
	/// in every thread. The underlying object will
	/// be created when it is referenced for the first
	/// time.
	/// See the NestedDiagnosticContext class for an
	/// example how to use this template.
	/// Every thread only has access to its own
	/// thread local data. There is no way for a thread
	/// to access another thread's local data.
{
	typedef TLSSlot<C> Slot;

public:
	ThreadLocal()
	{
	}
	
	~ThreadLocal()
	{
	}
	
	C* operator -> ()
	{
		return &get();
	}
	
	C& operator * ()
		/// "Dereferences" the smart pointer and returns a reference
		/// to the underlying data object. The reference can be used
		/// to modify the object.
	{
		return get();
	}

	C& get()
		/// Returns a reference to the underlying data object.
		/// The reference can be used to modify the object.
	{
		TLSAbstractSlot*& p = ThreadLocalStorage::current().get(this);
		if (!p) p = new Slot;
		return static_cast<Slot*>(p)->value();
	}
	
private:
	ThreadLocal(const ThreadLocal&);
	ThreadLocal& operator = (const ThreadLocal&);
};


} // namespace Poco


#endif // Foundation_ThreadLocal_INCLUDED

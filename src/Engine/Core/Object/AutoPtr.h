#pragma once

// Pointer which takes ownership of an object and deletes it when the pointer goes out of scope. Ownership can be transferred to another pointer, in which case the source pointer becomes null.
template <class T> 
class AutoPtr
{
public:
	AutoPtr() = default;
	AutoPtr(const AutoPtr<T>& ptr) : m_ptr(ptr.m_ptr)
	{
		const_cast<AutoPtr<T>&>(ptr).m_ptr = nullptr;
	}

	AutoPtr(T* ptr) : m_ptr(ptr) {}

	~AutoPtr()
	{
		delete m_ptr;
	}

	// Assign from a pointer. Existing object is deleted and ownership is transferred from the source pointer, which becomes null.
	AutoPtr<T>& operator=(const AutoPtr<T>& rhs)
	{
		delete m_ptr;
		m_ptr = rhs.m_ptr;
		const_cast<AutoPtr<T>&>(rhs).m_ptr = nullptr;
		return *this;
	}

	// Assign a new object. Existing object is deleted.
	AutoPtr<T>& operator=(T* rhs)
	{
		delete m_ptr;
		m_ptr = rhs;
		return *this;
	}

	// Detach the object from the pointer without destroying it and return it. The pointer becomes null.
	T* Detach()
	{
		T* ret = m_ptr;
		m_ptr = nullptr;
		return ret;
	}

	// Reset to null. Destroys the object.
	void Reset()
	{
		*this = nullptr;
	}

	// Point to the object.
	T* operator->() const { assert(m_ptr); return m_ptr; }
	// Dereference the object.
	T& operator*() const { assert(m_ptr); return *m_ptr; }
	// Convert to the object.
	operator T*() const { return m_ptr; }

	// Return the object.
	T* Get() const { return m_ptr; }
	// Return whether is a null pointer.
	bool IsNull() const { return m_ptr == nullptr; }

private:
	T* m_ptr = nullptr;
};
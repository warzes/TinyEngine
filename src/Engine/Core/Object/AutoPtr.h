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
	operator T* () const { return m_ptr; }

	// Return the object.
	T* Get() const { return m_ptr; }
	// Return whether is a null pointer.
	bool IsNull() const { return m_ptr == nullptr; }

private:
	T* m_ptr = nullptr;
};

// Pointer which takes ownership of an array allocated with new[] and deletes it when the pointer goes out of scope.
template <class T> class AutoArrayPtr
{
public:
	/// Construct a null pointer.
	AutoArrayPtr() :
		array(nullptr)
	{
	}

	/// Copy-construct. Ownership is transferred, making the source pointer null.
	AutoArrayPtr(const AutoArrayPtr<T>& ptr) :
		array(ptr.array)
	{
		const_cast<AutoArrayPtr<T>&>(ptr).array = nullptr;
	}

	/// Construct and take ownership of the array.
	AutoArrayPtr(T* array_) :
		array(array_)
	{
	}

	/// Destruct. Delete the array pointed to.
	~AutoArrayPtr()
	{
		delete[] array;
	}

	/// Assign from a pointer. Existing array is deleted and ownership is transferred from the source pointer, which becomes null.
	AutoArrayPtr<T>& operator = (AutoArrayPtr<T>& rhs)
	{
		delete array;
		array = rhs.array;
		rhs.array = nullptr;
		return *this;
	}

	/// Assign a new array. Existing array is deleted.
	AutoArrayPtr<T>& operator = (T* rhs)
	{
		delete array;
		array = rhs;
		return *this;
	}

	/// Detach the array from the pointer without destroying it and return it. The pointer becomes null.
	T* Detach()
	{
		T* ret = array;
		array = nullptr;
		return ret;
	}

	/// Reset to null. Destroys the array.
	void Reset()
	{
		*this = nullptr;
	}

	/// Point to the array.
	T* operator -> () const { assert(array); return array; }
	/// Dereference the array.
	T& operator * () const { assert(array); return *array; }
	/// Convert to the array element pointer.
	operator T* () const { return Get(); }

	/// Return the array.
	T* Get() const { return array; }
	/// Return whether is a null pointer.
	bool IsNull() const { return array == nullptr; }

private:
	/// Array pointer.
	T* array;
};
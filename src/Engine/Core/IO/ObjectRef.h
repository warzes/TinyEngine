#pragma once

// Reference to an object with id for serialization.
struct ObjectRef
{
	ObjectRef() = default;
	ObjectRef(const ObjectRef& ref) : id(ref.id) { }
	ObjectRef(unsigned id_) : id(id_) {}

	bool operator == (const ObjectRef& rhs) const { return id == rhs.id; }
	bool operator != (const ObjectRef& rhs) const { return !(*this == rhs); }

	unsigned id = 0;
};
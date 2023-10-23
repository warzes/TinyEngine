#include "stdafx.h"
#include "Attribute.h"
#include "Core/Geometry/Rect.h"
#include "Core/Geometry/BoundingAABB.h"
#include "Core/Geometry/IntBox.h"
#include "Core/Math/Color.h"
#include "Core/IO/JSONValue.h"
#include "Core/Utilities/StringUtilities.h"
#include "Core/IO/ObjectRef.h"
#include "Core/IO/ResourceRef.h"

const std::string Attribute::typeNames[] =
{
	"bool",
	"byte",
	"unsigned",
	"int",
	"IntVector2",
	"IntVector3",
	"IntRect",
	"IntBox",
	"float",
	"Vector2",
	"Vector3",
	"Vector4",
	"Quaternion",
	"Color",
	"Rect",
	"BoundingBox",
	"Matrix3",
	"Matrix3x4",
	"Matrix4",
	"String",
	"ResourceRef",
	"ResourceRefList",
	"ObjectRef",
	"JSONValue",
	""
};

const size_t Attribute::byteSizes[] =
{
sizeof(bool),
sizeof(unsigned char),
sizeof(unsigned),
sizeof(int),
sizeof(glm::ivec2),
sizeof(glm::ivec3),
sizeof(IntRect),
sizeof(IntBox),
sizeof(float),
sizeof(glm::vec2),
sizeof(glm::vec3),
sizeof(glm::vec4),
sizeof(glm::quat),
sizeof(ColorF),
sizeof(Rect),
sizeof(BoundingAABB),
sizeof(glm::mat3),
sizeof(glm::mat3x4),
sizeof(glm::mat4),
0,
0,
0,
sizeof(unsigned),
0,
0
};

AttributeAccessor::~AttributeAccessor()
{
}

Attribute::Attribute(const char* name_, AttributeAccessor* accessor_, const char** enumNames_) :
	name(name_),
	accessor(accessor_),
	enumNames(enumNames_)
{
}

void Attribute::FromValue(Serializable* instance, const void* source)
{
	accessor->Set(instance, source);
}

void Attribute::ToValue(Serializable* instance, void* dest)
{
	accessor->Get(instance, dest);
}

void Attribute::Skip(AttributeType type, Stream& source)
{
	if (byteSizes[type])
	{
		source.Seek(source.Position() + byteSizes[type]);
		return;
	}

	switch (type)
	{
	case ATTR_STRING:
		source.Read<std::string>();
		break;

	case ATTR_RESOURCEREF:
		source.Read<ResourceRef>();
		break;

	case ATTR_RESOURCEREFLIST:
		source.Read<ResourceRefList>();
		break;

	case ATTR_OBJECTREF:
		source.Read<ObjectRef>();
		break;

	case ATTR_JSONVALUE:
		source.Read<JSONValue>();
		break;

	default:
		break;
	}
}

const std::string& Attribute::TypeName() const
{
	return typeNames[Type()];
}

size_t Attribute::ByteSize() const
{
	return byteSizes[Type()];
}

void Attribute::FromJSON(AttributeType type, void* dest, const JSONValue& source)
{
	switch (type)
	{
	case ATTR_BOOL:
		*(reinterpret_cast<bool*>(dest)) = source.GetBool();
		break;

	case ATTR_BYTE:
		*(reinterpret_cast<unsigned char*>(dest)) = (unsigned char)source.GetNumber();
		break;

	case ATTR_UNSIGNED:
		*(reinterpret_cast<unsigned*>(dest)) = (unsigned)source.GetNumber();
		break;

	case ATTR_INT:
		*(reinterpret_cast<int*>(dest)) = (int)source.GetNumber();
		break;

	case ATTR_INTVECTOR2:
		StringUtils::FromString(*(reinterpret_cast<glm::ivec2*>(dest)), source.GetString().c_str());
		break;

	case ATTR_INTVECTOR3:
		StringUtils::FromString(*(reinterpret_cast<glm::ivec3*>(dest)), source.GetString().c_str());
		break;

	case ATTR_INTRECT:
		StringUtils::FromString(*(reinterpret_cast<IntRect*>(dest)), source.GetString().c_str());
		break;

	case ATTR_INTBOX:
		StringUtils::FromString(*(reinterpret_cast<IntBox*>(dest)), source.GetString().c_str());
		break;

	case ATTR_FLOAT:
		*(reinterpret_cast<float*>(dest)) = (float)source.GetNumber();
		break;

	case ATTR_VECTOR2:
		StringUtils::FromString(*(reinterpret_cast<glm::vec2*>(dest)), source.GetString().c_str());
		break;

	case ATTR_VECTOR3:
		StringUtils::FromString(*(reinterpret_cast<glm::vec3*>(dest)), source.GetString().c_str());
		break;

	case ATTR_VECTOR4:
		StringUtils::FromString(*(reinterpret_cast<glm::vec4*>(dest)), source.GetString().c_str());
		break;

	case ATTR_QUATERNION:
		StringUtils::FromString(*(reinterpret_cast<glm::quat*>(dest)), source.GetString().c_str());
		break;

	case ATTR_COLOR:
		StringUtils::FromString(*(reinterpret_cast<ColorF*>(dest)), source.GetString().c_str());
		break;

	case ATTR_RECT:
		StringUtils::FromString(*(reinterpret_cast<Rect*>(dest)), source.GetString().c_str());
		break;

	case ATTR_BOUNDINGBOX:
		StringUtils::FromString(*(reinterpret_cast<BoundingAABB*>(dest)), source.GetString().c_str());
		break;

	case ATTR_MATRIX3:
		StringUtils::FromString(*(reinterpret_cast<glm::mat3*>(dest)), source.GetString().c_str());
		break;

	case ATTR_MATRIX3X4:
		StringUtils::FromString(*(reinterpret_cast<glm::mat3x4*>(dest)), source.GetString().c_str());
		break;

	case ATTR_MATRIX4:
		StringUtils::FromString(*(reinterpret_cast<glm::mat4*>(dest)), source.GetString().c_str());
		break;

	case ATTR_STRING:
		*(reinterpret_cast<std::string*>(dest)) = source.GetString();
		break;

	case ATTR_RESOURCEREF:
		reinterpret_cast<ResourceRef*>(dest)->FromString(source.GetString());
		break;

	case ATTR_RESOURCEREFLIST:
		reinterpret_cast<ResourceRefList*>(dest)->FromString(source.GetString());
		break;

	case ATTR_OBJECTREF:
		reinterpret_cast<ObjectRef*>(dest)->id = (unsigned)source.GetNumber();
		break;

	case ATTR_JSONVALUE:
		*(reinterpret_cast<JSONValue*>(dest)) = source;
		break;

	default:
		break;
	}
}

void Attribute::ToJSON(AttributeType type, JSONValue& dest, const void* source)
{
	switch (type)
	{
	case ATTR_BOOL:
		dest = *(reinterpret_cast<const bool*>(source));
		break;

	case ATTR_BYTE:
		dest = *(reinterpret_cast<const unsigned char*>(source));
		break;

	case ATTR_UNSIGNED:
		dest = *(reinterpret_cast<const unsigned*>(source));
		break;

	case ATTR_INT:
		dest = *(reinterpret_cast<const int*>(source));
		break;

	case ATTR_INTVECTOR2:
		dest = StringUtils::ToString(*(reinterpret_cast<const glm::ivec2*>(source)));
		break;

	case ATTR_INTVECTOR3:
		dest = StringUtils::ToString(*(reinterpret_cast<const glm::ivec3*>(source)));
		break;

	case ATTR_INTRECT:
		dest = StringUtils::ToString(*(reinterpret_cast<const IntRect*>(source)));
		break;

	case ATTR_INTBOX:
		dest = StringUtils::ToString(*(reinterpret_cast<const IntBox*>(source)));
		break;

	case ATTR_FLOAT:
		dest = *(reinterpret_cast<const float*>(source));
		break;

	case ATTR_VECTOR2:
		dest = StringUtils::ToString(*(reinterpret_cast<const glm::vec2*>(source)));
		break;

	case ATTR_VECTOR3:
		dest = StringUtils::ToString(*(reinterpret_cast<const glm::vec3*>(source)));
		break;

	case ATTR_VECTOR4:
		dest = StringUtils::ToString(*(reinterpret_cast<const glm::vec4*>(source)));
		break;

	case ATTR_QUATERNION:
		dest = StringUtils::ToString(*(reinterpret_cast<const glm::quat*>(source)));
		break;

	case ATTR_COLOR:
		dest = StringUtils::ToString(*(reinterpret_cast<const ColorF*>(source)));
		break;

	case ATTR_RECT:
		dest = StringUtils::ToString(*(reinterpret_cast<const Rect*>(source)));
		break;

	case ATTR_BOUNDINGBOX:
		dest = StringUtils::ToString(*(reinterpret_cast<const BoundingAABB*>(source)));
		break;

	case ATTR_MATRIX3:
		dest = StringUtils::ToString(*(reinterpret_cast<const glm::mat3*>(source)));
		break;

	case ATTR_MATRIX3X4:
		dest = StringUtils::ToString(*(reinterpret_cast<const glm::mat3x4*>(source)));
		break;

	case ATTR_MATRIX4:
		dest = StringUtils::ToString(*(reinterpret_cast<const glm::mat4*>(source)));
		break;

	case ATTR_STRING:
		dest = *(reinterpret_cast<const std::string*>(source));
		break;

	case ATTR_RESOURCEREF:
		dest = reinterpret_cast<const ResourceRef*>(source)->ToString();
		break;

	case ATTR_RESOURCEREFLIST:
		dest = reinterpret_cast<const ResourceRefList*>(source)->ToString();
		break;

	case ATTR_OBJECTREF:
		dest = reinterpret_cast<const ObjectRef*>(source)->id;
		break;

	case ATTR_JSONVALUE:
		dest = *(reinterpret_cast<const JSONValue*>(source));
		break;

	default:
		break;
	}
}

AttributeType Attribute::TypeFromName(const std::string& name)
{
	return (AttributeType)StringUtils::ListIndex(name, typeNames, MAX_ATTR_TYPES);
}

AttributeType Attribute::TypeFromName(const char* name)
{
	return (AttributeType)StringUtils::ListIndex(name, typeNames, MAX_ATTR_TYPES);
}

template<> AttributeType AttributeImpl<bool>::Type() const
{
	return ATTR_BOOL;
}

template<> AttributeType AttributeImpl<int>::Type() const
{
	return ATTR_INT;
}

template<> AttributeType AttributeImpl<glm::ivec2>::Type() const
{
	return ATTR_INTVECTOR2;
}

template<> AttributeType AttributeImpl<glm::ivec3>::Type() const
{
	return ATTR_INTVECTOR3;
}

template<> AttributeType AttributeImpl<IntRect>::Type() const
{
	return ATTR_INTRECT;
}

template<> AttributeType AttributeImpl<IntBox>::Type() const
{
	return ATTR_INTBOX;
}

template<> AttributeType AttributeImpl<unsigned>::Type() const
{
	return ATTR_UNSIGNED;
}

template<> AttributeType AttributeImpl<unsigned char>::Type() const
{
	return ATTR_BYTE;
}

template<> AttributeType AttributeImpl<float>::Type() const
{
	return ATTR_FLOAT;
}

template<> AttributeType AttributeImpl<std::string>::Type() const
{
	return ATTR_STRING;
}

template<> AttributeType AttributeImpl<glm::vec2>::Type() const
{
	return ATTR_VECTOR2;
}

template<> AttributeType AttributeImpl<glm::vec3>::Type() const
{
	return ATTR_VECTOR3;
}

template<> AttributeType AttributeImpl<glm::vec4>::Type() const
{
	return ATTR_VECTOR4;
}

template<> AttributeType AttributeImpl<glm::quat>::Type() const
{
	return ATTR_QUATERNION;
}

template<> AttributeType AttributeImpl<ColorF>::Type() const
{
	return ATTR_COLOR;
}

template<> AttributeType AttributeImpl<BoundingAABB>::Type() const
{
	return ATTR_BOUNDINGBOX;
}

template<> AttributeType AttributeImpl<glm::mat3>::Type() const
{
	return ATTR_MATRIX3;
}

template<> AttributeType AttributeImpl<glm::mat3x4>::Type() const
{
	return ATTR_MATRIX3X4;
}

template<> AttributeType AttributeImpl<glm::mat4>::Type() const
{
	return ATTR_MATRIX4;
}

template<> AttributeType AttributeImpl<ResourceRef>::Type() const
{
	return ATTR_RESOURCEREF;
}

template<> AttributeType AttributeImpl<ResourceRefList>::Type() const
{
	return ATTR_RESOURCEREFLIST;
}

template<> AttributeType AttributeImpl<ObjectRef>::Type() const
{
	return ATTR_OBJECTREF;
}

template<> AttributeType AttributeImpl<JSONValue>::Type() const
{
	return ATTR_JSONVALUE;
}
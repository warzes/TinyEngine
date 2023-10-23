#include "stdafx.h"
#include "JSONFile.h"
#include "Core/IO/Stream.h"
#include "Core/Object/AutoPtr.h"
#include "Core/Logging/Log.h"

void JSONFile::RegisterObject()
{
	RegisterFactory<JSONFile>();
}

bool JSONFile::BeginLoad(Stream& source)
{
	size_t dataSize = source.Size() - source.Position();
	AutoArrayPtr<char> buffer(new char[dataSize]);
	if (source.Read(buffer, dataSize) != dataSize)
		return false;

	const char* pos = buffer;
	const char* end = pos + dataSize;

	// Remove any previous content
	root.SetNull();
	/// \todo If fails, log the line number on which the error occurred
	bool success = root.Parse(pos, end);
	if (!success)
	{
		LogError("Parsing JSON from " + source.Name() + " failed; data may be partial");
	}

	return success;
}

bool JSONFile::Save(Stream& dest)
{
	std::string buffer;
	root.ToString(buffer);
	if (buffer.length())
		return dest.Write(&buffer[0], buffer.length()) == buffer.length();
	else
		return true;
}
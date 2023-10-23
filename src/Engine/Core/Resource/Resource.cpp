#include "stdafx.h"
#include "Resource.h"
#include "Core/Logging/Log.h"

bool Resource::BeginLoad(Stream&)
{
	return false;
}

bool Resource::EndLoad()
{
	// Resources that do not need access to main-thread critical objects do not need to override this
	return true;
}

bool Resource::Save(Stream&)
{
	LogError("Save not supported for " + TypeName());
	return false;
}

bool Resource::Load(Stream& source)
{
	bool success = BeginLoad(source);
	if (success)
		success &= EndLoad();

	return success;
}

void Resource::SetName(const std::string& newName)
{
	name = newName;
	nameHash = StringHash(newName);
}
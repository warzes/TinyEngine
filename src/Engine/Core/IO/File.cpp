#include "stdafx.h"
#include "File.h"
#include "FileSystem.h"
//-----------------------------------------------------------------------------
static const char* openModes[] =
{
	"rb",
	"wb",
	"r+b",
	"w+b"
};
//-----------------------------------------------------------------------------
File::File() :
	m_mode(FILE_READ),
	m_handle(nullptr),
	m_readSyncNeeded(false),
	m_writeSyncNeeded(false)
{
}
//-----------------------------------------------------------------------------
File::File(const std::string& fileName, FileMode mode) :
	m_mode(FILE_READ),
	m_handle(nullptr),
	m_readSyncNeeded(false),
	m_writeSyncNeeded(false)
{
	Open(fileName, mode);
}
//-----------------------------------------------------------------------------
File::~File()
{
	Close();
}
//-----------------------------------------------------------------------------
bool File::Open(const std::string& fileName, FileMode fileMode)
{
	Close();

	if (fileName.empty())
		return false;

	errno_t ferr = 0;

	ferr = fopen_s(&m_handle, FileSystem::NativePath(fileName).c_str(), openModes[fileMode]);
	// If file did not exist in readwrite mode, retry with write-update mode
	if (m_mode == FILE_READWRITE && !m_handle)
	{
		ferr = fopen_s(&m_handle, FileSystem::NativePath(fileName).c_str(), openModes[fileMode + 1]);
	}

	if (!m_handle)
		return false;

	m_name = fileName;
	m_mode = fileMode;
	m_position = 0;
	m_readSyncNeeded = false;
	m_writeSyncNeeded = false;

	fseek(m_handle, 0, SEEK_END);
	m_size = ftell(m_handle);
	fseek(m_handle, 0, SEEK_SET);
	return true;
}
//-----------------------------------------------------------------------------
size_t File::Read(void* dest, size_t numBytes)
{
	if (!m_handle || m_mode == FILE_WRITE)
		return 0;

	if (numBytes + m_position > m_size)
		numBytes = m_size - m_position;
	if (!numBytes)
		return 0;

	// Need to reassign the position due to internal buffering when transitioning from writing to reading
	if (m_readSyncNeeded)
	{
		fseek(m_handle, (long)m_position, SEEK_SET);
		m_readSyncNeeded = false;
	}

	size_t ret = fread(dest, numBytes, 1, m_handle);
	if (ret != 1)
	{
		// If error, return to the position where the read began
		fseek(m_handle, (long)m_position, SEEK_SET);
		return 0;
	}

	m_writeSyncNeeded = true;
	m_position += numBytes;
	return numBytes;
}
//-----------------------------------------------------------------------------
size_t File::Seek(size_t newPosition)
{
	if (!m_handle)
		return 0;

	// Allow sparse seeks if writing
	if (m_mode == FILE_READ && newPosition > m_size)
		newPosition = m_size;

	fseek(m_handle, (long)newPosition, SEEK_SET);
	m_position = newPosition;
	m_readSyncNeeded = false;
	m_writeSyncNeeded = false;
	return m_position;
}
//-----------------------------------------------------------------------------
size_t File::Write(const void* data, size_t numBytes)
{
	if (!m_handle || m_mode == FILE_READ)
		return 0;

	if (!numBytes)
		return 0;

	// Need to reassign the position due to internal buffering when transitioning from reading to writing
	if (m_writeSyncNeeded)
	{
		fseek(m_handle, (long)m_position, SEEK_SET);
		m_writeSyncNeeded = false;
	}

	if (fwrite(data, numBytes, 1, m_handle) != 1)
	{
		// If error, return to the position where the write began
		fseek(m_handle, (long)m_position, SEEK_SET);
		return 0;
	}

	m_readSyncNeeded = true;
	m_position += numBytes;
	if (m_position > m_size)
		m_size = m_position;

	return m_size;
}
//-----------------------------------------------------------------------------
bool File::IsReadable() const
{
	return m_handle != 0 && m_mode != FILE_WRITE;
}
//-----------------------------------------------------------------------------
bool File::IsWritable() const
{
	return m_handle != 0 && m_mode != FILE_READ;
}
//-----------------------------------------------------------------------------
void File::Close()
{
	if (m_handle)
	{
		fclose(m_handle);
		m_handle = 0;
		m_position = 0;
		m_size = 0;
	}
}
//-----------------------------------------------------------------------------
void File::Flush()
{
	if (m_handle)
		fflush(m_handle);
}
//-----------------------------------------------------------------------------
bool File::IsOpen() const
{
	return m_handle != 0;
}
//-----------------------------------------------------------------------------
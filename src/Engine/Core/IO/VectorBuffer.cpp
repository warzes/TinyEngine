#include "stdafx.h"
#include "VectorBuffer.h"

VectorBuffer::VectorBuffer()
{
}

VectorBuffer::VectorBuffer(const std::vector<unsigned char>& data)
{
	SetData(data);
}

VectorBuffer::VectorBuffer(const void* data, size_t numBytes)
{
	SetData(data, numBytes);
}

VectorBuffer::VectorBuffer(Stream& source, size_t numBytes)
{
	SetData(source, numBytes);
}

size_t VectorBuffer::Read(void* dest, size_t numBytes)
{
	if (numBytes + m_position > m_size)
		numBytes = m_size - m_position;
	if (!numBytes)
		return 0;

	unsigned char* srcPtr = &buffer[m_position];
	unsigned char* destPtr = (unsigned char*)dest;
	m_position += numBytes;

	size_t copySize = numBytes;
	while (copySize >= sizeof(unsigned))
	{
		*((unsigned*)destPtr) = *((unsigned*)srcPtr);
		srcPtr += sizeof(unsigned);
		destPtr += sizeof(unsigned);
		copySize -= sizeof(unsigned);
	}
	if (copySize & sizeof(unsigned short))
	{
		*((unsigned short*)destPtr) = *((unsigned short*)srcPtr);
		srcPtr += sizeof(unsigned short);
		destPtr += sizeof(unsigned short);
	}
	if (copySize & 1)
		*destPtr = *srcPtr;

	return m_size;
}

size_t VectorBuffer::Seek(size_t newPosition)
{
	if (newPosition > m_size)
		newPosition = m_size;

	m_position = newPosition;
	return m_position;
}

size_t VectorBuffer::Write(const void* data, size_t numBytes)
{
	if (!numBytes)
		return 0;

	// Expand the buffer if necessary
	if (numBytes + m_position > m_size)
	{
		m_size = m_position + numBytes;
		buffer.resize(m_size);
	}

	unsigned char* srcPtr = (unsigned char*)data;
	unsigned char* destPtr = &buffer[m_position];
	m_position += numBytes;

	size_t copySize = numBytes;
	while (copySize >= sizeof(unsigned))
	{
		*((unsigned*)destPtr) = *((unsigned*)srcPtr);
		srcPtr += sizeof(unsigned);
		destPtr += sizeof(unsigned);
		copySize -= sizeof(unsigned);
	}
	if (copySize & sizeof(unsigned short))
	{
		*((unsigned short*)destPtr) = *((unsigned short*)srcPtr);
		srcPtr += sizeof(unsigned short);
		destPtr += sizeof(unsigned short);
	}
	if (copySize & 1)
		*destPtr = *srcPtr;

	return numBytes;
}

bool VectorBuffer::IsReadable() const
{
	return true;
}

bool VectorBuffer::IsWritable() const
{
	return true;
}

void VectorBuffer::SetData(const std::vector<unsigned char>& data)
{
	buffer = data;
	m_position = 0;
	m_size = data.size();
}

void VectorBuffer::SetData(const void* data, size_t numBytes)
{
	if (!data)
		numBytes = 0;

	buffer.resize(numBytes);
	if (numBytes)
		memcpy(&buffer[0], data, numBytes);

	m_position = 0;
	m_size = numBytes;
}

void VectorBuffer::SetData(Stream& source, size_t numBytes)
{
	buffer.resize(numBytes);
	size_t actualSize = source.Read(&buffer[0], numBytes);
	if (actualSize != numBytes)
		buffer.resize(actualSize);

	m_position = 0;
	m_size = actualSize;
}

void VectorBuffer::Clear()
{
	buffer.clear();
	m_position = 0;
	m_size = 0;
}

void VectorBuffer::Resize(size_t newSize)
{
	buffer.resize(newSize);
	m_size = newSize;
	if (m_position > m_size)
		m_position = m_size;
}
#pragma once

// 32-bit case-insensitive hash value for a string.
class StringHash
{
public:
	static const StringHash Zero;

	StringHash() = default;
	StringHash(const StringHash& hash) = default;
	explicit StringHash(unsigned value) : m_value(value) {}
	explicit StringHash(const std::string& str) : m_value(Calculate(str.c_str())) {}
	explicit StringHash(const char* str) : m_value(Calculate(str)) {}
	explicit StringHash(char* str) : m_value(Calculate(str)) {}

	StringHash& operator=(const StringHash&) = default;

	StringHash& operator=(const std::string& rhs)
	{
		m_value = Calculate(rhs.c_str());
		return *this;
	}

	StringHash& operator=(const char* rhs)
	{
		m_value = Calculate(rhs);
		return *this;
	}

	StringHash& operator=(char* rhs)
	{
		m_value = Calculate(rhs);
		return *this;
	}

	StringHash operator+(const StringHash& rhs) const
	{
		StringHash ret;
		ret.m_value = m_value + rhs.m_value;
		return ret;
	}

	StringHash& operator+=(const StringHash& rhs)
	{
		m_value += rhs.m_value;
		return *this;
	}

	bool operator==(const StringHash& rhs) const { return m_value == rhs.m_value; }
	bool operator!=(const StringHash& rhs) const { return m_value != rhs.m_value; }
	bool operator<(const StringHash& rhs) const { return m_value < rhs.m_value; }
	bool operator>(const StringHash& rhs) const { return m_value > rhs.m_value; }

	operator bool() const { return m_value != 0; }
	unsigned Value() const { return m_value; }
	
	std::string ToString() const;
	unsigned ToHash() const { return m_value; }

	static unsigned Calculate(const char* str);

private:
	unsigned m_value = 0;
};
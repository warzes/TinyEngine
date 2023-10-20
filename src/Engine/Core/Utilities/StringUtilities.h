#pragma once

namespace StringUtils
{
	// Count number of elements in a string.
	[[nodiscard]] size_t CountElements(const std::string& string, char separator = ' ');
	// Count number of elements in a string.
	[[nodiscard]] size_t CountElements(const char* string, char separator = ' ');

	// Trim whitespace from a string.
	[[nodiscard]] std::string Trim(const std::string& string);

	// Replace all occurrences of a substring in a string.
	[[nodiscard]] std::string Replace(const std::string& string, const std::string& replaceThis, const std::string& replaceWith);
	// Replace all occurrences of a char in a string.
	[[nodiscard]] std::string Replace(const std::string& string, char replaceThis, char replaceWith);

	// Replace all occurrences of a substring in a string.
	void ReplaceInPlace(std::string& string, const std::string& replaceThis, const std::string& replaceWith);
	// Replace all occurrences of a char in a string.
	void ReplaceInPlace(std::string& string, char replaceThis, char replaceWith);

	// Convert string to uppercase.
	[[nodiscard]] std::string ToUpper(const std::string& string);
	// Convert string to lowercase.
	[[nodiscard]] std::string ToLower(const std::string& string);

	// Check if string starts with another string.
	[[nodiscard]] bool StartsWith(const std::string& string, const std::string& substring);
	// Check if string ends with another string.
	[[nodiscard]] bool EndsWith(const std::string& string, const std::string& substring);

	// Split a string with separator.
	[[nodiscard]] std::vector<std::string> Split(const std::string& string, char separator = ' ');
	// Split a string with separator.
	[[nodiscard]] std::vector<std::string> Split(const char* string, char separator = ' ');

	// Return an index to a string list corresponding to the given string, or a default value if not found. The string list must be empty-terminated.
	size_t ListIndex(const std::string& string, const std::string* strings, size_t defaultIndex);
	// Return an index to a string list corresponding to the given C string, or a default value if not found. The string list must be empty-terminated.
	size_t ListIndex(const char* string, const std::string* strings, size_t defaultIndex);
	// Return an index to a C string list corresponding to the given string, or a default value if not found. The string list must be null-terminated.
	size_t ListIndex(const std::string& string, const char** strings, size_t defaultIndex);
	// Return an index to a C string list corresponding to the given C string, or a default value if not found. The string list must be null-terminated.
	size_t ListIndex(const char* string, const char** strings, size_t defaultIndex);

	bool FromString(glm::vec2& out, const char* string);
	bool FromString(glm::vec3& out, const char* string);
	bool FromString(glm::vec4& out, const char* string);
	bool FromString(glm::ivec2& out, const char* string);
	bool FromString(glm::ivec3& out, const char* string);
	bool FromString(glm::mat3& out, const char* string);
	bool FromString(glm::mat3x4& out, const char* string);
	bool FromString(glm::mat4& out, const char* string);

	std::string ToString(const glm::vec2& in);
	std::string ToString(const glm::vec3& in);
	std::string ToString(const glm::vec4& in);
	std::string ToString(const glm::ivec2& in);
	std::string ToString(const glm::ivec3& in);
	std::string ToString(const glm::mat3& in);
	std::string ToString(const glm::mat3x4& in);
	std::string ToString(const glm::mat4& in);
} // StringUtils
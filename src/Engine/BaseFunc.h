#pragma once

#define SE_CHECK_AND_RETURN(exp, msg)        { if(!(exp)) { Fatal(msg); return; } }
#define SE_CHECK_AND_RETURN_NULL(exp, msg)   { if(!(exp)) { Fatal(msg); return nullptr; } }
#define SE_CHECK_AND_RETURN_VAL(exp, msg, r) { if(!(exp)) { Fatal(msg); return r; } }

template<class T>
inline constexpr void Swap(T& left, T& right) noexcept
{
	T tmp = std::move(left);
	left = std::move(right);
	right = std::move(tmp);
}

// Implementation from "08/02/2015 Better array 'countof' implementation with C++ 11 (updated)" - https://www.g-truc.net/post-0708.html
template<typename T, size_t N>
[[nodiscard]] constexpr size_t Countof(T const (&)[N])
{
	return N;
}
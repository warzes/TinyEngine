#pragma once

class Color final
{
public:
	inline static constexpr auto Black = { 0, 0, 0, 255 };
	inline static constexpr auto Red = { 255, 0, 0, 255 };
	inline static constexpr auto Magenta = { 255, 0, 255, 255 };
	inline static constexpr auto Green = { 0, 255, 0, 255 };
	inline static constexpr auto Cyan = { 0, 255, 255, 255 };
	inline static constexpr auto Blue = { 0, 0, 255, 255 };
	inline static constexpr auto Yellow = { 255, 255, 0, 255 };
	inline static constexpr auto White = { 255, 255, 255, 255 };
	inline static constexpr auto Gray = { 128, 128, 128, 255 };

	constexpr Color() noexcept = default;
	explicit constexpr Color(uint32_t color) noexcept
	{
		r = static_cast<std::uint8_t>((color & 0xFF000000U) >> 24);
		g = static_cast<std::uint8_t>((color & 0x00FF0000U) >> 16);
		b = static_cast<std::uint8_t>((color & 0x0000FF00U) >> 8);
		a = static_cast<std::uint8_t>( color & 0x000000FFU);
	}
	explicit constexpr Color(int red, int green, int blue, int alpha = 0xFFU) noexcept : r((uint8_t)red), g((uint8_t)green), b((uint8_t)blue), a((uint8_t)alpha) {}
	explicit constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFFU) noexcept : r(red), g(green), b(blue), a(alpha) {}
	explicit constexpr Color(float red, float green, float blue, float alpha = 1.0f) noexcept
	{
		r = static_cast<std::uint8_t>(red * 255.0f);
		g = static_cast<std::uint8_t>(green * 255.0f);
		b = static_cast<std::uint8_t>(blue * 255.0f);
		a = static_cast<std::uint8_t>(alpha * 255.0f);
	}

	explicit Color(const glm::vec3& v) noexcept
	{
		r = static_cast<std::uint8_t>(std::round(v[0] * 255.0f));
		g = static_cast<std::uint8_t>(std::round(v[1] * 255.0f));
		b = static_cast<std::uint8_t>(std::round(v[2] * 255.0f));
		a = 0xFFU;
	}

	explicit Color(const glm::vec4& v) noexcept
	{
		r = static_cast<std::uint8_t>(std::round(v[0] * 255.0f));
		g = static_cast<std::uint8_t>(std::round(v[1] * 255.0f));
		b = static_cast<std::uint8_t>(std::round(v[2] * 255.0f));
		a = static_cast<std::uint8_t>(std::round(v[3] * 255.0f));
	}

	Color& operator=(const Color&) = default;

	[[nodiscard]] constexpr glm::vec4 GetNormColor() const noexcept
	{
		return glm::vec4{r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f};
	}

	bool operator==(const Color& c) { return r == c.r && g == c.g && b == c.b && a == c.a; }

	[[nodiscard]] constexpr auto GetNormR() const noexcept { return r / 255.0f; }
	[[nodiscard]] constexpr auto GetNormG() const noexcept { return g / 255.0f; }
	[[nodiscard]] constexpr auto GetNormB() const noexcept { return b / 255.0f; }
	[[nodiscard]] constexpr auto GetNormA() const noexcept { return a / 255.0f; }

	[[nodiscard]] constexpr auto GetIntValue() const noexcept
	{
		return 
			(static_cast<std::uint32_t>(r) << 24) |
			(static_cast<std::uint32_t>(g) << 16) |
			(static_cast<std::uint32_t>(b) << 8)  |
			static_cast<std::uint32_t>(a);
	}

	uint8_t r = 0xFFU;
	uint8_t g = 0xFFU;
	uint8_t b = 0xFFU;
	uint8_t a = 0xFFU;
};


// TODO: REFACT ==>

#define RGBX(rgb,x)   ( ((rgb)&0xFFFFFF) | (((unsigned)(x))<<24) )
#define RGB3(r,g,b)   ( ((r)<<16u) | ((g)<<8u) | (b) )
#define RGB4(r,g,b,a) RGBX(RGB3(r,g,b),a)

#define BLACK   RGBX(0x000000,255u)
#define WHITE   RGBX(0xFFF1E8,255u)
#define RED     RGB3(   255u,48u,48u )
#define GREEN   RGB3(  144u,255u,48u )
#define BLUE    RGBX(0x065AB5,255u)
#define CYAN    RGB3(   0u,192u,255u )
#define ORANGE  RGB3(  255u,144u,48u )
#define PURPLE  RGB3( 178u,128u,255u )
#define YELLOW  RGB3(   255u,224u,0u )
#define GRAY    RGB3( 149u,149u,149u )
#define PINK    RGB3(  255u,48u,144u )
#define AQUA    RGB3(  48u,255u,144u )

inline constexpr glm::vec3 RGBToVec(unsigned rgb)
{
	return { (float)((rgb >> 16) & 255) / 255.f, (float)((rgb >> 8) & 255) / 255.f, (float)((rgb >> 0) & 255) / 255.f };
}

class ColorF
{
public:
	bool operator==(const ColorF& rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }
	bool operator!=(const ColorF& rhs) const { return !(*this == rhs); }

	float r;
	float g;
	float b;
	float a;
};

namespace StringUtils
{
	bool FromString(ColorF& out, const char* string);
	std::string ToString(const ColorF& in);
}
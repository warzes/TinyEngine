#pragma once

class Color
{
public:
	enum
	{
		RED = 0xff0000ff,
		GREEN = 0xff00ff00,
		BLUE = 0xffff0000,
		BLACK = 0xff000000,
		WHITE = 0xffFFffFF
	};

	Color() {}
	Color(uint32_t abgr)
	{
		r = uint8_t(abgr & 0xff);
		g = uint8_t((abgr >> 8) & 0xff);
		b = uint8_t((abgr >> 16) & 0xff);
		a = uint8_t((abgr >> 24) & 0xff);
	}

	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

	uint32_t abgr() const { return ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r; }

	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

#define RGBX(rgb,x)   ( ((rgb)&0xFFFFFF) | (((unsigned)(x))<<24) )
#define RGB3(r,g,b)   ( ((r)<<16) | ((g)<<8) | (b) )
#define RGB4(r,g,b,a) RGBX(RGB3(r,g,b),a)

#define BLACK   RGBX(0x000000,255)
#define WHITE   RGBX(0xFFF1E8,255)
#define RED     RGB3(   255,48,48 )
#define GREEN   RGB3(  144,255,48 )
#define BLUE    RGBX(0x065AB5,255)
#define CYAN    RGB3(   0,192,255 )
#define ORANGE  RGB3(  255,144,48 )
#define PURPLE  RGB3( 178,128,255 )
#define YELLOW  RGB3(   255,224,0 )
#define GRAY    RGB3( 149,149,149 )
#define PINK    RGB3(  255,48,144 )
#define AQUA    RGB3(  48,255,144 )

inline constexpr glm::vec3 RGBToVec(unsigned rgb)
{
	return { (float)((rgb >> 16) & 255) / 255.f, (float)((rgb >> 8) & 255) / 255.f, (float)((rgb >> 0) & 255) / 255.f };
}
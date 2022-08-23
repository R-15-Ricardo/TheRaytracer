#pragma once

#include <string>
#include <fstream>
#include <cmath>
#include <png++/png.hpp>

class Color {
	public:
		double r;
		double g;
		double b;

		Color();
		Color(double _r, double _g, double _b);

		uint8_t getPixelLevel(const char channel);
		double brightness();

		Color operator+ (const Color& other) const;
		Color operator* (const Color& other) const;
		Color operator| (const Color& other) const;

		void clip ();

		template <class T>
		friend Color operator*(T scalar, const Color& color);

	private:
		//noting... I guess...
};

template<class T>
Color operator*(T scalar, const Color &color)
{
	return Color(scalar * color.r, scalar * color.g, scalar * color.b);
}

// TODO: remove png::image class and optimize for Color**
class Texture {
	public:
		Texture();
		Texture(const std::string texturePath);
		//~Texture()
		int getWidth() {return this->width;}
		int getHeight() {return this->height;}

		Color getColor(unsigned int x, unsigned int y);

	private:
		int width;
		int height;
		Color* textureColorBuffer;
};


class Material {
	public:
		Material ();
		Material (Color bCol, double refl, double gl);
		Material (Texture imTex, double refl, double gl);
		Material (Color bCol, double rIndex);

		bool hasTexture() {return this->textureFlag;};
		Color baseColor;
		Texture texture;

		double indexOfRefraction;
		double reflectiveness;
		double gloss;

	private:
		bool textureFlag;
};


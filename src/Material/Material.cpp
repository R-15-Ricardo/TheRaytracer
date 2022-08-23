#include "Material.hpp"

#include <png++/image.hpp>
// ------------------------- Color class definitions -------------------------
Color::Color() : r(1.0), g(1.0), b(1.0) {}
Color::Color(double _r, double _g, double _b) : r(_r), g(_g), b(_b) {}

uint8_t Color::getPixelLevel(const char channel)
{
	switch (channel) 
	{
		case 'r':
			return static_cast<uint8_t>(std::floor(this->r * 255.0));
		case 'g':
			return static_cast<uint8_t>(std::floor(this->g * 255.0));
		case 'b':
			return static_cast<uint8_t>(std::floor(this->b * 255.0));
	}
	return 0;
}

double Color::brightness()
{
	return (this->r + this->g + this-> b)/3.0;
}

Color Color::operator+(const Color &other) const
{
	return Color(this->r + other.r, this->g + other.g, this->b + other.b);
}

Color Color::operator*(const Color &other) const
{
	return Color(this->r * other.r, this->g * other.g, this->b * other.b);
}

Color Color::operator|(const Color &other) const
{
	return Color((this->r + other.r)/2.0, (this->g + other.g)/2.0, (this->b + other.b)/2.0);
}

void Color::clip ()
{
	double alllight = this->r + this->g + this->b;
	double excesslight = alllight - 3;
	if (excesslight > 0)
	{
		this->r = this->r + excesslight*(this->r/alllight);
		this->g = this->g + excesslight*(this->g/alllight);
		this->b = this->b + excesslight*(this->b/alllight);
	}

	if (this->r > 1) {this->r = 1;}
	if (this->g > 1) {this->g = 1;}
	if (this->b > 1) {this->b = 1;}

	if (this->r < 0) {this->r = 0;}
	if (this->g < 0) {this->g = 0;}
	if (this->b < 0) {this->b = 0;}
}

// ------------------------- Texture class definitions -------------------------
Texture::Texture(){}
Texture::Texture(const std::string texturePath)
{
	png::image<png::rgb_pixel> imgTemp(texturePath);
	this->width = imgTemp.get_width();
	this->height = imgTemp.get_height();

	this->textureColorBuffer = new Color [width*height];
	for (int y = 0; y < this->height; y++)
	{
		for (int x = 0; x < this->width; x++)
		{
			int bufferIndex = y*width + x;
			png::rgb_pixel fetchedPixel = imgTemp.get_pixel(x,y);

			this->textureColorBuffer[bufferIndex].r = (double)fetchedPixel.red/255.0;
			this->textureColorBuffer[bufferIndex].g = (double)fetchedPixel.green/255.0;
			this->textureColorBuffer[bufferIndex].b = (double)fetchedPixel.blue/255.0;
		}
	}
}

Color Texture::getColor(unsigned int x, unsigned int y)
{
	y = this->height - y - 1;
	return this->textureColorBuffer[y*this->width + x];
}

// ------------------------- Material class definitions -------------------------
Material::Material () : baseColor(Color(1.0,1.0,1.0)), textureFlag(false), reflectiveness(0.0), gloss(0.0), indexOfRefraction(1.0) {}
Material::Material (Color bCol, double refl, double gl) : baseColor(bCol), reflectiveness(refl), gloss(gl)
{
	this->indexOfRefraction = 1.0;
	this->textureFlag = false;
}
Material::Material (Texture imTex, double refl, double gl) : baseColor(Color(0.0,0.0,0.0)), texture(imTex), reflectiveness(refl), gloss(gl)
{
	this->indexOfRefraction = 1.0;
	this->textureFlag = true;
}

Material::Material (Color bCol, double rIndex) : baseColor(bCol), indexOfRefraction(rIndex)
{
	this->texture = Texture();

	this->reflectiveness = 0.0;
	this->gloss = 0.0;

	this->textureFlag = false;
}


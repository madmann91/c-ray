//
//  texture.h
//  C-ray
//
//  Created by Valtteri on 09/04/2019.
//  Copyright © 2015-2020 Valtteri Koskivuori. All rights reserved.
//

#include <stdbool.h>

enum colorspace {
	linear,
	sRGB
};

enum precision {
	char_p,
	float_p,
	none
};

struct texture {
	bool hasAlpha;
	enum colorspace colorspace;
	enum precision precision;
	union {
		unsigned char *byte_p; //For 24/32bit
		float *float_p; //For hdr
	} data;
	int channels;
	unsigned width;
	unsigned height;
};

struct color;

struct texture *newTexture(enum precision p, unsigned width, unsigned height, int channels);

void setPixel(struct texture *t, struct color c, unsigned int x, unsigned int y);

/// Get the color of a pixel from a given texture
/// @param t Texture to retrieve color from
/// @param x X coordinate of pixel
/// @param y Y coordinate of pixel
struct color textureGetPixel(const struct texture *t, unsigned x, unsigned y);
struct color textureGetPixelFiltered(const struct texture *t, float x, float y);

/// Convert texture from sRGB to linear color space
/// @remarks The texture data will be modified directly.
/// @param t Texture to convert
void textureFromSRGB(struct texture *t);

/// Convert texture from linear color space to sRGB.
/// @remarks The texture data will be modified directly.
/// @param t Texture to convert
void textureToSRGB(struct texture *t);

/// Deallocate a given texture
/// @param tex Texture to deallocate
void destroyTexture(struct texture *tex);

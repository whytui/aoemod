
#pragma once

#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <memory.h>

namespace _BITMAP {


class BitmapExporter {

private:
	// Generates a BMP file using data (=pixel values) from dataArray
	// file must be opened/closed by caller : here it must be an actively opened file with write authorization.
	// pixelsX/pixelsY must be even numbers.
	// bitsPerPixel can be 4, 8, 16, 24 (8 recommended)
	// palette is allowed to be NULL or incomplete: it is only used to "overwrite" the default palette. if "palette" is incomplete, default colors will be used for missing entries.
	static bool GenerateBitmap(FILE *file, long int pixelsX, long int pixelsY, long int bitsPerPixel, const unsigned char *dataArray,
		const unsigned char *palette, long int paletteSizeInBytes);

public:

// sizeX and sizeY = size of dataArray, and size of exported image.
// dataArray: values must be from left to right then top to bottom. Other types than 1-byte are accepted but values must be 0-255 (after adding minValue).
// minValue = lowest value in dataArray. Used so bitmap's pixel values always start at 0 (important for matching palette colors)
// palette (nullable) = palette to use, does not require to contain 256 colors. Each color is 4 bytes: Blue/Green/Red/zero. First color=lowest value's color.
// paletteSizeInBytes = exact size in bytes of palette memory (4*number of RGB colors)
template<typename T> static bool ExportDataAsBitmapUsingPalette(const char *filename, long int sizeX, long int sizeY, const T *dataArray,
	int minValue, const unsigned char *palette, long int paletteSizeInBytes) {
	assert((sizeof(T) == 1) || (sizeof(T) == 2) || (sizeof(T) == 4));
	if (!filename) { return false; }
	FILE *file;
	errno_t e = fopen_s(&file, filename, "wb+"); // overwrite
	if (e) {
		return false;
	}
	
	// Copy input buffer to a correctly-ordered buffer, using easy-to-use values (0-4)
	unsigned char *buffer = (unsigned char*)malloc(sizeX * sizeY);
	for (int x = 0; x < sizeX; x++) {
		for (int y = sizeY - 1; y >= 0; y--) {
			int offset = (y * sizeX) + x;
			buffer[offset] = ((unsigned char)(dataArray[offset] - minValue)); // so values start at 0
		}
	}

	int pixelsX = sizeX;
	int pixelsY = sizeY;
	int imgBytesSize = pixelsX * pixelsY;
	GenerateBitmap(file, pixelsX, pixelsY, 8, buffer, palette, paletteSizeInBytes);
	free(buffer);
	fclose(file);
	return true;
}


// sizeX and sizeY = size of dataArray, and size of exported image.
// dataArray: values must be from left to right then top to bottom. Other types than 1-byte are accepted but values must be 0-255 (after adding minValue).
// minValue = minimal existing value in dataArray (maxValue = maximal value)
// If maxValue-minValue is low enough, values are multiplied to distribute them in [0-255] interval, which produces better shades of grey rendering.
template<typename T> static bool ExportDataAsBitmapGreyShades(const char *filename, long int sizeX, long int sizeY, const T *dataArray,
	int minValue, int maxValue) {
	static_assert((sizeof(T) == 1) || (sizeof(T) == 2) || (sizeof(T) == 4), "ExportDataAsBitmapGreyShades: T must be a BYTE, WORD or DWORD numeric type");
	if (!filename) { return false; }
	if (maxValue < minValue) { return false; }
	FILE *file;
	errno_t e = fopen_s(&file, filename, "wb+"); // overwrite
	if (e) {
		return false;
	}

	int range = maxValue - minValue + 1;
	if (range >= 128) {
		range = 256;
	}
	int valueToAdd = 0;
	if (minValue < 0) {
		valueToAdd = -minValue;
	}
	assert(range > 0);
	int factor = 256 / range;

	// Copy input buffer to a correctly-ordered buffer, using easy-to-use values (0-4)
	unsigned char *buffer = (unsigned char*)malloc(sizeX * sizeY);
	for (int x = 0; x < sizeX; x++) {
		for (int y = sizeY - 1; y >= 0; y--) {
			int offset = (y * sizeX) + x;
			buffer[offset] = ((unsigned char)(dataArray[offset] + valueToAdd)) * factor; // get a repartition of values 0-255 and exploit all shades of grey
		}
	}

	int pixelsX = sizeX;
	int pixelsY = sizeY;
	int imgBytesSize = pixelsX * pixelsY;
	GenerateBitmap(file, pixelsX, pixelsY, 8, buffer, NULL, 0);
	free(buffer);
	fclose(file);
	return true;
}


};

}


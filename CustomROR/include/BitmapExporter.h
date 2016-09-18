
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
	// bitsPerPixel can be 4, 8, 16, 24 (8 recommended = tested value)
	// palette is allowed to be NULL or incomplete: it is only used to "overwrite" the default palette. if "palette" is incomplete, default colors will be used for missing entries.
	// dataArray MUST contain values in following order: line (left to right) then column (bottom to top - WARNING !)
	// This method supports any size (X/Y) and takes care of BMP format trick for X sizes that are not a multiple of 4.
	static bool GenerateBitmap(FILE *file, long int pixelsX, long int pixelsY, long int bitsPerPixel, const unsigned char *dataArray,
		const unsigned char *palette, long int paletteSizeInBytes);

public:

// sizeX and sizeY = size of dataArray, and size of exported image.
// dataArray: values must be from left to right then top to bottom. Other types than 1-byte are accepted but values must be 0-255 (after adding minValue).
// dataArray X/Y sizes must match exactly sizeX/sizeY parameters !
// minValue = lowest value in dataArray. Used so bitmap's pixel values always start at 0 (important for matching palette colors)
// palette (nullable) = palette to use, does not require to contain 256 colors. Each color is 4 bytes: Blue/Green/Red/zero. First color=lowest value's color.
// paletteSizeInBytes = exact size in bytes of palette memory (4*number of RGB colors)
// If reverseAxes is true, the function writes map data to a buffer with an inversion of X/Y axes. To be used when provided array's first index is X (Y values are consecutive in memory)
template<typename T> static bool ExportDataAsBitmapUsingPalette(const char *filename, long int sizeX, long int sizeY,
	const T *dataArray, int minValue, const unsigned char *palette, long int paletteSizeInBytes, bool reverseAxes) {
	static_assert((sizeof(T) == 1) || (sizeof(T) == 2) || (sizeof(T) == 4), "ExportDataAsBitmapUsingPalette: T must be a BYTE, WORD or DWORD numeric type");
	if (!filename) { return false; }
	FILE *file;
	errno_t e = fopen_s(&file, filename, "wb+"); // overwrite
	if (e) {
		return false;
	}
	
	// Copy input buffer to a correctly-ordered buffer, using easy-to-use values (0-4)
	unsigned char *buffer = (unsigned char*)malloc(sizeX * sizeY);
	for (int x = 0; x < sizeX; x++) {
		for (int y = 0; y < sizeY - 1; y++) {
			int offsetBmp = (y * sizeX) + x;
			int offsetSrc = offsetBmp; // if provided buffer's organization is the expected one: in memory, values are organized like (x0y0 x1y0 x2y0... x0y1 x1y1 x2y1... x0y2 x1y2 x2y2...)
			if (reverseAxes) {
				// Provided buffer is accessed like buffer[x][y] : in memory, values are organized like (x0y0 x0y1 x0y2... x1y0 x1y1 x1y2... x2y0 x2y1 x2y2...)
				offsetSrc = (x * sizeY) + y;
			}
			buffer[offset] = ((unsigned char)(dataArray[offset] - minValue)); // so values start at 0
		}
	}

	int pixelsX = sizeX;
	int pixelsY = sizeY;
	int imgBytesSize = pixelsX * pixelsY;
	GenerateBitmap(file, pixelsX, pixelsY, 8, buffer, palette, paletteSizeInBytes);
	free(buffer); // free the temporary memory we used
	fclose(file);
	return true;
}


// sizeX and sizeY = size of exported image. Must <= actual data array size (this method supports a partial export of data array)
// dataColumnPointers: array of (sizeX) pointers to positions in data array of each beginning of data column.
// In data array, values must be from left to right then top to bottom. Other types than 1-byte are accepted but values must be 0-255 (after adding minValue).
// minValue = lowest value in dataArray. Used so bitmap's pixel values always start at 0 (important for matching palette colors)
// palette (nullable) = palette to use, does not require to contain 256 colors. Each color is 4 bytes: Blue/Green/Red/zero. First color=lowest value's color.
// paletteSizeInBytes = exact size in bytes of palette memory (4*number of RGB colors)
template<typename T> static bool ExportDataColumnsAsBitmapUsingPalette(const char *filename, long int sizeX, long int sizeY,
	T **dataColumnPointers, int minValue, const unsigned char *palette, long int paletteSizeInBytes) {
	assert(paletteSizeInBytes % 4 == 0);
	static_assert((sizeof(T) == 1) || (sizeof(T) == 2) || (sizeof(T) == 4), "ExportDataColumnsAsBitmapUsingPalette: T must be a BYTE, WORD or DWORD numeric type");
	if (!filename) { return false; }
	FILE *file;
	errno_t e = fopen_s(&file, filename, "wb+"); // overwrite
	if (e) {
		return false;
	}

	// Copy input buffer to a correctly-ordered buffer, using easy-to-use values (0-4)
	unsigned char *buffer = (unsigned char*)malloc(sizeX * sizeY);
	// Provided buffer's organization is the expected one: in memory, values are organized like (x0y0 x1y0 x2y0... x0y1 x1y1 x2y1... x0y2 x1y2 x2y2...)
	// Note that increasing y values go from bottom to top, which IS expected behaviour
	for (int x = 0; x < sizeX; x++) {
		for (int y = 0; y < sizeY - 1; y++) {
			int offset = (y * sizeX) + x;
			const T *srcData = dataColumnPointers[x];
			buffer[offset] = ((unsigned char)(srcData[y] - minValue)); // so values start at 0
		}
	}
	
	int pixelsX = sizeX;
	int pixelsY = sizeY;
	int imgBytesSize = pixelsX * pixelsY;
	GenerateBitmap(file, pixelsX, pixelsY, 8, buffer, palette, paletteSizeInBytes);
	free(buffer); // free the temporary memory we used
	fclose(file);
	return true;
}



// sizeX and sizeY = size of dataArray, and size of exported image.
// dataArray: values must be from left to right then top to bottom. Other types than 1-byte are accepted but values must be 0-255 (after adding minValue).
// dataArray X/Y sizes must match exactly sizeX/sizeY parameters !
// minValue = minimal existing value in dataArray (maxValue = maximal value)
// If maxValue-minValue is low enough, values are multiplied to distribute them in [0-255] interval, which produces better shades of grey rendering.
// If reverseAxes is true, the function writes map data to a buffer with an inversion of X/Y axes. To be used when provided array's first index is X (Y values are consecutive in memory)
template<typename T> static bool ExportDataAsBitmapGreyShades(const char *filename, long int sizeX, long int sizeY,
	const T *dataArray, int minValue, int maxValue, bool reverseAxes) {
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
		for (int y = 0; y < sizeY - 1; y++) {
			int offsetSrc = (y * sizeX) + x; // If provided buffer's organization is the expected one: in memory, values are organized like (x0y0 x1y0 x2y0... x0y1 x1y1 x2y1... x0y2 x1y2 x2y2...)
			if (reverseAxes) {
				// Provided buffer is accessed like buffer[x][y] : in memory, values are organized like (x0y0 x0y1 x0y2... x1y0 x1y1 x1y2... x2y0 x2y1 x2y2...)
				offsetSrc = (x * sizeY) + y;
			}
			int offsetDest = (y * sizeX) + x;
			buffer[offsetDest] = ((unsigned char)(dataArray[offsetSrc] + valueToAdd)) * factor; // get a repartition of values 0-255 and exploit all shades of grey
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


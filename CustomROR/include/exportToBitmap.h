
#pragma once

#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <memory.h>

namespace _BITMAP {


// Generates a BMP file using data (=pixel values) from dataArray
// pixelsX/pixelsY must be even numbers.
// palette is allowed to be NULL or incomplete.
static bool GenerateBitmap(FILE *file, long int pixelsX, long int pixelsY, long int bitsPerPixel, const unsigned char *dataArray,
	const unsigned char *palette, long int paletteSizeInBytes) {
	assert(dataArray != NULL);
	assert(bitsPerPixel > 0);
	assert(bitsPerPixel <= 24);
	if ((dataArray == NULL) || (bitsPerPixel <= 0) || (bitsPerPixel > 24)) { return false; }
	long int filePaletteSize = 0;
	switch (bitsPerPixel) {
	case 4:
		filePaletteSize = 0x80;
		break;
	case 8:
		filePaletteSize = 0x400;
		break;
	}
	long int bmpInfoSize = 0x36 - 0xE;
	long int imgBytesSize = (pixelsX * pixelsY * bitsPerPixel) / 8;
	long int fileSize = 0x36 + imgBytesSize;
	unsigned char headerAndBmpInfo[0x36] = { 'B', 'M', /*2*/0, 0, 0, 0, /*6*/ 0, 0, /*8*/0, 0, /*A img offset*/ 0, 0, 0, 0,
		/*E size*/00, 00, 00, 00, /*12 pxlx*/ 0, 00, 00, 00, /*pxly*/0, 00, 00, 00, /*1A*/ 1, 0,
		/*1C bits/pixel*/(unsigned char)bitsPerPixel, 0, /*compression*/0, 0, 0, 0, /*22 imgsize*/0, 0, 0, 0, /*26*/0, 0, 0, 0, /*2A*/0, 0, 0, 0,
		/*2E*/0, 0, 0, 0, /*32*/0, 0, 0, 0 };
	long int *p = (long int*)(headerAndBmpInfo + 0x12); // x pixel count
	*p = pixelsX;
	p = (long int*)(headerAndBmpInfo + 0x16); // y pixel count
	*p = pixelsY;
	p = (long int*)(headerAndBmpInfo + 0xE); // bmp info size (0x28)
	*p = bmpInfoSize;
	p = (long int*)(headerAndBmpInfo + 0x2);
	*p = fileSize;
	p = (long int*)(headerAndBmpInfo + 0xA); // image offset
	*p = 0x36;
	p = (long int*)(headerAndBmpInfo + 0x22);
	*p = imgBytesSize;
	char *defaultPalette = NULL;
	if (filePaletteSize > 0) {
		defaultPalette = (char*)malloc(filePaletteSize);
		memset(defaultPalette, 0, filePaletteSize);
		for (int i = 0; i < 256; i++) {
			defaultPalette[i * 4] = i; // Blue
			defaultPalette[i * 4 + 1] = i; // Green
			defaultPalette[i * 4 + 2] = i; // Red
			// +3 : remains zero
		}
		if (palette && (paletteSizeInBytes > 0)) {
			assert(paletteSizeInBytes <= filePaletteSize);
			if (paletteSizeInBytes > filePaletteSize) {
				paletteSizeInBytes = filePaletteSize;
			}
			memcpy_s(defaultPalette, filePaletteSize, palette, paletteSizeInBytes);
		}
	}
	fwrite(headerAndBmpInfo, 0x36, sizeof(unsigned char), file);
	if (filePaletteSize > 0) {
		assert(defaultPalette != NULL);
		fwrite(defaultPalette, filePaletteSize, sizeof(unsigned char), file);
	}
	if (defaultPalette) {
		free(defaultPalette);
	}
	fwrite(dataArray, imgBytesSize, sizeof(unsigned char), file);
	return true;
}


// sizeX and sizeY = size of dataArray, and size of exported image.
// dataArray: values must be from left to right then top to bottom. Other types than 1-byte are accepted but values must be 0-255 (after adding minValue).
// minValue = lowest value in dataArray. Used so bitmap's pixel values always start at 0 (important for matching palette colors)
// palette (nullable) = palette to use, does not require to contain 256 colors. Each color is 4 bytes: Blue/Green/Red/zero. First color=lowest value's color.
// paletteSizeInBytes = exact size in bytes of palette memory (4*number of RGB colors)
template<typename T> bool ExportDataAsBitmapUsingPalette(const char *filename, long int sizeX, long int sizeY, const T *dataArray,
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
template<typename T> bool ExportDataAsBitmapGreyShades(const char *filename, long int sizeX, long int sizeY, const T *dataArray,
	int minValue, int maxValue) {
	assert((sizeof(T) == 1) || (sizeof(T) == 2) || (sizeof(T) == 4));
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



}


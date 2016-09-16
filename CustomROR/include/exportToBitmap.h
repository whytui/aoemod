
#pragma once

#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <memory.h>

namespace _BITMAP {


static bool GenerateBitmap(FILE *file, long int pixelsX, long int pixelsY, long int bitsPerPixel, const char *dataArray) {
	assert(dataArray != NULL);
	assert(bitsPerPixel > 0);
	assert(bitsPerPixel <= 24);
	if ((dataArray == NULL) || (bitsPerPixel <= 0) || (bitsPerPixel > 24)) { return false; }
	long int paletteSize = 0;
	switch (bitsPerPixel) {
	case 4:
		paletteSize = 0x80;
		break;
	case 8:
		paletteSize = 0x400;
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
	char *palette = NULL;
	if (paletteSize > 0) {
		palette = (char*)malloc(paletteSize);
		memset(palette, 0, paletteSize);
		for (int i = 0; i < 256; i++) {
			palette[i * 4] = i; // Blue
			palette[i * 4 + 1] = i; // Green
			palette[i * 4 + 2] = i; // Red
			// +3 : remains zero
		}
	}
	fwrite(headerAndBmpInfo, 0x36, sizeof(unsigned char), file);
	if (paletteSize > 0) {
		assert(palette != NULL);
		fwrite(palette, paletteSize, sizeof(unsigned char), file);
	}
	if (palette) {
		free(palette);
	}
	fwrite(dataArray, imgBytesSize, sizeof(unsigned char), file);
	return true;
}


// Dataarray: values must be from left to right then top to bottom
template<typename T> bool ExportDataAsBitmap(const char *filename, long int sizeX, long int sizeY, const T *dataArray,
	int minValue, int maxValue) {
	assert((sizeof(T) == 1) || (sizeof(T) == 2) || (sizeof(T) == 4));
	if (!filename) { return false; }
	if (maxValue < minValue) { return false; }
	FILE *file;
	errno_t e = fopen_s(&file, filename, "wb+");
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
	
	unsigned char *buffer = (unsigned char*)malloc(sizeX * sizeY);
	for (int x = 0; x < sizeX; x++) {
		for (int y = sizeY - 1; y >= 0; y--) {
			int offset = (y * sizeX) + x;
			buffer[offset] = ((unsigned char)(dataArray[offset] + valueToAdd)) * factor;
		}
	}

	int pixelsX = sizeX;
	int pixelsY = sizeY;
	int imgBytesSize = pixelsX * pixelsY;
	GenerateBitmap(file, pixelsX, pixelsY, 8, (const char*) buffer);
	free(buffer);
	fclose(file);
	return true;
}


// TO remove
static bool ExportDataAsBitmap(const char *filename) {
	if (!filename) { return false; }
	FILE *file;
	errno_t e = fopen_s(&file, filename, "wb+");
	if (e) {
		return false;
	}

	int pixelsX = 32;
	int pixelsY = 32;
	int imgBytesSize = pixelsX * pixelsY;
	char *buf1 = (char*)malloc(imgBytesSize);
	memset(buf1, 0, imgBytesSize);
	for (int x = 0; x < pixelsX; x++) {
		for (int y = pixelsY - 1; y >= 0; y--) {
			int offset = (y * pixelsX) + x;
			buf1[offset] = (char)offset;
			//buf[offset] = (char)x * 8;
		}
	}
	GenerateBitmap(file, pixelsX, pixelsY, 8, buf1);
	free(buf1);
	fclose(file);
	return true;
}

}


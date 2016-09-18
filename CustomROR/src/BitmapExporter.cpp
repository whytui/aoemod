#include "../include/BitmapExporter.h"

namespace _BITMAP {

// Generates a BMP file using data (=pixel values) from dataArray
// file must be opened/closed by caller : here it must be an actively opened file with write authorization.
// bitsPerPixel can be 4, 8, 16, 24 (8 recommended = tested value)
// palette is allowed to be NULL or incomplete: it is only used to "overwrite" the default palette. if "palette" is incomplete, default colors will be used for missing entries.
// dataArray MUST contain values in following order: line (left to right) then column (bottom to top)
// This method supports any size (X/Y) and takes care of BMP format trick for X sizes that are not a multiple of 4.
bool BitmapExporter::GenerateBitmap(FILE *file, long int pixelsX, long int pixelsY, long int bitsPerPixel,
	const unsigned char *dataArray, const unsigned char *palette, long int paletteSizeInBytes) {
	assert(file != NULL);
	assert(dataArray != NULL);
	assert(bitsPerPixel > 0);
	assert(bitsPerPixel <= 24);
	// BMP format: when X pixel count is not a multiple of 4, we need to add bytes so each data row in BMP image data is a multiple of 4.
	bool needFinalBytesAfterEachRow = (pixelsX % 4 != 0);
	int bmpSizeX = pixelsX; // Can differ from pixelsX
	int bmpSizeY = pixelsY; // Always = pixelsY
	if (needFinalBytesAfterEachRow) {
		bmpSizeX += (4 - bmpSizeX % 4);
	}
	if ((file == NULL) || (dataArray == NULL) || (bitsPerPixel <= 0) || (bitsPerPixel > 24)) { return false; }
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
	long int imgBytesSize = (bmpSizeX * bmpSizeY * bitsPerPixel) / 8;
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
	*p = 0x36 + filePaletteSize;
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
			// +3 : remains zero (fourth byte)
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
	if (!needFinalBytesAfterEachRow) {
		// Simple case: copy the image buffer as is, when x is a multiple of 4. The fastest case !
		fwrite(dataArray, imgBytesSize, sizeof(unsigned char), file);
	} else {
		// Image buffer is not BMP-compatible. Slower because we need to insert end-of-line bytes.
		const unsigned char *curPos = dataArray;
		unsigned char additionalBytes[4] = { 0, 0, 0, 0 };
		assert((bmpSizeX - pixelsX >= 0) && (bmpSizeX - pixelsX < 4));
		additionalBytes[bmpSizeX - pixelsX] = 0xFF; // Last byte must be 0xFF (previous "added" ones must be 0)
		for (int curLine = 0; curLine < pixelsY; curLine++) {
			fwrite(curPos, pixelsX, sizeof(unsigned char), file); // copy current row.
			curPos += pixelsX; // go to next line in buffer
			// Add specific bytes to complete BMP format's line
			fwrite(additionalBytes, bmpSizeX - pixelsX, sizeof(unsigned char), file); // add end-of-line bytes
		}
	}
	return true;
}

}


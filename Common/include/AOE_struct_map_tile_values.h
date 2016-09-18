
#pragma once

/*
* This file contains empiresX.exe structures definition
* Please read README.txt first.
* Reading/using information from this file implies sharing with the community
* ALL your findings about AOE/ROR structures
* Please share knowledge for better modding experience !
*/
namespace AOE_STRUCTURES {

	static const unsigned char CST_MAP_BUILD_LIKE_DISABLED = (unsigned char)0xFF;
	static const unsigned char CST_MAP_BUILD_LIKE_MAX_VALUE = (unsigned char)CST_MAP_BUILD_LIKE_DISABLED - 1;

#define CHECKSUM_MAP_TILE_VALUES 0x005443C8
	// One is included in 0x7C04A0 address, other in each infAI (stores exploration info)
	// Size= 0x28. Constructor = 0x43D420 mapInfo.constructor(sizeY, sizeX, defaultValue)
	// Describes a structure that stores information for currently-AI-managed player's build position priorities (for a given building to be constructed).
	// Other usages: in infAI: explored map info. -1=not explored, 0=explored, 1= ? 2=to re-explore??
	// Note about coordinates: (posX,posY)=(i,j) is the tile where i<x<i+1 and i<y<i+1
	// So coordinates go from 0 to ("size"-1).
	class STRUCT_MAP_TILE_VALUES {
	public:
		unsigned long int checksum; // C8 43 54 00
		long int arraySizeY; // +04. This can differ from actual map size !
		long int arraySizeX; // +08. This can differ from actual map size !
		long int startPosY; // +0C. Always 0 in standard game. Value to substract before using indexes.
		// 0x10
		long int startPosX; // +10. Always 0 in standard game. Value to substract before using indexes.
		// mapLikeValuesMemoryZone[arraySizeY * (x) + (y)] should be the same as ptrColsPtr[X][Y]. NOT ALWAYS TRUE. Strange conception. For perf maybe ?
		// Please use supplied methods if possible.
	private:
		unsigned char *mapLikeValuesMemoryZone; // +14. Pointer to the beginning of map values memory zone. Do not use directly. Size is this->arraySizeX*this->arraySizeY (bytes).
	public:
		unsigned char **ptrColsPtr; // +18. The list of pointers to each row (X dimension) in mapLikeValues. ptrColsPtr[X]=mapData+(X*mapsizeY) is a pointer to the column X.
		long int unknown_1C; // +1C. Not always initialized !
		long int unknown_matchCount; // +20. Not always initialized !
		unsigned char maxValue; // +24. Not always initialized !. 0xFE for build "like" values (always ?). FF is a NO, 0->0xFE are "like values"
		char unknown_25[3];

		bool IsCheckSumValid() const { return this->checksum == CHECKSUM_MAP_TILE_VALUES; }

		// Easy-to-use methods

		// Returns true if position is valid regarding this object's allocated array.
		// Position is not guaranteed to be valid regarding actual map size !
		inline bool IsPositionValid(long int posX, long int posY) const {
			return ((posX >= 0) && (posX < this->arraySizeX) &&
				(posY >= 0) && (posY < this->arraySizeY)
				);
		};
		bool FixPositionToGetInMapBounds(long int *posX, long int *posY) {
			if (*posX < 0) { *posX = 0; }
			if (*posY < 0) { *posY = 0; }
			if (*posX >= this->arraySizeX) { *posX = this->arraySizeX - 1; }
			if (*posY >= this->arraySizeX) { *posY = this->arraySizeX - 1; }
		}
		// Will not fail/crash if position is out of bounds
		unsigned char GetTileValue(long int posX, long int posY, unsigned char valueIfInvalidPos) const {
			return this->IsPositionValid(posX, posY) ? ptrColsPtr[posX][posY] : valueIfInvalidPos/*CST_MAP_BUILD_LIKE_DISABLED*/;
		}
		// Set a tile like value
		// Will not fail/crash if position is out of bounds
		void SetTileValue(long int posX, long int posY, long int value) {
			if (this->IsPositionValid(posX, posY)) { ptrColsPtr[posX][posY] = (unsigned char)value; }
		}
		// Increments a tile like value with provided value (can be <0). Preserves disabled ones (0xFF)
		// Will not fail/crash if position is out of bounds
		void AddToMapLikeValue(long int posX, long int posY, long int relativeValue) {
			if (!this->IsPositionValid(posX, posY) || (ptrColsPtr[posX][posY] == CST_MAP_BUILD_LIKE_DISABLED)) {
				return;
			}
			long int v = ptrColsPtr[posX][posY]; // long int to avoid overflows
			v += relativeValue;
			if (v > CST_MAP_BUILD_LIKE_MAX_VALUE) { v = CST_MAP_BUILD_LIKE_MAX_VALUE; }
			if (v < 0) { v = 0; }
			ptrColsPtr[posX][posY] = (unsigned char)v;

		}
		// Set a tile like value but preserves disabled ones (0xFF)
		// Will not fail/crash if position is out of bounds.
		void SetMapLikeValueIfNotFF(long int posX, long int posY, long int value) {
			if (this->IsPositionValid(posX, posY) && (ptrColsPtr[posX][posY] != CST_MAP_BUILD_LIKE_DISABLED)) {
				ptrColsPtr[posX][posY] = (unsigned char)value;
			}
		}

		// Returns true if 1 of the line-end is disabled.
		// On each side, if one of the last 2 tiles is disabled, then the result is true
		bool IsAnExtremityBlocked_yLine(long int posX, long int posY_low, long int posY_high) const {
			// Checks on values
			if (posY_low > posY_high) { return true; }
			if (posX < 0) { posX = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX >= this->arraySizeX) { posX = arraySizeX - 1; }
			if (posY_low >= this->arraySizeY) { posY_low = arraySizeY - 1; }
			if (posY_high >= this->arraySizeY) { posY_high = arraySizeY - 1; }
			unsigned char *currentRow = this->ptrColsPtr[posX];
			if (posY_low >= arraySizeY - 1) { return true; }
			if ((currentRow[posY_low] == CST_MAP_BUILD_LIKE_DISABLED) || (currentRow[posY_low + 1] == CST_MAP_BUILD_LIKE_DISABLED)) {
				return true;
			}
			if (posY_high <= 0) { return true; }
			if ((currentRow[posY_high] == CST_MAP_BUILD_LIKE_DISABLED) || (currentRow[posY_high - 1] == CST_MAP_BUILD_LIKE_DISABLED)) {
				return true;
			}
			return false;
		}

		// Returns true if 1 of the line-end is disabled.
		// On each side, if one of the last 2 tiles is disabled, then the result is true
		bool IsAnExtremityBlocked_xLine(long int posX_low, long int posX_high, long int posY) const {
			// Checks on values
			if (posX_low > posX_high) { return true; }
			if (posY < 0) { posY = 0; }
			if (posX_low < 0) { posX_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY >= this->arraySizeY) { posY = arraySizeY - 1; }
			if (posX_low >= this->arraySizeX) { posX_low = arraySizeX - 1; }
			if (posX_high >= this->arraySizeX) { posX_high = arraySizeX - 1; }
			unsigned char *currentRow = this->ptrColsPtr[posX_low];
			if (this->ptrColsPtr[posX_low][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			if (posX_low >= arraySizeX - 1) { return true; }
			if (this->ptrColsPtr[posX_low + 1][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			if (this->ptrColsPtr[posX_high][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			if (posX_high <= 0) { return true; }
			if (this->ptrColsPtr[posX_high - 1][posY] == CST_MAP_BUILD_LIKE_DISABLED) { return true; }
			return false;
		}

		bool IsRegionAllDisabled(long int posX_low, long int posY_low, long int posX_high, long int posY_high) const {
			// Checks on values
			if ((posX_low > posX_high) || (posY_low > posY_high)) { return true; }
			if (posX_low < 0) { posX_low = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX_low >= this->arraySizeX) { posX_low = arraySizeX - 1; }
			if (posY_low >= this->arraySizeY) { posY_low = arraySizeY - 1; }
			if (posX_high >= this->arraySizeX) { posX_high = arraySizeX - 1; }
			if (posY_high >= this->arraySizeY) { posY_high = arraySizeY - 1; }
			for (long int i = posX_low; i <= posX_high; i++) {
				unsigned char *currentRow = this->ptrColsPtr[i];
				for (long int j = posY_low; j <= posY_high; j++) {
					if (currentRow[j] != CST_MAP_BUILD_LIKE_DISABLED) {
						return false;
					}
				}
			}
			return true;
		}

		// Set tile like value for all tiles in a rectangle (borders included)
		void SetMapLikeValueRectangle(long int posX_low, long int posY_low, long int posX_high, long int posY_high, long int value, bool preserveDisabled = false) {
			// Checks on values
			if ((posX_low > posX_high) || (posY_low > posY_high)) { return; }
			if (posX_low < 0) { posX_low = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX_low >= this->arraySizeX) { posX_low = arraySizeX - 1; }
			if (posY_low >= this->arraySizeY) { posY_low = arraySizeY - 1; }
			if (posX_high >= this->arraySizeX) { posX_high = arraySizeX - 1; }
			if (posY_high >= this->arraySizeY) { posY_high = arraySizeY - 1; }
			// Treatments
			for (long int i = posX_low; i <= posX_high; i++) {
				unsigned char *currentRow = this->ptrColsPtr[i];
				for (long int j = posY_low; j <= posY_high; j++) {
					if (!preserveDisabled || (currentRow[j] != CST_MAP_BUILD_LIKE_DISABLED)) {
						currentRow[j] = (unsigned char)value;
					}
				}
			}
		}

		// Add like value for all tiles in a rectangle (borders included). Value can be negative.
		// This does NOT modify disabled tiles (0xFF)
		void AddMapLikeValueRectangle(long int posX_low, long int posY_low, long int posX_high, long int posY_high, long int value) {
			// Checks on values
			if ((posX_low > posX_high) || (posY_low > posY_high)) { return; }
			if (posX_low < 0) { posX_low = 0; }
			if (posY_low < 0) { posY_low = 0; }
			if (posX_high < 0) { posX_high = 0; }
			if (posY_high < 0) { posY_high = 0; }
			if (posX_low >= this->arraySizeX) { posX_low = arraySizeX - 1; }
			if (posY_low >= this->arraySizeY) { posY_low = arraySizeY - 1; }
			if (posX_high >= this->arraySizeX) { posX_high = arraySizeX - 1; }
			if (posY_high >= this->arraySizeY) { posY_high = arraySizeY - 1; }
			// Treatments
			for (long int i = posX_low; i <= posX_high; i++) {
				unsigned char *currentRow = this->ptrColsPtr[i];
				for (long int j = posY_low; j <= posY_high; j++) {
					long int tmpvalue = currentRow[j];
					if (tmpvalue != CST_MAP_BUILD_LIKE_DISABLED) {
						tmpvalue += value; // Note: value can be <0
						if (tmpvalue > CST_MAP_BUILD_LIKE_MAX_VALUE) { tmpvalue = CST_MAP_BUILD_LIKE_MAX_VALUE; }
						if (tmpvalue < 0) { tmpvalue = 0; }
						currentRow[j] = (unsigned char)tmpvalue;
					}
				}
			}
		}

		// For each of the 4 sides of a building:
		// If the side is blocked (all neighbour tiles are disabled, side excluding corners),
		// then disable opposite side neighbour tiles (including corners)
		void ProtectBuildingOppositeSides(int unitPosX, int unitPosY, float unitRadius_f, char valueToUse = AOE_STRUCTURES::CST_MAP_BUILD_LIKE_DISABLED) {
			int unitRadius = (int)unitRadius_f;
			int unitRadiusDecreaseForHighPositions = 1 - ((int)(unitRadius * 2) % 2); // 0 for values like 1.5 or 2.5, 1 for round values
			// Get borders of the building (last tile part of the building) for simpler calculations
			int xLeftSide = unitPosX - unitRadius;
			int xRightSide = unitPosX + unitRadius - unitRadiusDecreaseForHighPositions;
			int yBottomSide = unitPosY - unitRadius;
			int yTopSide = unitPosY + unitRadius - unitRadiusDecreaseForHighPositions;
			int includeCornersInDetection = 1; // Set to 0 to ignore diagonal-corner tile, 1 to take it into account.
			//assert((includeCornersInDetection == 0) || (includeCornersInDetection == 1));

			// Is left blocked ?
			//bool isDisabled = this->IsRegionAllDisabled(xLeftSide - 1, yBottomSide, xLeftSide - 1, yTopSide);
			bool isDisabled = this->IsAnExtremityBlocked_yLine(xLeftSide - 1, yBottomSide - includeCornersInDetection, yTopSide + includeCornersInDetection);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xRightSide + 1, yBottomSide - 1, xRightSide + 1, yTopSide + 1, valueToUse, true);
			}
			// Is Right blocked ?
			isDisabled = this->IsAnExtremityBlocked_yLine(xRightSide + 1, yBottomSide - includeCornersInDetection, yTopSide + includeCornersInDetection);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xLeftSide - 1, yBottomSide - 1, xLeftSide - 1, yTopSide + 1, valueToUse, true);
			}
			// Is bottom blocked ?
			isDisabled = this->IsAnExtremityBlocked_xLine(xLeftSide - includeCornersInDetection, xRightSide + includeCornersInDetection, yBottomSide - 1);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xLeftSide - 1, yTopSide + 1, xRightSide + 1, yTopSide + 1, valueToUse, true);
			}
			// Is top blocked ?
			isDisabled = this->IsAnExtremityBlocked_xLine(xLeftSide - includeCornersInDetection, xRightSide + includeCornersInDetection, yTopSide + 1);
			if (isDisabled) {
				// Mark opposite side as disabled (all side including corners)
				this->SetMapLikeValueRectangle(xLeftSide - 1, yBottomSide - 1, xRightSide + 1, yBottomSide - 1, valueToUse, true);
			}

			// If a corner tile if disabled, do we disable diagonal-opposed corner tile ?
		}
	};
	static_assert(sizeof(STRUCT_MAP_TILE_VALUES) == 0x28, "STRUCT_MAP_TILE_VALUES size");


}

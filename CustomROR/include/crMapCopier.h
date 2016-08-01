
#pragma once

#include <Windows.h> // POINT, GetCursorPos...
#include <assert.h>
#include <string>
#include <set>
#include <AOE_offsets.h>
#include <ROR_structures.h>
#include "UI_utilities.h"
#include "traceMessage.h"
#include "AOE_map.h"
#include "crCommon.h"


using namespace ROR_STRUCTURES_10C;

#define MAX_MAP_SIZE 255

namespace MAP {

	// Represents copied unit data
	class UNIT_INSTANCE_SERIALIZED_DATA {
	public:
		UNIT_INSTANCE_SERIALIZED_DATA() {
			this->playerId = -1;
			this->unitDefId = -1;
			this->unitId = -1;
		}
		float posX;
		float posY;
		//posZ
		short int unitDefId;
		long int unitId;
		char orientation; // float in serialized format in AOE ?
		char status;
		short int playerId;
	};

	class MapCopier {
	public:
		MapCopier();
		~MapCopier();

		// Copy map data into internal buffer. Terrain+units
		bool CopyMapZone(long int minX, long int minY, long int maxX, long int maxY);
		// Paste from internal buffer to actual map
		bool PasteMapZone(long int startPosX, long int startPosY);

		long int GetCopiedSizeX() { return this->copiedSizeX; }
		long int GetCopiedSizeY() { return this->copiedSizeY; }
		long int GetCopiedUnitsCount() { return this->copiedUnits.size(); }

	private:
		const int maxArraySize = MAX_MAP_SIZE;
		TERRAIN_BYTE copiedTerrainData[MAX_MAP_SIZE][MAX_MAP_SIZE];
		BORDER_BYTE copiedBorderData[MAX_MAP_SIZE][MAX_MAP_SIZE];
		char copiedElevationIndexData[MAX_MAP_SIZE][MAX_MAP_SIZE];
		long int copiedSizeX;
		long int copiedSizeY;
		std::list<UNIT_INSTANCE_SERIALIZED_DATA*> copiedUnits;

		void ClearCopiedUnits();
	};


	extern MapCopier mapCopierInstance;
}

#undef MAX_MAP_SIZE


#pragma once

#include <Windows.h> // POINT, GetCursorPos...
#include <assert.h>
#include <string>
#include <set>
#include <AOE_offsets.h>
#include <AOE_struct_game_settings.h>
#include "UI_utilities.h"
#include "traceMessage.h"
#include "AOE_map.h"
#include "RockNRorCommon.h"
#include "AOEPrimitives_units.h"


using namespace AOE_STRUCTURES;

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
		float orientation; // Represents orientation for types 30-80, orientationIndex as float for types 10/20/90.
		char status;
		short int playerId;
	};

	class MapCopier {
	public:
		MapCopier();
		~MapCopier();

		// Copy map data into internal buffer (Terrain+units).
		// includeUnits: if true, all units are copied. If false, only eye candy units are copied.
		bool CopyMapZone(long int minX, long int minY, long int maxX, long int maxY, bool includeUnits);
		// Paste from internal buffer to actual map
		bool PasteMapZone(long int startPosX, long int startPosY);

		long int GetCopiedSizeX() const { return this->copiedSizeX; }
		long int GetCopiedSizeY() const { return this->copiedSizeY; }
		long int GetCopiedUnitsCount() const { return this->copiedUnits.size(); }
		bool BufferContainsUnits() const { return this->copiedBufferContainsUnits; }
		std::string GetLastError() const { return this->lastError; };

	private:
		const int maxArraySize = MAX_MAP_SIZE;
		TERRAIN_BYTE copiedTerrainData[MAX_MAP_SIZE][MAX_MAP_SIZE];
		TERRAIN_BORDER_BYTE copiedBorderData[MAX_MAP_SIZE][MAX_MAP_SIZE];
		char copiedElevationIndexData[MAX_MAP_SIZE][MAX_MAP_SIZE];
		long int copiedSizeX;
		long int copiedSizeY;
		std::list<UNIT_INSTANCE_SERIALIZED_DATA*> copiedUnits;
		bool copiedBufferContainsUnits;
		std::string lastError;

		void ClearCopiedUnits();
	};


	extern MapCopier mapCopierInstance;
}

#undef MAX_MAP_SIZE

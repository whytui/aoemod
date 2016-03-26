#include "../include/ROR_struct_export.h"

namespace ROR_STRUCTURES_10C {
	
	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(char *obj, unsigned long int RORAddress) {
		return std::string(obj);
	}

	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_GAME_SETTINGS *obj, unsigned long int RORAddress) {
		std::string res = "";
		res += "COMM: ";
		res += this->ExportStruct(obj->MPCommunicationStruct, sizeof(ROR_STRUCTURES_10C::STRUCT_MP_COMMUNICATION));
		return res;
	}

	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_GAME_GLOBAL *obj, unsigned long int RORAddress) {
		std::string res = "TotalPlayerCount=";
		res += std::to_string(obj->playerTotalCount);
		res += ", time=";
		res += std::to_string(obj->currentGameTime);
		res += "ms, unitSeq=";
		res += std::to_string(obj->seqUnitId);
		return res;
	}

	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_MP_COMMUNICATION *obj, unsigned long int RORAddress) {
		std::string res = "Player (1-8) types =";
		assert(obj->maximumPlayerCount == 8);
		for (int i = 1; i < 9; i++) {
			res += " ";
			res += std::to_string(obj->playerTypes[i]);
		}
		return res;
	}

	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_PLAYER *obj, unsigned long int RORAddress) {
		std::string res = "Player ";
		res += std::to_string(obj->playerId);
		res += " addr= ";
		char buf[11];
		sprintf_s(buf, "0x%08X", RORAddress);
		res += buf;
		res += " - AI= ";
		sprintf_s(buf, "0x%08X", obj->ptrAIStruct);
		res += buf;
		res += " - Name= ";
		res += this->ExportStruct(obj->playerName_length16max, 16);

		return res;
	}

	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNIT_BASE *obj, unsigned long int RORAddress) {
		std::string res = "UnitId=";
		res += std::to_string(obj->unitInstanceId);
		res += " - addr= ";
		char buf[11];
		sprintf_s(buf, "0x%08X", RORAddress);
		res += buf;
		res += " - status=";
		res += std::to_string(obj->unitStatus);
		res += "\nHP=";
		res += std::to_string(obj->remainingHitPoints);
		res += " - shortcut=";
		res += std::to_string(obj->shortcutNumber);
		res += " - x=";
		res += std::to_string(obj->positionX);
		res += " - y=";
		res += std::to_string(obj->positionY);
		res += " - UnitId_78?=";
		res += std::to_string(obj->unknown_078_unitInstanceId);
		res += "\nDefinition: ";
		res += this->ExportStruct(obj->ptrStructDefUnit);
		return res;
	}
	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNIT *obj, unsigned long int RORAddress) {
		return this->ExportStruct_internal((STRUCT_UNIT_BASE*)obj, RORAddress);
	}

	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_DEF_UNIT *obj, unsigned long int RORAddress) {
		return this->ExportStruct_internal((STRUCT_UNITDEF_BASE*)obj, RORAddress);
	}
	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNITDEF_BASE *obj, unsigned long int RORAddress) {
		std::string res = "DATID1=";
		res += std::to_string(obj->DAT_ID1);
		res += " DATID2=";
		res += std::to_string(obj->DAT_ID2);
		res += " - Name=";
		res += this->ExportStruct(obj->ptrUnitName, 16);
		res += " - Type=";
		res += std::to_string(obj->unitType);
		res += " - AIType=";
		res += std::to_string(obj->unitAIType);
		res += "\nTotalHP=";
		res += std::to_string(obj->totalHitPoints);
		res += " - lineOfSight=";
		res += std::to_string(obj->lineOfSight);
		res += " - commandAttribute=";
		res += std::to_string(obj->commandAttribute);
		res += " - interactionMode=";
		res += std::to_string(obj->interactionMode);
		res += " - visibleInFog=";
		res += std::to_string(obj->visibleInFog);
		res += " - terrainRestriction=";
		res += std::to_string(obj->terrainRestriction);
		res += "\n";
		if (obj->DerivesFromType50()) {
			res += this->ExportStruct<STRUCT_UNITDEF_TYPE50>((STRUCT_UNITDEF_TYPE50*)RORAddress);
		}
		return res;
	}

	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNITDEF_TYPE50 *obj, unsigned long int RORAddress) {
		std::string res = "\nArmor=";
		res += std::to_string(obj->displayedArmor);
		res += this->ExportArrayOfStruct(obj->ptrArmorsList, obj->armorsCount);
		res += "\nAttack=";
		res += std::to_string(obj->displayedAttack);
		res += this->ExportArrayOfStruct(obj->ptrAttacksList, obj->attacksCount);
		res += "\nSpeed=";
		res += std::to_string(obj->speed);
		res += " - searchRadius=";
		res += std::to_string(obj->searchRadius);
		res += " - reloadTime1=";
		res += std::to_string(obj->reloadTime1);
		res += " - reloadTime2=";
		res += std::to_string(obj->reloadTime2);
		res += " - workRate=";
		res += std::to_string(obj->workRate);
		return res;
	}


	std::string ROR_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_ARMOR_OR_ATTACK *obj, unsigned long int RORAddress) {
		std::string res = "cl=";
		res += std::to_string(obj->classId);
		res += " amount=";
		res += std::to_string(obj->amount);
		return res;
	}

}


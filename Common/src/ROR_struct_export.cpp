#include "../include/AOE_struct_export.h"

namespace AOE_STRUCTURES {
	
	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(char *obj, unsigned long int RORAddress) {
		return std::string(obj);
	}

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_GAME_SETTINGS *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res = "";
		res += "DM=";
		res += std::to_string(obj->isDeathMatch);
		res += " isScn=";
		res += std::to_string(obj->isScenario);
		res += " savedGame=";
		res += std::to_string(obj->isSavedGame);
		res += "\nCOMM: ";
		res += this->ExportStruct(obj->MPCommunicationStruct, sizeof(AOE_STRUCTURES::STRUCT_MP_COMMUNICATION));
		return res;
	}

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_GAME_GLOBAL *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res = "TotalPlayerCount=";
		res += std::to_string(obj->playerTotalCount);
		res += ", time=";
		res += std::to_string(obj->currentGameTime);
		res += "ms, unitSeq=";
		res += std::to_string(obj->seqUnitId);
		return res;
	}

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_MP_COMMUNICATION *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res = "Player (1-8) types =";
		assert(obj->maximumPlayerCount == 8);
		for (int i = 1; i < 9; i++) {
			res += " ";
			res += std::to_string(obj->playerTypes[i]);
		}
		return res;
	}

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_PLAYER *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
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

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_AI *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res = "targetPlayer=";
		int targetPlayersUsedElem = obj->structTacAI.targetPlayers.usedElements;
		if (targetPlayersUsedElem > 0) {
			long int firstArrayValue = this->GetDwordFromRORData((unsigned long)obj->structTacAI.targetPlayers.unitIdArray);
			res += std::to_string(firstArrayValue);
		}
		return res;
	}

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNIT_BASE *obj, unsigned long int RORAddress) {
		std::string res = "UnitId=";
		res += std::to_string(obj->unitInstanceId);
		res += " [p";
		unsigned long int unitPlayerId = this->GetDwordFromRORData((unsigned long int)&obj->ptrStructPlayer->playerId) & 0xFFFF;
		res += std::to_string(unitPlayerId);
		res += "] addr= ";
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
		res += this->ExportStruct(obj->unitDefinition);
		res += "\n\nActivity: ";
		res += this->ExportStruct(obj->currentActivity);
		if (obj->DerivesFromCommandable()) {
			res += this->ExportStruct<STRUCT_UNIT_COMMANDABLE>((STRUCT_UNIT_COMMANDABLE*)RORAddress);
		}
		return res;
	}
	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNIT_COMMANDABLE *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res;
		if (obj->ptrActionInformation) {
			res += "\nactionInfo[";
			res += GetHexStringAddress(obj->ptrActionInformation);
			res += "]: ";
			res += this->ExportStruct(obj->ptrActionInformation);
		}
		if (obj->DerivesFromTrainable()) {
			res += this->ExportStruct<STRUCT_UNIT_TRAINABLE>((STRUCT_UNIT_TRAINABLE*)RORAddress);
		}
		return res;
	}
	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNIT_TRAINABLE *obj, unsigned long int RORAddress) {
		STRUCT_UNIT_BUILDING *bld = (STRUCT_UNIT_BUILDING*)obj;
		std::string res;
		if (bld->IsTypeValid()) {
			res += this->ExportStruct<STRUCT_UNIT_BUILDING>((STRUCT_UNIT_BUILDING*)RORAddress);
		}
		return res;
	}
	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNIT_BUILDING *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res;
		if (obj->trainUnitActionInfo) {
			res += "\nbldActionInfo: ";
			res += this->ExportStruct(obj->trainUnitActionInfo);
		}
		return res;
	}

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNITDEF_BASE *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res = "DATID1=";
		res += std::to_string(obj->DAT_ID1);
		res += " DATID2=";
		res += std::to_string(obj->DAT_ID2);
		res += " - Name=";
		res += this->GetZeroTerminatedStringFromRORData((unsigned long int)obj->ptrUnitName);
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
		if (obj->DerivesFromAttackable()) {
			res += this->ExportStruct<STRUCT_UNITDEF_ATTACKABLE>((STRUCT_UNITDEF_ATTACKABLE*)RORAddress);
		}
		return res;
	}

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNITDEF_ATTACKABLE *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
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


	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_ARMOR_OR_ATTACK *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res = "cl=";
		res += std::to_string(obj->classId);
		res += " amount=";
		res += std::to_string(obj->amount);
		return res;
	}


	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNIT_GROUP *obj, unsigned long int RORAddress) {
		std::string res = "id=";
		res += std::to_string(obj->unitGroupId);
		res += " type=";
		res += std::to_string(obj->unitGroupType);
		res += " currentTask=";
		res += std::to_string(obj->currentTask);
		res += " taskSubTypeId=";
		res += std::to_string(obj->taskSubTypeId);
		res += "\n\tcommanderUnitId=";
		res += std::to_string(obj->commanderUnitId);
		res += " unitCount=";
		res += std::to_string(obj->unitCount);
		res += " totalGroupHP=";
		res += std::to_string(obj->initialTotalGroupHP);
		res += "\n\tposX=";
		res += std::to_string(obj->posX);
		res += " posY=";
		res += std::to_string(obj->posY);
		res += " - retreatPosX=";
		res += std::to_string(obj->retreatPosX);
		res += " retreatPosY=";
		res += std::to_string(obj->retreatPosY);
		res += "\n\ttargetPlayerId=";
		res += std::to_string(obj->targetPlayerId);
		res += " targetUnitId=";
		res += std::to_string(obj->targetUnitId);
		res += " targetDAT_ID=";
		res += std::to_string(obj->targetDAT_ID);
		res += " targetPosX=";
		res += std::to_string(obj->targetPosX);
		res += " targetPosY=";
		res += std::to_string(obj->targetPosY);
		res += " attackPlayId=";
		res += std::to_string(obj->attackPlayId);
		res += " targetCount=";
		res += std::to_string(obj->targetUnitIdArrayUsedElemCount);
		res += "\n\tunknown_198=";
		res += std::to_string(obj->unknown_198);
		res += " unknown_1A8=";
		res += std::to_string(obj->unknown_1A8);
		res += " unknown_1B8=";
		res += std::to_string(obj->unknown_1B8);
		res += " unknown_1BC=";
		res += std::to_string(obj->unknown_1BC);
		res += " lastTaskTime_ms=";
		res += std::to_string(obj->lastTaskingTime_ms);
		return res;
	}


	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_ACTION_BASE *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res = "actionType=";
		res += std::to_string(obj->actionTypeID);
		res += ", status=";
		res += std::to_string(obj->actionStatus);
		res += ", actor=";
		res += GetHexStringAddress((unsigned long int)obj->actor);
		res += ", target=";
		res += std::to_string(obj->targetUnitId);
		if (obj->requiredActionInfo != NULL) {
			res += " requiredActionInfo=";
			res += ExportStruct(obj->requiredActionInfo);
		}
		return res;
	}

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNIT_ACTION_INFO *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		if (!obj->ptrActionLink) {
			return "No actionLink";
		}
		return this->ExportStruct(obj->ptrActionLink);
	}

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_ACTION_LINK *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res;
		if (obj->actionStruct) {
			res = "Action: ";
			res += this->ExportStruct(obj->actionStruct);
		}
		if (obj->nextActionLink) {
			res += "\nHasNext=> ";
			res += this->ExportStruct(obj->nextActionLink);
		}
		return res;
	}

	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNIT_ACTIVITY *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res;
		res += "Activity id=";
		//res += std::to_string(obj->currentActionId);
		res += GetHexStringAddress(obj->currentActionId, 3);
		res += " [+28]=";
		//res += std::to_string(obj->internalId_whenAttacked);
		res += GetHexStringAddress(obj->internalId_whenAttacked, 3);
		res += " (backup: ";
		//res += std::to_string(obj->previousActionId);
		res += GetHexStringAddress(obj->previousActionId, 3);
		res += " - ";
		//res += std::to_string(obj->previous_whenAttackedInternalId);
		res += GetHexStringAddress(obj->previous_whenAttackedInternalId, 3);
		res += ") curPos X=";
		res += std::to_string(obj->currentPosX);
		res += " Y=";
		res += std::to_string(obj->currentPosY);
		res += " maxDist=";
		res += std::to_string(obj->maxDistance);
		res += "\nPos X=";
		res += std::to_string(obj->targetPosX);
		res += " Y=";
		res += std::to_string(obj->targetPosY);
		res += "\nTarget id=";
		res += std::to_string(obj->targetUnitId);
		res += " AIType=";
		res += std::to_string(obj->targetUnitType);
		res += " (previous id=";
		res += std::to_string(obj->previousTargetUnitId);
		res += " AIType=";
		res += std::to_string(obj->previousTargetUnitType);
		res += ")\ntargetArraySize=";
		res += std::to_string(obj->targetsInfoArrayUsedElements);
		res += " ";
		res += this->ExportArrayOfStruct(obj->targetsInfoArray, obj->targetsInfoArrayUsedElements);

		res += "\nunknown60:";
		for (int i = 0; i < obj->ListOfUnitIdThatAttackedMe.usedElements; i++) {
			unsigned long int tmp = this->GetDwordFromRORData((unsigned long int)(obj->ListOfUnitIdThatAttackedMe.unitIdArray + i));
			long int unitId = (long int)tmp;
			res += " ";
			res += std::to_string(unitId);
		}
		return res;
	}


	std::string AOE_STRUCT_EXPORTER::ExportStruct_internal(STRUCT_UNIT_ACTIVITY_TARGET_ELEM *obj, unsigned long int RORAddress) {
		if (!obj) { return "NULL"; }
		std::string res = "[";
		res += std::to_string(obj->actorUnitId);
		res += " atk p";
		res += std::to_string(obj->targetPlayerId);
		res += "] internalId=";
		res += GetHexStringAddress(obj->internalId, 3);
		res += " target=";
		res += std::to_string(obj->targetUnitId);
		return res;
	}

}


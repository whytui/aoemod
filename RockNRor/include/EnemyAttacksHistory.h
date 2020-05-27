#pragma once

#include <assert.h>
#include "GameHistoryIntervalsBase.h"
#include "serialize.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	// Represents a game time interval and a number of attacks during this time frame.
	class TimeIntervalAttackRecord : public ROCKNROR::TimeIntervalRecordBase {
	public:
		long int attacksCount;
		long int attacksCountInMyTown;
		long int deathsCount; // How many of "my" units were killed (excludes conversions, excludes depleted farms !)
		long int convertedCount; // How many of "my" units were converted by enemies
		long int triggeredPanicModesCount; // Number of times strategy panic mode was triggered because of this player's attacks
		long int attacksCountByClass[62];

		void ResetAllInfo() override {
			__super::ResetAllInfo();
			this->attacksCount = 0;
			this->attacksCountInMyTown = 0;
			this->deathsCount = 0;
			this->convertedCount = 0;
			this->triggeredPanicModesCount = 0;
			for (int i = 0; i < _countof(this->attacksCountByClass); i++) { this->attacksCountByClass[i] = 0; }
		}

		long int Serialize(FILE *outputFile) const override {
			long int result = __super::Serialize(outputFile);

			result += this->WriteBytes(outputFile, &this->attacksCount, sizeof(this->attacksCount));
			result += this->WriteBytes(outputFile, &this->attacksCountInMyTown, sizeof(this->attacksCountInMyTown));
			result += this->WriteBytes(outputFile, &this->deathsCount, sizeof(this->deathsCount));
			result += this->WriteBytes(outputFile, &this->convertedCount, sizeof(this->convertedCount));
			result += this->WriteBytes(outputFile, &this->triggeredPanicModesCount, sizeof(this->triggeredPanicModesCount));
			for (int i = 0; i < _countof(attacksCountByClass); i++) {
				result += this->WriteBytes(outputFile, &this->attacksCountByClass[i], sizeof(this->attacksCountByClass[i]));
			}

			return result;
		}

		bool Deserialize(FILE *inputFile) override {
			__super::Deserialize(inputFile);

			this->ReadBytes(inputFile, &this->attacksCount, sizeof(this->attacksCount));
			this->ReadBytes(inputFile, &this->attacksCountInMyTown, sizeof(this->attacksCountInMyTown));
			this->ReadBytes(inputFile, &this->deathsCount, sizeof(this->deathsCount));
			this->ReadBytes(inputFile, &this->convertedCount, sizeof(this->convertedCount));
			this->ReadBytes(inputFile, &this->triggeredPanicModesCount, sizeof(this->triggeredPanicModesCount));
			for (int i = 0; i < _countof(attacksCountByClass); i++) {
				this->ReadBytes(inputFile, &this->attacksCountByClass[i], sizeof(this->attacksCountByClass[i]));
			}
				
			return true;
		}

		// Record an attack from a unit with the unit class specified
		void AddAttackRecord(GLOBAL_UNIT_AI_TYPES attackerClass, bool isInTown) {
			this->attacksCount++;
			if (isInTown) {
				this->attacksCountInMyTown++;
			}
			if ((attackerClass < 0) || (attackerClass >= _countof(this->attacksCountByClass))) { return; }
			this->attacksCountByClass[attackerClass]++;
		}
	};
	
	template <typename T> class TimeIntervalAttacksRecordForPlayer : public ROCKNROR::GameHistoryIntervalsBase<T> {
	public:
		TimeIntervalAttacksRecordForPlayer() {
			this->lastAttackInMyTownPosX = -1;
			this->lastAttackInMyTownPosY = -1;
		}

		void ResetAllInfo() override {
			__super::ResetAllInfo();
			this->lastAttackInMyTownPosX = -1;
			this->lastAttackInMyTownPosY = -1;
		}

		long int attackerPlayerId;
		float lastAttackInMyTownPosX;
		float lastAttackInMyTownPosY;

		long int Serialize(FILE *outputFile) const override {
			long int result = 0;

			this->WriteBytes(outputFile, &this->lastAttackInMyTownPosX, sizeof(this->lastAttackInMyTownPosX));
			this->WriteBytes(outputFile, &this->lastAttackInMyTownPosY, sizeof(this->lastAttackInMyTownPosY));
			__super::Serialize(outputFile);

			return result;
		}
		bool Deserialize(FILE *inputFile) override {
			this->ReadBytes(inputFile, &this->lastAttackInMyTownPosX, sizeof(this->lastAttackInMyTownPosX));
			this->ReadBytes(inputFile, &this->lastAttackInMyTownPosY, sizeof(this->lastAttackInMyTownPosY));
			__super::Deserialize(inputFile);
			return true;
		}

		// Get the number of attacks from a player during the specified interval (game times in milliseconds)
		int GetAttacksCountInPeriod(long int startGameTime, long int endGameTime) {
			if (endGameTime <= startGameTime) { return 0; }
			if (startGameTime < 0) { startGameTime = 0; }
			CUSTOM_AI::TimeIntervalAttackRecord *interval = this->GetIntervalForGameTime(startGameTime);
			if (interval == NULL) {
				interval = this->GetOldestValidInterval();
			}
			int result = 0;
			while (interval && (interval->intervalStartGameTime <= endGameTime)) {
				result += interval->attacksCount;
				interval = this->GetNextInterval(interval);
			}
			return result;
		}

		// Get the number of attacks *in my town* from a player during the specified interval (game times in milliseconds)
		int GetAttacksInMyTownCountInPeriod(long int startGameTime, long int endGameTime) {
			if (endGameTime <= startGameTime) { return 0; }
			CUSTOM_AI::TimeIntervalAttackRecord *interval = this->GetIntervalForGameTime(startGameTime);
			if (interval == NULL) {
				interval = this->GetOldestValidInterval();
			}
			int result = 0;
			while (interval && (interval->intervalStartGameTime <= endGameTime)) {
				result += interval->attacksCountInMyTown;
				interval = this->GetNextInterval(interval);
			}
			return result;
		}

		// Get the number of panic modes cause by a player during the specified interval (game times in milliseconds)
		int GetPanicModesCountInPeriod(long int startGameTime, long int endGameTime) {
			if (endGameTime <= startGameTime) { return 0; }
			CUSTOM_AI::TimeIntervalAttackRecord *interval = this->GetIntervalForGameTime(startGameTime);
			if (interval == NULL) {
				interval = this->GetOldestValidInterval();
			}
			int result = 0;
			while (interval && (interval->intervalStartGameTime <= endGameTime)) {
				result += interval->triggeredPanicModesCount;
				interval = this->GetNextInterval(interval);
			}
			return result;
		}
	};

	
}

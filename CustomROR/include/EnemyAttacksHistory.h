#pragma once

#include <assert.h>
#include "GameHistoryIntervalsBase.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	// Represents a game time interval and a number of attacks during this time frame.
	class TimeIntervalAttackRecord : public CUSTOMROR::TimeIntervalRecordBase {
	public:
		long int attacksCount;
		long int triggeredPanicModesCount; // Number of times strategy panic mode was triggered because of this player's attacks
		long int attacksCountByClass[62];

		void ResetAllInfo() override {
			__super::ResetAllInfo();
			this->attacksCount = 0;
			this->triggeredPanicModesCount = 0;
			for (int i = 0; i < _countof(this->attacksCountByClass); i++) { this->attacksCountByClass[i] = 0; }
		}

		// Record an attack from a unit with the unit class specified
		void AddAttackRecord(GLOBAL_UNIT_AI_TYPES attackerClass) {
			this->attacksCount++;
			if ((attackerClass < 0) || (attackerClass >= _countof(this->attacksCountByClass))) { return; }
			this->attacksCountByClass[attackerClass]++;
		}
	};
	
	template <typename T> class TimeIntervalAttacksRecordForPlayer : public CUSTOMROR::GameHistoryIntervalsBase<T> {
	public:
		TimeIntervalAttacksRecordForPlayer() {
			
		}

		long int attackerPlayerId;


		// Get the number of attacks from a player during the specified interval (game times in milliseconds)
		int GetAttacksCountInPeriod(long int startGameTime, long int endGameTime) {
			if (endGameTime <= startGameTime) { return 0; }
			CUSTOM_AI::TimeIntervalAttackRecord *interval = this->GetIntervalForGameTime(startGameTime);
			if (interval == NULL) {
				interval = this->GetOldestValidInterval();
			}
			int result = 0;
			while (interval && (interval->intervalEndGameTime > endGameTime)) {
				result += interval->attacksCount;
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
			while (interval && (interval->intervalEndGameTime > endGameTime)) {
				result += interval->triggeredPanicModesCount;
				interval = this->GetNextInterval(interval);
			}
			return result;
		}
	};

	
}

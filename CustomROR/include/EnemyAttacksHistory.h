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

		void ResetAllInfo() override {
			__super::ResetAllInfo();
			this->attacksCount = 0;
			this->triggeredPanicModesCount = 0;
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
			int result = 0;
			while (interval && (interval->intervalEndGameTime > endGameTime)) {
				result += interval->attacksCount;
				interval = this->GetNextInterval(interval);
			}
			return result;
		}
	};

	
}

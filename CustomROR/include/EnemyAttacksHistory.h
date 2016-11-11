#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_main_ai.h>
#include "CustomRORInfo.h"
#include "StrategyUpdater.h"
#include "GameHistoryIntervalsBase.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {


	class TimeIntervalAttackRecord : public CUSTOMROR::TimeIntervalRecordBase {
	public:
		long int attacksCount;

		void ResetAllInfo() override {
			__super::ResetAllInfo();
			this->attacksCount = 0;
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

#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_main_ai.h>
#include "CustomRORInfo.h"
#include "StrategyUpdater.h"

using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	namespace AI_CONST {
		const long int maxAttackIntervalsInHistory = 20; // Number of (msot recent) intervals we keep in history
		const long int attackRecordBasicIntervalMilliseconds = 5000;
		static_assert(AI_CONST::attackRecordBasicIntervalMilliseconds > 0, "attackRecordBasicIntervalMilliseconds must be > 0");
	}

	struct TimeIntervalAttackRecord {
		long int intervalStartGameTime;
		long int intervalEndGameTime;
		long int attacksCount;

		void ResetAllInfo() {
			this->intervalEndGameTime = 0;
			this->intervalStartGameTime = 0;
			this->attacksCount = 0;
		}
	};
	
	class TimeIntervalAttacksRecordForPlayer {
	public:
		TimeIntervalAttacksRecordForPlayer() {
			this->ResetAllInfo();
		}

		long int attackerPlayerId;


		// Clear all underlying info
		void ResetAllInfo() {
			this->validIntervalCount = 0;
			this->indexOfMostRecentInterval = 0;
			for (int i = 0; i < AI_CONST::maxAttackIntervalsInHistory; i++) {
				this->attacksHistory[i].ResetAllInfo();
			}
		}
		// To be called on each game start/load. This calls ResetAllInfo and then initializes the necessary information.
		void InitOnGameStart(long int currentGameTime) {
			this->InitFirstInterval(currentGameTime);
		}

		// Get the most recent interval
		// Never returns NULL unless Init has NOT been called correctly
		TimeIntervalAttackRecord *GetMostRecentInterval() {
			if (this->validIntervalCount <= 0) {
				return NULL;
			}
			return this->GetInterval(this->indexOfMostRecentInterval);
		}
		// Get the oldest (valid) interval
		// Never returns NULL unless Init has NOT been called correctly
		TimeIntervalAttackRecord *GetOldestValidInterval() {
			if (this->validIntervalCount <= 0) {
				return NULL;
			}
			int index = this->indexOfMostRecentInterval - this->validIntervalCount + 1;
			index = this->GetFixedRawIntervalIndex(index);
			return this->GetInterval(index);
		}
		// Get interval for the specified game time (in general, should be current game time)
		// Automatically inserts missing intervals (if any)
		// Can return NULL if game time is before known intervals.
		TimeIntervalAttackRecord *GetIntervalForGameTime(long int gameTime) {
			TimeIntervalAttackRecord *curInterval = this->GetMostRecentInterval();
			if (curInterval == NULL) {
				this->InitFirstInterval(gameTime);
				curInterval = this->GetMostRecentInterval();
			}
			assert(curInterval != NULL);
			if (curInterval->intervalEndGameTime < gameTime) {
				// Game time is AFTER most recent interval
				this->AddMissingIntervals(gameTime);
				curInterval = this->GetMostRecentInterval();
				if ((curInterval->intervalStartGameTime > gameTime) || (curInterval->intervalEndGameTime < gameTime)) {
					traceMessageHandler.WriteMessage("AddMissingIntervals did not go well ! Result is inconsistent");
				}
				return curInterval;
			}
			TimeIntervalAttackRecord *oldestInterval = this->GetOldestValidInterval();
			if (oldestInterval->intervalStartGameTime > gameTime) {
				// Game time is BEFORE oldest interval
				return NULL;
			}
			// Check current interval
			if ((curInterval->intervalStartGameTime <= gameTime) && (gameTime <= curInterval->intervalEndGameTime)) {
				return curInterval;
			}
			// Other cases : find the relevant interval
			int curIndex = this->GetFixedRawIntervalIndex(this->indexOfMostRecentInterval - 1);
			curInterval = this->GetInterval(curIndex);
			while (curInterval != oldestInterval) {
				if ((curInterval->intervalStartGameTime <= gameTime) && (gameTime <= curInterval->intervalEndGameTime)) {
					return curInterval;
				}
				curIndex = this->GetFixedRawIntervalIndex(curIndex - 1);
				curInterval = this->GetInterval(curIndex);
			}
			assert(false && "This code should not be reached. Could not find the correct interval");
			traceMessageHandler.WriteMessage("ERROR: Could not find the correct interval");
			return NULL;
		}
		
		private:
			TimeIntervalAttackRecord attacksHistory[AI_CONST::maxAttackIntervalsInHistory]; // "circular" array.
			int indexOfMostRecentInterval; // current (last) index in array
			int validIntervalCount; // number of valid entries in array

			// Adds 1 new interval. Start time must correspond to most recent interval's end time (if there already is an interval)
			TimeIntervalAttackRecord *AddNewInterval(long int startGameTime) {
				if (this->validIntervalCount > 0) {
					if (this->GetInterval(this->indexOfMostRecentInterval)->intervalEndGameTime != startGameTime) {
						assert(false && "CustomAI: Non-contiguous intervals");
						startGameTime = this->GetInterval(this->indexOfMostRecentInterval)->intervalEndGameTime;
					}
				}
				if (this->validIntervalCount < AI_CONST::maxAttackIntervalsInHistory) {
					// Just use a new (free) slot = next slot
					this->validIntervalCount++;
				} else {
					// Overwrite oldest interval
				}
				// Next slot = free slot OR oldest one
				this->indexOfMostRecentInterval = this->GetFixedRawIntervalIndex(this->indexOfMostRecentInterval + 1); // Next interval index
				TimeIntervalAttackRecord *newInterval = this->GetInterval(this->indexOfMostRecentInterval);
				newInterval->ResetAllInfo();
				newInterval->intervalStartGameTime = startGameTime;
				newInterval->intervalEndGameTime = startGameTime + AI_CONST::attackRecordBasicIntervalMilliseconds;
				return newInterval;
			}

			// Add missing intervals if current game time belongs to an interval that is not contiguous to last known one
			void AddMissingIntervals(long int currentGameTime) {
				assert(currentGameTime >= 0);
				if (this->validIntervalCount <= 0) {
					this->InitFirstInterval(currentGameTime);
					return;
				}
				TimeIntervalAttackRecord *lastInterval = this->GetMostRecentInterval();
				long int diff = currentGameTime - lastInterval->intervalEndGameTime;
				if (diff <= 0) {
					return; // ERROR: currentGameTime is NOT after our last interval
				}
				// Maximum end time if using adding intervals in all slots (starting from "most recent one") = if we keep only the latest slot
				long int maxReachableTime = lastInterval->intervalEndGameTime + ((AI_CONST::maxAttackIntervalsInHistory - 1) * AI_CONST::attackRecordBasicIntervalMilliseconds);
				if (currentGameTime > maxReachableTime) {
					// Game time is beyond what we can reach. Which means we need to create new intervals on all slots, all intervals being "empty" (nothing happened)
					this->InitFirstInterval(currentGameTime);
					long int currentIntervalEndTime = currentGameTime + AI_CONST::attackRecordBasicIntervalMilliseconds;
					for (int i = 1; i < AI_CONST::maxAttackIntervalsInHistory; i++) { // loop: ignoring first one (already created by InitFirstInterval) 
						int curIndex = this->indexOfMostRecentInterval - i; // Go backwards
						curIndex = this->GetFixedRawIntervalIndex(curIndex);
						currentIntervalEndTime -= AI_CONST::attackRecordBasicIntervalMilliseconds;
						TimeIntervalAttackRecord *curInterval = this->GetInterval(curIndex);
						this->validIntervalCount++;
						curInterval->attacksCount = 0;
						curInterval->intervalEndGameTime = currentIntervalEndTime;
						curInterval->intervalStartGameTime = curInterval->intervalEndGameTime - AI_CONST::attackRecordBasicIntervalMilliseconds;
					}
					assert(this->validIntervalCount == AI_CONST::maxAttackIntervalsInHistory);
					return;
				}
				// Otherwise: we can add intervals until we reach the correct interval
				long int currentIntervalEndTime = lastInterval->intervalEndGameTime;
				for (int i = 0; i < AI_CONST::maxAttackIntervalsInHistory; i++) {
					TimeIntervalAttackRecord *curInterval = this->AddNewInterval(currentIntervalEndTime);
					currentIntervalEndTime = curInterval->intervalEndGameTime;
					if (currentIntervalEndTime >= currentGameTime) {
						return; // the new interval contains current game time
					}
				}
				traceMessageHandler.WriteMessage("ERROR: could not insert missing intervals in history");
			}

			// Get fixed "raw" index to fit in array (and avoid overflow). Result is always in [0, n-1]
			inline int GetFixedRawIntervalIndex(int index) {
				index = index % AI_CONST::maxAttackIntervalsInHistory;
				if (index < 0) {
					index += AI_CONST::maxAttackIntervalsInHistory;
				}
				return index;
			}
			// Securely get an interval provided its "raw" index.
			TimeIntervalAttackRecord *GetInterval(int rawIndex) {
				assert((rawIndex >= 0) && (rawIndex < AI_CONST::maxAttackIntervalsInHistory));
				int i = GetFixedRawIntervalIndex(rawIndex);
				return &this->attacksHistory[i];
			}
			// Creates the first interval with correct game time. This flushes all previous information !
			void InitFirstInterval(long int currentGameTime) {
				this->ResetAllInfo();
				TimeIntervalAttackRecord *interval = this->AddNewInterval(currentGameTime);
			}

	};

	
}

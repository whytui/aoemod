#pragma once

#include <assert.h>
#include <stdlib.h>
#include "traceMessage.h"



namespace CUSTOMROR {

	namespace AI_CONST {
		const long int maxIntervalsInHistory = 20; // Number of (most recent) intervals we keep in history
		const long int historyIntervalLength_ms = 5000;
		static_assert(AI_CONST::historyIntervalLength_ms > 0, "historyIntervalLength_ms must be > 0");
	}

	// Represents a basic interval of game time (cf historyIntervalLength_ms)
	// Can be derived to store specific information for time intervals.
	class TimeIntervalRecordBase {
	public:
		TimeIntervalRecordBase() { this->ResetBaseInfo(); }
		long int intervalStartGameTime; // Game time at beginning of interval
		long int intervalEndGameTime; // Game time at ending of interval

		// Reset all underlying information
		virtual void ResetAllInfo() {
			ResetBaseInfo();
		}
	private:
		// Reset all base class information
		void ResetBaseInfo() {
			this->intervalEndGameTime = 0;
			this->intervalStartGameTime = 0;
		}
	};
	

	// Represents a set of 'n' latest intervals (in game time).
	// Can be derived to add specific information/usage.
	template <typename T>
	class GameHistoryIntervalsBase {
	public:
		// Restriction for compiler: T should derive from TimeIntervalRecordBase (to get explicit messages). Not sure this works?
		static_assert(std::is_base_of<TimeIntervalRecordBase, T>::value, "GameHistoryIntervalsBase: T not derived from TimeIntervalRecordBase");

		GameHistoryIntervalsBase() {
			this->ResetAllInfo();
		}

		// Clear all underlying info
		virtual void ResetAllInfo() {
			this->validIntervalCount = 0;
			this->indexOfMostRecentInterval = 0;
			for (int i = 0; i < AI_CONST::maxIntervalsInHistory; i++) {
				this->attacksHistory[i].ResetAllInfo();
			}
		}
		// To be called on each game start/load. This calls ResetAllInfo and then initializes the necessary information.
		void InitOnGameStart(long int currentGameTime) {
			this->InitFirstInterval(currentGameTime);
		}

		// Get the most recent interval. This does NOT add any interval.
		// Never returns NULL unless Init has NOT been called correctly
		T *GetMostRecentInterval() {
			if (this->validIntervalCount <= 0) {
				return NULL;
			}
			return this->GetInterval(this->indexOfMostRecentInterval);
		}

		// Get the oldest (valid) interval. This does NOT add any interval.
		// Never returns NULL unless Init has NOT been called correctly
		T *GetOldestValidInterval() {
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
		T *GetIntervalForGameTime(long int gameTime) {
			T *curInterval = this->GetMostRecentInterval();
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
			T *oldestInterval = this->GetOldestValidInterval();
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

		// Returns the interval that precedes provided one.
		// Returns NULL if no such interval.
		T *GetPreviousInterval(T *currentInterval) {
			if (currentInterval == NULL) { return NULL; }
			if (currentInterval == this->GetOldestValidInterval()) { return NULL; } // OLDEST interval doesn't have a previous
			for (int i = 0; i < AI_CONST::maxIntervalsInHistory; i++) {
				if (this->GetInterval(i) == currentInterval) {
					int nextIndex = this->GetFixedRawIntervalIndex(i - 1);
					return this->GetInterval(nextIndex);
				}
			}
			return NULL; // Not found. Provided interval may not belong to this array !
		}

		// Returns the interval that follows provided one.
		// Returns NULL if no such interval.
		T *GetNextInterval(T *currentInterval) {
			if (currentInterval == NULL) { return NULL; }
			if (currentInterval == this->GetMostRecentInterval()) { return NULL; } // LAST interval doesn't have a next
			for (int i = 0; i < AI_CONST::maxIntervalsInHistory; i++) {
				if (this->GetInterval(i) == currentInterval) {
					int nextIndex = this->GetFixedRawIntervalIndex(i + 1);
					return this->GetInterval(nextIndex);
				}
			}
			return NULL; // Not found. Provided interval may not belong to this array !
		}


		// Get a list representing all intervals for a time frame
		std::list<T*> GetIntervalsForPeriod(long int startGameTime, long int endGameTime) {
			std::list<T*> result;
			if (endGameTime <= startGameTime) { return result; }
			CUSTOM_AI::TimeIntervalAttackRecord *interval = this->GetIntervalForGameTime(startGameTime);
			while (interval && (interval->intervalEndGameTime > endGameTime)) {
				result.push_back(interval);
				interval = this->GetNextInterval(interval);
			}
			return result;
		}

	private:
		T attacksHistory[AI_CONST::maxIntervalsInHistory]; // "circular" array.
		int indexOfMostRecentInterval; // current (last) index in array
		int validIntervalCount; // number of valid entries in array

		// Adds 1 new interval. Start time must correspond to most recent interval's end time (if there already is an interval)
		T *AddNewInterval(long int startGameTime) {
			if (this->validIntervalCount > 0) {
				if (this->GetInterval(this->indexOfMostRecentInterval)->intervalEndGameTime != startGameTime) {
					assert(false && "CustomAI: Non-contiguous intervals");
					startGameTime = this->GetInterval(this->indexOfMostRecentInterval)->intervalEndGameTime;
				}
			}
			if (this->validIntervalCount < AI_CONST::maxIntervalsInHistory) {
				// Just use a new (free) slot = next slot
				this->validIntervalCount++;
			} else {
				// Overwrite oldest interval
			}
			// Next slot = free slot OR oldest one
			this->indexOfMostRecentInterval = this->GetFixedRawIntervalIndex(this->indexOfMostRecentInterval + 1); // Next interval index
			T *newInterval = this->GetInterval(this->indexOfMostRecentInterval);
			newInterval->ResetAllInfo();
			newInterval->intervalStartGameTime = startGameTime;
			newInterval->intervalEndGameTime = startGameTime + AI_CONST::historyIntervalLength_ms;
			return newInterval;
		}

		// Add missing intervals if current game time belongs to an interval that is not contiguous to last known one
		void AddMissingIntervals(long int currentGameTime) {
			assert(currentGameTime >= 0);
			if (this->validIntervalCount <= 0) {
				this->InitFirstInterval(currentGameTime);
				return;
			}
			T *lastInterval = this->GetMostRecentInterval();
			long int diff = currentGameTime - lastInterval->intervalEndGameTime;
			if (diff <= 0) {
				return; // ERROR: currentGameTime is NOT after our last interval
			}
			// Maximum end time if using adding intervals in all slots (starting from "most recent one") = if we keep only the latest slot
			long int maxReachableTime = lastInterval->intervalEndGameTime + ((AI_CONST::maxIntervalsInHistory - 1) * AI_CONST::historyIntervalLength_ms);
			if (currentGameTime > maxReachableTime) {
				// Game time is beyond what we can reach. Which means we need to create new intervals on all slots, all intervals being "empty" (nothing happened)
				this->InitFirstInterval(currentGameTime);
				long int currentIntervalEndTime = currentGameTime + AI_CONST::historyIntervalLength_ms;
				for (int i = 1; i < AI_CONST::maxIntervalsInHistory; i++) { // loop: ignoring first one (already created by InitFirstInterval) 
					int curIndex = this->indexOfMostRecentInterval - i; // Go backwards
					curIndex = this->GetFixedRawIntervalIndex(curIndex);
					currentIntervalEndTime -= AI_CONST::historyIntervalLength_ms;
					T *curInterval = this->GetInterval(curIndex);
					this->validIntervalCount++;
					curInterval->ResetAllInfo();
					curInterval->intervalEndGameTime = currentIntervalEndTime;
					curInterval->intervalStartGameTime = curInterval->intervalEndGameTime - AI_CONST::historyIntervalLength_ms;
				}
				assert(this->validIntervalCount == AI_CONST::maxIntervalsInHistory);
				return;
			}
			// Otherwise: we can add intervals until we reach the correct interval
			long int currentIntervalEndTime = lastInterval->intervalEndGameTime;
			for (int i = 0; i < AI_CONST::maxIntervalsInHistory; i++) {
				T *curInterval = this->AddNewInterval(currentIntervalEndTime);
				currentIntervalEndTime = curInterval->intervalEndGameTime;
				if (currentIntervalEndTime >= currentGameTime) {
					return; // the new interval contains current game time
				}
			}
			traceMessageHandler.WriteMessage("ERROR: could not insert missing intervals in history");
		}

		// Get fixed "raw" index to fit in array (and avoid overflow). Result is always in [0, n-1]
		inline int GetFixedRawIntervalIndex(int index) const {
			index = index % AI_CONST::maxIntervalsInHistory;
			if (index < 0) {
				index += AI_CONST::maxIntervalsInHistory;
			}
			return index;
		}
		// Securely get an interval provided its "raw" index (should be in [0,n-1]).
		// This does not check if the interval is used/valid: it just gets it in the array.
		T *GetInterval(int rawIndex) {
			assert((rawIndex >= 0) && (rawIndex < AI_CONST::maxIntervalsInHistory));
			int i = GetFixedRawIntervalIndex(rawIndex);
			return &this->attacksHistory[i];
		}
		// Creates the first interval with correct game time. This flushes all previous information !
		void InitFirstInterval(long int currentGameTime) {
			this->ResetAllInfo();
			T *interval = this->AddNewInterval(currentGameTime);
		}

	};

	
}

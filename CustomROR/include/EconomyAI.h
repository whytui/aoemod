#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include "CustomRORInfo.h"
#include "AOEPrimitives_units.h"
#include "unitHandling.h"


using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	namespace AI_CONST {
		const unsigned long int delayForVillagerFixes_ms = 3000; // Interval in milliseconds between 2 executions of villager fixes (stuck villagers, etc)
	}


	// Stores information about military topics for custom AI (for a specific player)
	class EconomyAI {
	public:
		EconomyAI() {
			this->ResetAllInfo();
		}

		long int lastVillagersFix_ms; // Last game time (milliseconds) when fix on villagers were run

		void ResetAllInfo();

		// Run various fixes on villager actions, provided that a sufficient delay has passed since last execution
		void RunFixesOnVillagerActions(AOE_STRUCTURES::STRUCT_PLAYER *player, long int currentGameTime);

	private:
		// Remove farmers when more than 1 are assigned to the same farm.
		void CheckForDuplicateFarmers(AOE_STRUCTURES::STRUCT_PLAYER *player);

		// Fix repairment that are stuck in idle status because they got blocked at some point in their movement.
		void FixStuckRepairmen(AOE_STRUCTURES::STRUCT_PLAYER *player);

	};
	
}

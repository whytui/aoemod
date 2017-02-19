#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include "RockNRorInfo.h"
#include "AOEPrimitives_units.h"
#include "unitHandling.h"
#include "AOEPrimitives_UI_gameMain.h"


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
		void RunFixesOnVillagerActions(STRUCT_PLAYER *player, long int currentGameTime);

		// Called when "player" kills a gaia animal
		void OnGaiaAnimalKilled(STRUCT_PLAYER *player, STRUCT_UNIT_ATTACKABLE *killedAnimal);

	private:
		// Remove farmers when more than 1 are assigned to the same farm.
		void CheckForDuplicateFarmers(STRUCT_PLAYER *player);

		// Fix repairment that are stuck in idle status because they got blocked at some point in their movement.
		void FixStuckRepairmen(STRUCT_PLAYER *player);

	};
	
}
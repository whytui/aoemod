#pragma once

#include <assert.h>
#include <AOE_struct_units.h>
#include <AOE_struct_player.h>
#include "RockNRorInfo.h"
#include "AOEPrimitives_units.h"
#include "unitHandling.h"
#include "AOEPrimitives_UI_gameMain.h"
#include "AOE_strategy.h"
#include "combatAI.h"


using namespace AOE_STRUCTURES;

namespace CUSTOM_AI {

	namespace AI_CONST {
		const unsigned long int delayForVillagerFixes_ms = 3000; // Interval in milliseconds between 2 executions of villager fixes (stuck villagers, etc)
		const float maxAllowedPredatorAnimalSpeedForHunting = 1.1f; // Maximum animal speed (including "attack move" speed) AI villagers are allowed to hunt deliberately.
		const long int minimumVillagersToAllowExplorersInLateGame_RM = 7;
		const long int minimumVillagersToAllowExplorersInLateGame_DM = 4;
		const unsigned long int delayForExplorationLateGame_ms = 120000; // Milliseconds after which we are no longer in "game start"
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

		// Returns true if unit can be targeted as a resource by AI players
		// At infAI level, role = "is important resource"
		static bool IsAITargetableResource(STRUCT_UNIT_BASE *unit);

		// Returns true if unit is a flag/artefact (for AI). Original method=0x4BE200
		// At infAI level, role = "is important misc"
		static bool IsArtefactOrFlag(GLOBAL_UNIT_AI_TYPES unitClass);

		// Returns true if unit is one of (artefact OR targetable resource OR creatable unit). For AI.
		// Original method= 0x4BE100, only depends on unit class.
		// Game role = "is important object" at infAI level
		static bool IsArtefactOrTargetableResourceOrCreatable(STRUCT_UNIT_BASE *unit);

		// Override the ROR method (0x4D1BB5) that executes current tacAI task update (cf AI_UPDATE_TYPES)
		// Returns true if some specific treatments have been executed here = do NOT execute ROR standard treatment for current task
		// Returns false otherwise (default) = let ROR execute normally its treatments for current task
		// Warning *** this is supposed to take care of processing time, be carful about performance ***
		static bool RunOneTacAIUpdateTask(STRUCT_TAC_AI *tacAI, long int startProcessingTime, long int allowedProcessingTime);

		// Override the ROR method that calculates desired number of explorers, and then desired number of non-explorers (cf call to 0x4D9BE0)
		// Returns true if treatments have been run here and if ROR treatments MUST NOT be run (prevent ROR from calculating number of villagers by task)
		// Returns false if nothing special has been done = default case (let ROR treatments happen)
		// Warning *** this is supposed to take care of processing time, be carful about performance ***
		static bool CalcVillagerCountByTask(STRUCT_TAC_AI *tacAI);

		// Override the ROR method that handles insertions for first granary, first SP, auto build farms/towers, specific build item from SN numbers
		// Returns true if ROR treatments must be skipped, false if ROR treatments must be executed normally
		static bool UpdateStrategyAutoBuildInsertions(STRUCT_TAC_AI *tacAI);

		// Handles the specific build item from SN numbers (SNSpecificBuildItemToBuild)
		static void HandleSNSpecificBuildItem(STRUCT_TAC_AI *tacAI);

		// Checks if some repairations should be triggered
		// Returns true if ROR treatments must be skipped, false if ROR treatments must be executed normally
		static bool HandleNewRepairmen(STRUCT_TAC_AI *tacAI);

		// Returns true if ROR treatments must be skipped, false if ROR treatments must be executed normally
		static bool AiUpdateFoodDropSite(STRUCT_TAC_AI *tacAI);
	private:
		// Remove farmers when more than 1 are assigned to the same farm.
		void CheckForDuplicateFarmers(STRUCT_PLAYER *player);

		// Fix repairment that are stuck in idle status because they got blocked at some point in their movement.
		void FixStuckRepairmen(STRUCT_PLAYER *player);

	};
	
}

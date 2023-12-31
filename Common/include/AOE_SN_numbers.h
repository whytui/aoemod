
#pragma once


namespace AOE_CONST_FUNC {

	const static int CST_FIRST_SN_NUMBER = 0;
	const static int CST_LAST_SN_NUMBER = 225;
	const static int CST_SN_NUMBER_COUNT = (CST_LAST_SN_NUMBER - CST_FIRST_SN_NUMBER + 1);
	enum SN_NUMBERS {
		SNPercentCivilianExplorers = 0,
		SNPercentCivilianBuilders = 1,
		SNPercentCivilianGatherers = 2,
		SNCapCivilianExplorers = 3,
		SNCapCivilianBuilders = 4,
		SNCapCivilianGatherers = 5,
		SNMinimumAttackGroupSize = 16,
		SNTotalNumberExplorers = 18,
		SNPercentEnemySightedResponse = 19,
		SNEnemySightedResponseDistance = 20, // Sets the distance inside of which units will be candidates for response to an enemy attack. Must be >= 0 and <= 144
		SNSentryDistance = 22, // Sets the distance at which the town is defended. Must be >= 0
		SNArtifactReturnDistance = 23,
		SNPercentVictoryClamp = 24,
		SNMinimumDefendGroupSize = 25,
		SNMaximumAttackGroupSize = 26,
		SNMaximumDefendGroupSize = 28,
		SNMinimumPeaceLikeLevel = 29,
		SNPercentHealthRetreat = 30,
		SNPercentDeathRetreat = 31,
		SNPercentExplorationRequired = 32,
		SNZeroPriorityDistance = 34,
		SNMinimumCivilianExplorers = 35,
		SNNumberAttackGroups = 36,
		SNAttackGroupMakeup = 37,
		SNNumberDefendGroups = 38,
		SNDefendGroupMakeup = 39,
		SNGroupFillMethod = 40,
		SNAttackGroupGatherSpacing = 41,
		SNNumberExploreGroups = 42,
		SNMinimumExploreGroupSize = 43,
		SNMaximumExploreGroupSize = 44,
		SNExploreGroupMakeup = 45,
		SNAttackSeparationTime = 46,
		SNAttackCoordination = 47,
		SNAttackResponseSeparationTime = 48,
		SNRetreatAfterTargetDestroyed = 49,
		SNGoldDefendPriority = 50, // If >0, gold mines are added in "units to defend" list in infAI.
		SNStoneDefendPriority = 51, // If >0, stone mines are added in "units to defend" list in infAI.
		SNForageDefendPriority = 52, // If >0, berry bushes are added in "units to defend" list in infAI.
		SNChokePointDefendPriority = 53,
		SNRuinsDefendPriority = 54, // If >0, ruins are added in "units to defend" list in infAI.
		SNArtifactDefendPriority = 55, // If >0, relics are added in "units to defend" list in infAI.
		SNTownDefendPriority = 56, // If >0, Town center is added in "units to defend" list in infAI.
		SNDefenseDistance = 57, // Sets the distance at which items (town excluded) are defended.  Must be >= 0
		SNNumberBoatAttackGroups = 58,
		SNMinimumBoatAttackGroupSize = 59,
		SNMaximumBoatAttackGroupSize = 60,
		SNNumberBoatExploreGroups = 61,
		SNMinimumBoatExploreGroupSize = 62,
		SNMaximumBoatExploreGroupSize = 63,
		SNDesiredNumberTradeEscorts = 64,
		SNDesiredNumberFishEscorts = 65,
		SNDesiredNumberTransportEscorts = 66,
		SNNumberBoatDefendGroups = 67,
		SNMinimumBoatDefendGroupSize = 68,
		SNMaximumBoatDefendGroupSize = 69,
		SNDockDefendPriority = 70, // If >0, Docks are added in "units to defend" list in infAI.
		SNLockAttackAndAttackResponse = 71,
		SNSentryDistanceVariation = 72,
		SNMinimumTownSize = 73,
		SNMaximumTownSize = 74,
		SNGroupCommanderSelectionMethod = 75,
		SNConsecutiveIdleUnitLimit = 76,
		SNTargetEvaluationDistance = 77,
		SNTargetEvaluationHitpoints = 78,
		SNTargetEvaluationDamageCapability = 79,
		SNTargetEvaluationKills = 80,
		SNTargetEvaluationAllyProximity = 81,
		SNTargetEvaluationROF = 82,
		SNTargetEvaluationRandomness = 83,
		SNNumberWallGates = 84,
		SNSizeWallGates = 85,
		SNStoragePitMaxDistance = 86,
		SNGranaryMaxDistance = 87,
		SNTacticalUpdateFrequency = 88, // Sets the number of seconds that pass between each tactical AI update. Must be >= 0. Default 3 seconds ?
		SNTargetEvaluationAttackAttempts = 89, // Sets the multiplier used for the attack attempts rating in computer player target evaluation. Must be >= 0. 
		SNTargetEvaluationRange = 90, // Sets the multiplier used for the range rating in computer player target evaluation. Must be >= 0. 
		SNPercentUnitHealthRetreat = 91,
		SNDefendOverlapDistance = 92,
		SNScaleMinimumAttackGroupSize = 93,
		SNScaleMaximumAttackGroupSize = 94,
		SNScalePercentHealthRetreat = 95,
		SNScalePercentDeathRetreat = 96,
		SNScalePercentUnitHealthRetreat = 97,
		SNAttackGroupSizeRandomness = 98,
		SNScalingFrequency = 99,
		SNMaximumGAIAAttackResponse = 100,
		SNBuildFrequency = 101,
		SNAttackSeparationTimeRandomness = 102,
		SNAttackIntelligence = 103,
		SNInitialAttackDelay = 104,
		SNSaveScenarioInformation = 105,
		SNSpecialAttackType1 = 106,
		SNSpecialAttackType2 = 107,
		SNSpecialAttackType3 = 108,
		SNSpecialAttackInfluence1 = 109,
		SNSpecialAttackInfluence2 = 110,
		SNSpecialAttackInfluence3 = 111,
		SNMinimumWaterBodySizeForDock = 112,
		SNSkipAges = 113,
		SNNumberBuildAttemptsBeforeSkip = 114,
		SNMaxSkipsPerAttempt = 115,
		SN_UNKNOWN116_SNTrainingRebuilds = 116, // Not sure
		SNFoodGathererPercentage = 117,
		SNGoldGathererPercentage = 118,
		SNStoneGathererPercentage = 119,
		SNWoodGathererPercentage = 120,
		SNDefendImportantGroupLeaders = 121,
		SNTargetEvaluationContinent = 122,
		SNTargetEvaluationSiegeWeapon = 123, // set >0 value to influence siege weapons to attack stationary targets ?
		SNTributeAmount = 124,
		SNTributeChatFrequency = 125,
		SNTributeChatRandomness = 126,
		SNTributeTimeout = 127,
		SNTributePlayer = 128,
		SNTributeSuccessOutcome = 129,
		SNTributeFailureOutcome = 130,
		SNGroupLeaderDefenseDistance = 131,
		SNTributePersistence = 132,
		SNTributeRevokeOnAttack = 133,
		SNInitialAttackDelayType = 134,
		SNBlotExplorationMap = 135, // or SNBlotExploredMap
		SNBlotSize = 136,
		SNTownWallPattern = 137, // unsure
		SNInitialFood = 138, // Bonus for AI players on initial resources
		SNInitialGold = 139, // Bonus for AI players on initial resources
		SNInitialStone = 140, // Bonus for AI players on initial resources
		SNInitialWood = 141, // Bonus for AI players on initial resources
		SNIntelligentGathering = 142,
		SNTaskUngroupedSoldiers = 143,
		SNTargetEvaluationBoat = 144,
		SNNumberEnemyObjectsRequired = 145,
		SNNumberMaxSkipCycles = 146,
		SNMostNeededResourceLookAhead = 147,
		SNRetaskGatherAmount = 148,
		SNMaxRetaskGatherAmount = 149,
		SNMaxStoragePits = 150,
		SNMaxGranaries = 151,
		SNHouseOverage = 152,
		//Unused ? = 153, // Not sure
		//Unused ? = 154, // Not sure
		SNBuildPlanDivisions = 155,
		SNBuildPlanFoodGathererPercentage = 156,
		SNBuildPlanGoldGathererPercentage = 157,
		SNBuildPlanStoneGathererPercentage = 158,
		SNBuildPlanWoodGathererPercentage = 159,
		SNMaxBuildPlanGathererPercentage = 160,
		SNRequiredFirstBuilding = 161, // 0=no restriction, 1=either granary or storage pit, 2=storage pit, 3=granary, 4=both
		SNUsePlayAttacking = 162,
		SNFoodDropsiteDistance = 163,
		SNWoodDropsiteDistance = 164,
		SNStoneDropsiteDistance = 165,
		SNGoldDropsiteDistance = 166,
		SNInitialExplorationRequired = 167,
		SNRandomPlacementFactor = 168, // A number that gets added into the placement of the computer player to randomize building placement (off of the calculated ideal).  Must be >= 0
		SNMinimumForestTiles = 169,
		SNMinimumFood = 170, // The minimum amount of food a computer player likes to keep on hand.  Must be >= 0.
		SNMinimumWood = 171, // The minimum amount of wood a computer player likes to keep on hand.  Must be >= 0.
		SNMinimumStone = 172,
		SNMinimumGold = 173,
		SNMaximumHousesBeforeDropsites = 174,
		SNSpecificBuildItemToBuild = 175, // a building "unitDefId" to add to strategy at "SNSpecificBuildItemTime". Set to -1 once it has been taken into account (0x4D7691).
		SNSpecificBuildItemTime = 176, // time in minutes when to add SNSpecificBuildItemToBuild to strategy
		SNUnskippableItemType = 177,
		SNAttackDiplomacyImpact = 178,
		SNExplorationRequiredBeforeExplorerCountReduction = 179,
		SNAutoBuildHouses = 180,
		SNUpgradeToToolAgeASAP = 181,
		SNUpgradeToBronzeAgeASAP = 182,
		SNUpgradeToIronAgeASAP = 183,
		SNTargetEvaluationTimeKillRatio = 184,
		SNTargetEvaluationInProgress = 185,
		SNCoopDemandTributeInterval = 186,
		SNCoopDemandTributeMaximum = 187,
		SNAttackWinningPlayer = 188, // Controls whether or not the computer player will attack the winning player (if there is more than one to choose from).  Must be 0 or 1. If 0, the weakest player is targetted !
		//? attack factor  ? ? ? ? = 189,
		SNMaximumFood = 190,
		SNMaximumWood = 191,
		SNMaximumStone = 192,
		SNMaximumGold = 193,
		SNCoopShareInformation = 194, // 0xC2. If set (1), when some AI players see an enemy unit, all allied AI players see it too (add to AI unit lists in infAI)
		SNAttackWinningPlayerFactor = 195,
		SNCoopShareAttacking = 196,
		SNCoopShareAttackingInterval = 197,
		SNPercentageExploreExterminators = 198,
		//? related to exploration / attack targeting  ? = 200, // SNPercentHalfExploration? // The percentage of map exploration that allows the computer player to task down to half the number of explorers.  Must be >= 0 ?
		SNTrackPlayerHistory = 201,
		SNMinimumDropsiteBuffer = 202,
		SNUseByTypeMaxGathering = 203,
		SNMinimumElephantHuntGroupSize = 204,
		SNAutoBuildDropsites = 205, // If set, AI player will automatically add storage pits and granaries to its strategy (recommended in random games)
		SNAutoBuildFarms = 206, // Warning: SNAutoBuildFarms only triggers farm construction in very late game time (once all strategy buildings are constructed), which actually won't occur in RM games unless using RockNRor AI improvements
		SNAutoBuildTowers = 207, // Cf EconomyAI::UpdateStrategyAutoBuildInsertions
		SNAutoBuildDocks = 208,
		SNAutoBuildFishingBoats = 209,
		SNAutoBuildTransports = 210,
		//? autobuild  ? = 211,
		SNDesiredNumberDocks = 212,
		SNDesiredNumberFishingBoats = 213,
		SNDesiredNumberTransports = 214,
		SNAllowDiplomacyChangeOnAllyAttack = 215,
		SNMinimumAmountForTrading = 216,
		SNAllowDiplomacyChangeOnTribute = 217,
		SNEasiestReactionPercentage = 218,
		SNEasierReactionPercentage = 219,
		SNMaxFarms = 220,
		SNHitsBeforeAllianceChange = 221,
		SNMaxTowers = 222,
		SNAutoBuildWarships = 223,
		SNDesiredNumberWarships = 224,
		SN_UNKNOWN225 // Last valid SN number ?
	};
}


#pragma once
#include "commonConfigEnums.h"

namespace CUSTOMROR {
	namespace CONFIG {

		// Configuration for farm automatic rebuilding
		class AutoRebuildFarmConfig {
		public:
			AutoRebuildFarmConfig() {
				this->enableAutoRebuildFarms = false; // Game default: disabled (not a standard feature)
				this->autoRebuildFarms_maxFarms = 4;
				this->autoRebuildFarms_maxFood = 3000;
				this->autoRebuildFarms_minWood = 200;
			}

			bool enableAutoRebuildFarms; // Enable the feature, rebuilding a farm also has other conditions...
			long int autoRebuildFarms_maxFarms; // Maximum number of farms that can be reached with "auto construction" of farms
			long int autoRebuildFarms_maxFood; // Maximum amount of food we allow before automatically rebuilding a farm.
			long int autoRebuildFarms_minWood; // Minimum amount of wood we require before automatically rebuilding a farm.

		};


	}
}

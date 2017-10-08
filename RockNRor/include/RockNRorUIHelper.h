
#pragma once

#include <assert.h>
#include <list>
#include <algorithm>
#include <macroUtil.h>
#include "RnrScreenBase.h"


namespace ROCKNROR {
	namespace UI {
		
		class RnrScreenBase;


		class RnrUIHelper {
		public:
			RnrUIHelper();
			~RnrUIHelper();

			// Find a RockNRor screen from its name. NULL if not found.
			// Result may not be currently-focused UI, or may even not be "opened" in ROR process
			RnrScreenBase *FindRnrScreen(const std::string &screenName) const;

			// Returns currently displayed RockNRor screen, if any
			RnrScreenBase *GetCurrentRnrScreen() const;

			// Closes and destroy a specific screen, if found
			bool DestroyOneScreen(RnrScreenBase *screenToDestroy);

			// Closes and destroys all remaining RockNRor screens
			void DestroyAllScreens();

			// Call this to indicate that a new RnrScreen has been created with operator "new"
			// This safely updates internal collection
			void NotifyNewRnrScreen(RnrScreenBase *newScreen);

			// Call this to indicate that instruction "delete screenToDestroy" is gonna be executed
			// This safely updates internal collection
			// Do not call this if you don't actually delete the object !
			void NotifyDeleteRnrScreen(RnrScreenBase *screenToDestroy);

		private:
			// All RockNRor screen objects. The actual UI may not be currently open in ROR process !
			std::list<RnrScreenBase*> rnrScreens;

			// Set to true when this class is currently performing screen deletion(s)
			// This sets rnrScreens collection to "Read Only" to prevent updates while being iterated.
			bool internalDeletionInProgress;
		};

	}
}


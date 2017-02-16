
#pragma once


class AutoAttackPolicy {
public:
	AutoAttackPolicy()  {
		this->SetDefaultValues();
	}
	AutoAttackPolicy(bool attackVillagers, bool attackMilitary, bool attackTowers, bool attackNonTowerBuildings, bool attackWalls) {
		this->attackMilitary = attackMilitary;
		this->attackNonTowerBuildings = attackNonTowerBuildings;
		this->attackTowers = attackTowers;
		this->attackVillagers = attackVillagers;
		this->attackWalls = attackWalls;
	}
	bool attackVillagers;
	bool attackMilitary; // All military units except towers
	bool attackTowers; // Towers are non-moving units with attack ability (some towers are living units, not buildings !)
	bool attackNonTowerBuildings; // All buildings but towers and walls
	bool attackWalls;

	void SetDefaultValues() {
		attackMilitary = true;
		attackNonTowerBuildings = true;
		attackTowers = true;
		attackVillagers = true;
		attackWalls = false;
	}

	// Set all flags to true or false according to parameter.
	void SetAllValues(bool value) {
		attackMilitary = value;
		attackNonTowerBuildings = value;
		attackTowers = value;
		attackVillagers = value;
		attackWalls = value;
	}

	// Returns true if other AutoAttackPolicy has the same settings
	bool Equals(const AutoAttackPolicy &other) const {
		return (other.attackMilitary == this->attackMilitary) &&
			(other.attackNonTowerBuildings == this->attackNonTowerBuildings) &&
			(other.attackTowers == this->attackTowers) &&
			(other.attackVillagers == this->attackVillagers) &&
			(other.attackWalls == this->attackWalls);
	}
};

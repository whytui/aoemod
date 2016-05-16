
#pragma once

enum AUTO_ATTACK_POLICIES {
	AAP_NOT_SET = 0,
	AAP_DEFAULT, // auto-attack everything but walls
	AAP_IGNORE_VILLAGERS, // auto-attack every unit except villagers (including non-war ships)
	AAP_IGNORE_BUILDINGS, // auto-attack every unit except non-tower buildings
	AAP_IGNORE_VILLAGERS_AND_BLDINGS, // auto-attack every unit except villagers, non-tower buildings
	AAP_IGNORE_ALL // never auto-attack any unit
};


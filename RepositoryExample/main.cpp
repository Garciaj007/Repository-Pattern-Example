
#include <string>
#include <cstdio>

#include "repository.h"

int main()
{
	repo::Consumables consumables;

	consumables.Add(repo::Consumable { "HealthPotion", 10, 0, 0 });
	consumables.Add(repo::Consumable { "ManaPotion", 0, 10, 0 });
	consumables.Add(repo::Consumable { "StaminaPotion", 0, 0, 10 });

	std::printf("Does this work?");
}
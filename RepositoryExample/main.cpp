
#include <string>
#include <cstdio>
#include <conio.h>

#include "consumables.h"

int main()
{
	Consumables consumables;

	if (consumables.Count() > 0)
	{
		printf("\nFound %u consumables!", consumables.Count());

		for (auto c : consumables.DANGEROUS_Expose())
		{
			printf("\nConsumable:");
			std::cout << Consumable::to_json(c).dump(4) << std::endl;
		}

		_getch(); // Pauses Application and waits for key input;
	}

	consumables.Add(Consumable { "HealthPotion", 10, 0, 0 });
	consumables.Add(Consumable { "ManaPotion", 0, 10, 0 });
	consumables.Add(Consumable { "StaminaPotion", 0, 0, 10 });
}
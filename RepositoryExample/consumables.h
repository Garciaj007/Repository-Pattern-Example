#pragma once

#include "repository.h"

struct Consumable
{
	std::string id = "";
	int healthGain = 0;
	int manaGain = 0;
	int staminaGain = 0;

	Consumable() = default;
	Consumable(const std::string& id, int healthGain, int manaGain, int staminaGain) :
		id(id),
		healthGain(healthGain),
		manaGain(manaGain),
		staminaGain(staminaGain)
	{
	}

	bool operator ==(Consumable other)
	{
		return this->id == other.id;
	}

	static Consumable from_json(nlohmann::json json)
	{
		Consumable c;
		json.at("id").get_to(c.id);
		json.at("healthGain").get_to(c.healthGain);
		json.at("manaGain").get_to(c.manaGain);
		json.at("staminaGain").get_to(c.staminaGain);
		return c;
	}

	static nlohmann::json to_json(Consumable& c)
	{
		return nlohmann::json{
			{"id", c.id},
			{"healthGain", c.healthGain},
			{"manaGain", c.manaGain},
			{"staminaGain", c.staminaGain}
		};
	}
};

struct Consumables : repo::JSONRepository<Consumable>
{
	Consumables() : repo::JSONRepository<Consumable>("consumables.json") {}
};
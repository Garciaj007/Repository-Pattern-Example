#pragma once

#include <vector>
#include <string>
#include <optional>
#include <cstdio>
#include <memory>

#include <filesystem>
#include <fstream>
#include <iostream>

// JSON Library
#include <nlohmann/json.hpp>

namespace repo
{
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename... Args>
	constexpr Ref<T> MakeRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	/////////////////////////////////
	// Base Pattern
	///////////////////////////////

	template<typename T>
	class ADataContext
	{
	public:
		std::vector<T> dataContext = std::vector<T>();

		virtual void Load() {};
		virtual void Save() {};
	};

	template<typename T>
	class Repository
	{
		Ref<ADataContext<T>> context;
	public:
		Repository(Ref<ADataContext<T>> context) : context(context) { context->Load(); }
		virtual ~Repository() { context->Save(); }

		std::optional<T> GetById(std::string& id)
		{
			typename std::vector<T>::iterator found = std::find_if(begin(context->dataContext), end(context->dataContext), [&] (T& item) { return item.id == id; });
			if (found != end(context->dataContext))
				return *found;
			return {};
		}

		void Add(T entity)
		{
			std::optional<T> found = GetById(entity.id);
			if (found.has_value())
			{
				printf("Replacing Entity %s already in Collection", entity.id.c_str());
				std::remove(begin(context->dataContext), end(context->dataContext), found.value());
			}
			context->dataContext.push_back(entity);
		}

		void Update(T entity)
		{
			std::optional<T> found = GetById(entity.id);
			if (found.has_value())
				Remove(found.value());
			Add(entity);
		}

		void Remove(T entity)
		{
			std::optional<T> found = GetById(entity.id);
			if (found.has_value())
				std::remove(begin(context->dataContext), end(context->dataContext), found.value());
		}
	};

	/////////////////////////////////
	// JSON Implementation
	///////////////////////////////

	template<typename T>
	class JSONDataContext : public ADataContext<T>
	{
	public:
		std::string filename; // Always need to set the filename before Load is called;

		JSONDataContext(const std::string& filename) : ADataContext<T>(), filename(filename) {}

		void Load() override
		{
			this->dataContext.clear();

			const auto path = std::filesystem::path("./" + filename);
			if (!std::filesystem::exists(path))
			{
				printf("Path {} does not exist", path.c_str());
				return;
			}

			std::ifstream i(path);
			nlohmann::json jsonData;
			i >> jsonData;

			for (auto& element : jsonData ["dataContext"])
				this->dataContext.push_back(element.get<T>());

			i.close();
		}

		void Save() override
		{
			const auto path = std::filesystem::path("./" + filename);
			if (!std::filesystem::exists(path))
				printf("Create File at Path: %s", path.string().c_str());

			nlohmann::json jsonData;
			jsonData ["dataContext"] = this->dataContext;
			std::ofstream o(path);
			o << jsonData;
			o.close();
		}
	};

	template<typename T>
	struct JSONRepository : Repository<T>
	{
		JSONRepository(const std::string& filename) :Repository<T>(MakeRef<JSONDataContext<T>>(filename)) {}
	};

	struct Comparable
	{
		std::string id;

		Comparable(const std::string& id) : id(id) {}
	};

	static bool operator ==(Comparable& lhs, Comparable& rhs)
	{
		return lhs.id == rhs.id;
	}

	static bool operator !=(Comparable& lhs, Comparable& rhs)
	{
		return !(lhs == rhs);
	}

	struct Consumable
	{
		std::string id = "";
		int healthGain = 0;
		int manaGain = 0;
		int staminaGain = 0;

		Consumable(const std::string& id, int healthGain, int manaGain, int staminaGain) :
			id(id),
			healthGain(healthGain),
			manaGain(manaGain),
			staminaGain(staminaGain)
		{
		}

		bool operator ==(Consumable& other)
		{
			return this->id == other.id;
		}
	};

	void to_json(nlohmann::json& j, const Consumable& c)
	{
		j = nlohmann::json { {"id", c.id}, {"healthGain", c.healthGain}, {"manaGain", c.manaGain}, {"staminaGain", c.staminaGain} };
	}

	void from_json(const nlohmann::json& j, Consumable& c)
	{
		j.at("id").get_to(c.id);
		j.at("healthGain").get_to(c.healthGain);
		j.at("manaGain").get_to(c.id);
		j.at("id").get_to(c.id);
	}

	struct Consumables : JSONRepository<Consumable>
	{
		Consumables() : JSONRepository<Consumable>("consumables.json") {}
	};
}
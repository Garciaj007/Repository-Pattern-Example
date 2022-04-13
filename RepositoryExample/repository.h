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

		const std::vector<T>& DANGEROUS_Expose() { return context->dataContext; }

		std::optional<T> GetById(std::string& id)
		{
			typename std::vector<T>::iterator found = std::find_if(begin(context->dataContext), end(context->dataContext), [&](T& item) { return item.id == id; });
			if (found != end(context->dataContext))
				return *found;
			return {};
		}

		void Add(T entity)
		{
			std::optional<T> found = GetById(entity.id);
			if (found.has_value())
			{
				printf("\nReplacing Entity %s already in Collection", entity.id.c_str());
				Remove(found.value());
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
			auto it = std::find(context->dataContext.begin(), context->dataContext.end(), entity);
			if (it != context->dataContext.end())
				context->dataContext.erase(it);
		}

		int Count() { return context->dataContext.size(); }
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
				printf("\nPath %s does not exist", std::filesystem::absolute(path).c_str());
				return;
			}

			std::ifstream i(path);
			nlohmann::json jsonData;
			i >> jsonData;

			for (auto& element : jsonData["dataContext"])
				this->dataContext.push_back(T::from_json(element));

			i.close();
		}

		void Save() override
		{
			const auto path = std::filesystem::path("./" + filename);
			if (!std::filesystem::exists(path))
				printf("\nCreate File at Path: %s", path.string().c_str());

			nlohmann::json jsonData;
			for (auto& element : this->dataContext)
				jsonData["dataContext"].push_back(T::to_json(element));
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
}
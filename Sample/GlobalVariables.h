#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <variant>

#include "Math/MathUtils.h"

class GlobalVariables {
public:
	static constexpr char kDirectoryPath[] = "Resources/GlobalVariables/";

	class Item {
		friend class GlobalVariables;

	public:
		Item& operator=(int32_t value);
		Item& operator=(float value);
		Item& operator=(const Vector3& value);

		template<class T> T Get() const { return std::get<T>(value_); }
		template<class T> bool IsType() const { return std::holds_alternative<T>(value_); }

	private:
		void Edit(const std::string& name);
		std::variant<int32_t, float, Vector3> value_;
	};

	class Group {
		friend class GlobalVariables;

	public:
		Item& operator[](const std::string& key);

		bool HasItem(const std::string& key) const;

	private:
		void Edit(const std::string& name);
		bool Save(const std::string& name);
		std::map<std::string, Item> items_;
	};

	static GlobalVariables* GetInstance();

	Group& operator[](const std::string& groupName);

	bool HasGroup(const std::string& groupName) const;
	bool HasItem(const std::string& groupName, const std::string& itemName) const;

	void LoadFiles();
	void Update();

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables&) = delete;
	const GlobalVariables& operator=(const GlobalVariables&) = delete;

	bool LoadFile(const std::string& name);

	std::map<std::string, Group> datas_;
};

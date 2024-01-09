#include "GlobalVariables.h"

#include <filesystem>
#include <format>
#include <fstream>

#include "Externals/nlohmann/json.hpp"

#include "Graphics/ImGuiManager.h"

using json = nlohmann::json;

GlobalVariables* GlobalVariables::GetInstance() {
	static GlobalVariables instance;
	return &instance;
}

GlobalVariables::Group& GlobalVariables::operator[](const std::string& groupName) {
	return datas_[groupName];
}

bool GlobalVariables::HasGroup(const std::string& groupName) const {
	return datas_.contains(groupName);
}

bool GlobalVariables::HasItem(const std::string& groupName, const std::string& itemName) const {
	assert(HasGroup(groupName));
	auto& group = datas_.at(groupName);
	return group.HasItem(itemName);	
}

void GlobalVariables::LoadFiles() {
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(dir)) {
		return;
	}

	std::filesystem::directory_iterator dirIter(dir);
	for (auto& entry : dirIter) {
		auto& path = entry.path();
		auto extension = path.extension().string();
		if (extension != ".json") {
			continue;
		}
		auto groupName = path.stem().string();
		LoadFile(groupName);
	}
}

void GlobalVariables::Update() {
	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	if (!ImGui::BeginMenuBar()) {
		return;
	}

	for (auto& groupIter : datas_) {
		groupIter.second.Edit(groupIter.first);
	}

	ImGui::Spacing();

	ImGui::EndMenuBar();
	ImGui::End();
}

bool GlobalVariables::LoadFile(const std::string& name) {
	std::string filepath = kDirectoryPath + name + ".json";
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::string message = "Failed open data file for read.";
		MessageBoxA(nullptr, message.c_str(), "Global Variables", S_OK);
		return false;
	}

	json root;
	file >> root;
	file.close();

	json::iterator groupIter = root.find(name);
	if (groupIter == root.end()) {
		std::string message = "Group object not found.";
		MessageBoxA(nullptr, message.c_str(), "Global Variables", S_OK);
		return false;
	}

	for (auto itemIter = groupIter->begin(); itemIter != groupIter->end(); ++itemIter) {
		const std::string& itemName = itemIter.key();
		
		if (itemIter->is_number_integer()) {
			int32_t value = itemIter->get<int32_t>();
			(*this)[name][itemName] = value;
		} else if (itemIter->is_number_float()) {
			double value = itemIter->get<double>();
			(*this)[name][itemName] = static_cast<float>(value);
		} else if (itemIter->is_array() && itemIter->size() == 3) {
			Vector3 value = {itemIter->at(0), itemIter->at(1), itemIter->at(2)};
			(*this)[name][itemName] = value;
		}
	}

	return true;
}

GlobalVariables::Item& GlobalVariables::Group::operator[](const std::string& key) {
	return items_[key];
}

bool GlobalVariables::Group::HasItem(const std::string& key) const { return items_.contains(key); }

void GlobalVariables::Group::Edit(const std::string& name) {
	if (!ImGui::BeginMenu(name.c_str())) {
		return;
	}
	for (auto& itemIter : items_) {
		itemIter.second.Edit(itemIter.first);
	}
	ImGui::NewLine();
	if (ImGui::Button("Save")) {
		if (Save(name)) {
			std::string message = std::format("{}.json saved", name);
			MessageBoxA(nullptr, message.c_str(), "Global Variables", S_OK);
		}
	}
	ImGui::EndMenu();
}

bool GlobalVariables::Group::Save(const std::string& name) {
	nlohmann::json root;
	root[name] = nlohmann::json::object();

	for (auto& itemIter : items_) {
		auto& itemName = itemIter.first;
		const auto& item = itemIter.second;
		if (std::holds_alternative<int32_t>(item.value_)) {
			root[name][itemName] = std::get<int32_t>(item.value_);
		} else if (std::holds_alternative<float>(item.value_)) {
			root[name][itemName] = std::get<float>(item.value_);
		} else if (std::holds_alternative<Vector3>(item.value_)) {
			auto& value = std::get<Vector3>(item.value_);
			root[name][itemName] = {value.x, value.y, value.z};
		}
	}

	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directories(dir);
	}

	std::filesystem::path filepath(kDirectoryPath + (name + ".json"));
	std::ofstream file(filepath);
	if (!file.is_open()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "Global Variables", S_OK);
		return false;
	}
	file << std::setw(4) << root << std::endl;
	file.close();
	return true;
}

GlobalVariables::Item& GlobalVariables::Item::operator=(int32_t value) {
	value_ = value;
	return *this;
}

GlobalVariables::Item& GlobalVariables::Item::operator=(float value) {
	value_ = value;
	return *this;
}

GlobalVariables::Item& GlobalVariables::Item::operator=(const Vector3& value) {
	value_ = value;
	return *this;
}

void GlobalVariables::Item::Edit(const std::string& name) {
	if (std::holds_alternative<int32_t>(value_)) {
		int32_t* ptr = std::get_if<int32_t>(&value_);
		ImGui::DragInt(name.c_str(), ptr);
	} else if (std::holds_alternative<float>(value_)) {
		float* ptr = std::get_if<float>(&value_);
		ImGui::DragFloat(name.c_str(), ptr, 0.1f);
	} else if (std::holds_alternative<Vector3>(value_)) {
		Vector3* ptr = std::get_if<Vector3>(&value_);
		ImGui::DragFloat3(name.c_str(), reinterpret_cast<float*>(ptr), 0.1f);
	}
}

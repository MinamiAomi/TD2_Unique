#pragma once

#include <map>
#include <memory>
#include <string>

class ToonModel;
class Texture;

class ResourceManager {
public:
    static ResourceManager* GetInstance();

    void AddToonModel(const std::string& name, const std::shared_ptr<ToonModel>& model) { toonModelMap_.emplace(std::make_pair(name, model)); }
    std::shared_ptr<ToonModel> FindModel(const std::string& name) const { return toonModelMap_.at(name); }
    
    void AddTexture(const std::string& name, const std::shared_ptr<Texture>& texture) {
        textureMap_.emplace(std::make_pair(name, texture));
    }
    std::shared_ptr<Texture> FindTexture(const std::string& name) const { return textureMap_.at(name); }

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::map<std::string, std::shared_ptr<ToonModel>> toonModelMap_;
    std::map<std::string, std::shared_ptr<Texture>> textureMap_;
};
#pragma once

#include <map>
#include <memory>
#include <string>

class Model;
class Texture;

class ResourceManager {
public:
    static ResourceManager* GetInstance();

    void AddModel(const std::string& name, const std::shared_ptr<Model>& model) { modelMap_.emplace(std::make_pair(name, model)); }
    std::shared_ptr<Model> FindModel(const std::string& name) const { return modelMap_.at(name); }
    
    void AddTexture(const std::string& name, const std::shared_ptr<Texture>& texture) {
        textureMap_.emplace(std::make_pair(name, texture));
    }
    std::shared_ptr<Texture> FindTexture(const std::string& name) const { return textureMap_.at(name); }

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::map<std::string, std::shared_ptr<Model>> modelMap_;
    std::map<std::string, std::shared_ptr<Texture>> textureMap_;
};
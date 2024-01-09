#include "TextureLoader.h"

#include <map>

namespace TextureLoader {

    std::map<std::filesystem::path, std::shared_ptr<TextureResource>> g_map;

    std::shared_ptr<TextureResource> Load(const std::filesystem::path& path) {

        auto iter = g_map.find(path);
        if (iter != g_map.end()) {
            return iter->second;
        }

        std::shared_ptr<TextureResource> texture = std::make_shared<TextureResource>();
        texture->CreateFromWICFile(path.wstring());

        g_map.insert(std::make_pair(path, texture));
        return texture;
    }

    void Release(const std::filesystem::path& path) {
        g_map.erase(path);
    }

    void Release(const std::shared_ptr<TextureResource>& texture) {
        std::erase_if(g_map, [&](const auto& iter) { return iter.second == texture; });
    }

    void ReleaseAll() {
        g_map.clear();
    }

}

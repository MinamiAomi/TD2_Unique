#pragma once

#include <memory>
#include <vector>

#include "Core/GPUBuffer.h"
#include "Core/UploadBuffer.h"
#include "Core/TextureResource.h"

#include "ModelLoader.h"

class ToonModel {
    friend class ToonRenderer;
public:
    using Vertex = ModelData::Vertex;
    using Index = ModelData::Index;

    void Create(const ModelData& modelData);
    
private:
    struct Texture {
        TextureResource textureResource;
    };
    struct Material {
        UploadBuffer constantBuffer;
        std::shared_ptr<Texture> texture;
    };
    struct Mesh {
        UploadBuffer vertexBuffer;
        UploadBuffer indexBuffer;
        uint32_t indexCount;
        std::shared_ptr<Material> material;
    };

    std::vector<Mesh> meshes_;
};

class ToonModelInstance {
    friend class ToonRenderer;
public:
    static const std::list<ToonModelInstance*>& GetInstanceList() { return instanceLists_; }

    ToonModelInstance();
    virtual ~ToonModelInstance();

    void SetModel(const std::shared_ptr<ToonModel>& model) { model_ = model; }
    void SetWorldMatrix(const Matrix4x4& worldMatrix) { worldMatrix_ = worldMatrix; }
    void SetColor(const Vector3& color) { color_ = color; }
    void SetAlpha(float alpha) { alpha_ = alpha; }
    void SetUseLighting(bool useLighting) { useLighting_ = useLighting; }
    void SetIsActive(bool isActive) { isActive_ = isActive; }
    void SetOutlineWidth(float width) { outlineWidth_ = width; }
    void SetOutlineColor(const Vector3& color) { outlineColor_ = color; }
    void SetUseOutline(bool useOutline) { useOutline_ = useOutline; }

    bool IsActive() const { return isActive_; }

private:
    static std::list<ToonModelInstance*> instanceLists_;

    ToonModelInstance(const ToonModelInstance&) = delete;
    ToonModelInstance& operator=(const ToonModelInstance&) = delete;
    ToonModelInstance(ToonModelInstance&&) = delete;
    ToonModelInstance& operator=(ToonModelInstance&&) = delete;

    std::shared_ptr<ToonModel> model_;
    Matrix4x4 worldMatrix_;
    Vector3 color_ = Vector3::one;
    float alpha_ = 1.0f;
    float outlineWidth_ = {0.02f};
    Vector3 outlineColor_;
    bool useLighting_ = true;
    bool useOutline_ = true;
    bool isActive_ = true;
};
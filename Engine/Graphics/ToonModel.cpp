#include "ToonModel.h"

#include "Core/Graphics.h"
#include "Core/CommandContext.h"
#include "Core/SamplerManager.h"
#include "ToonRenderer.h"
#include "DefaultTextures.h"

void ToonModel::Create(const ModelData& modelData) {

    struct MaterialData {
        Vector3 diffuse;
        float pad;
        Vector3 specular;
    };

    std::vector<std::shared_ptr<Texture>> createdTextures;
    std::vector<std::shared_ptr<Material>> createdMaterials;

    CommandContext commandContext;
    commandContext.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);
    // テクスチャ
    for (auto& srcTexture : modelData.textures) {
        auto& destTexture = createdTextures.emplace_back(std::make_shared<Texture>());
        if (!srcTexture.filePath.empty()) {
            destTexture->textureResource.CreateFromWICFile(commandContext, srcTexture.filePath.wstring());
        }
    }
    // マテリアル
    for (auto& srcMaterial : modelData.materials) {
        auto& destMaterial = createdMaterials.emplace_back(std::make_shared<Material>());
        destMaterial->constantBuffer.CreateConstantBuffer(L"ToonModel ConstantBuffer", sizeof(MaterialData));
        MaterialData materialData{};
        materialData.diffuse = srcMaterial.diffuse;
        materialData.specular = srcMaterial.specular;
        destMaterial->constantBuffer.Copy(materialData);


        if (srcMaterial.textureIndex < createdTextures.size()) {
            destMaterial->texture = createdTextures[srcMaterial.textureIndex];
        }
    }
    // メッシュ
    for (auto& srcMesh : modelData.meshes) {

        // 生成するメッシュ
        auto& destMesh = meshes_.emplace_back();
        // 頂点バッファを作成
        destMesh.vertexBuffer.Create(L"ToonModel VertexBuffer", sizeof(srcMesh.vertices[0]), srcMesh.vertices.size());
        destMesh.vertexBuffer.Copy(srcMesh.vertices.data(), destMesh.vertexBuffer.GetBufferSize());
        // インデックスバッファを作成
        destMesh.indexCount = uint32_t(srcMesh.indices.size());
        destMesh.indexBuffer.Create(L"ToonModel indexBuffer", sizeof(srcMesh.indices[0]), srcMesh.indices.size());
        destMesh.indexBuffer.Copy(srcMesh.indices.data(), destMesh.indexBuffer.GetBufferSize());

        assert(srcMesh.materialIndex < createdMaterials.size());
        destMesh.material = createdMaterials[srcMesh.materialIndex];
        assert(destMesh.material);
    }
    commandContext.Finish(true);
}


std::list<ToonModelInstance*> ToonModelInstance::instanceLists_;

ToonModelInstance::ToonModelInstance() {
    instanceLists_.emplace_back(this);
}

ToonModelInstance::~ToonModelInstance() {
    auto iter = std::find(instanceLists_.begin(), instanceLists_.end(), this);
    if (iter != instanceLists_.end()) {
        instanceLists_.erase(iter);
    }
}

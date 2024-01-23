#include "Model.h"

#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Core/CommandContext.h"
#include "Core/TextureLoader.h"
#include "Mesh.h"
#include "Material.h"

namespace {
    std::vector<Mesh> ParseMeshes(const aiScene* scene, const std::vector<std::shared_ptr<Material>>& materials) {
        std::vector<Mesh> meshes(scene->mNumMeshes);

        for (uint32_t meshIndex = 0; auto & destMesh : meshes) {
            const aiMesh* srcMesh = scene->mMeshes[meshIndex];
            assert(srcMesh->HasNormals());

            destMesh.vertices.resize(srcMesh->mNumVertices);
            for (uint32_t vertexIndex = 0; auto & destVertex : destMesh.vertices) {
                aiVector3D& srcPosition = srcMesh->mVertices[vertexIndex];
                aiVector3D& srcNormal = srcMesh->mNormals[vertexIndex];
                // セット
                destVertex.position = { srcPosition.x, srcPosition.y, srcPosition.z };
                destVertex.normal = { srcNormal.x, srcNormal.y, srcNormal.z };
                if (srcMesh->HasTextureCoords(0)) {
                    aiVector3D& srcTexcoord = srcMesh->mTextureCoords[0][vertexIndex];
                    destVertex.texcood = { srcTexcoord.x, srcTexcoord.y };
                }
                else {
                    destVertex.texcood = Vector2::zero;
                }
                // 左手座標系に変換
                destVertex.position.x *= -1.0f;
                destVertex.normal.x *= -1.0f;

                vertexIndex++;
            }

            destMesh.indices.reserve(srcMesh->mNumFaces * 3);
            for (uint32_t faceIndex = 0; faceIndex < srcMesh->mNumFaces; ++faceIndex) {
                aiFace& srcFace = srcMesh->mFaces[faceIndex];
                assert(srcFace.mNumIndices == 3);
                destMesh.indices.emplace_back(srcFace.mIndices[0]);
                destMesh.indices.emplace_back(srcFace.mIndices[2]);
                destMesh.indices.emplace_back(srcFace.mIndices[1]);
            }

            // マテリアルが読み込まれてない
            assert(srcMesh->mMaterialIndex < materials.size());
            destMesh.material = materials[srcMesh->mMaterialIndex];

            ++meshIndex;
        }
        return meshes;
    }

    std::vector<std::shared_ptr<Material>> ParseMaterials(const aiScene* scene, const std::filesystem::path& directory) {
        std::vector<std::shared_ptr<Material>> materials(scene->mNumMaterials);

        for (uint32_t materialIndex = 0; auto & destMaterial : materials) {
            const aiMaterial* srcMaterial = scene->mMaterials[materialIndex];
            destMaterial = std::make_shared<Material>();

            aiColor3D diffuse{};
            if (srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == aiReturn_SUCCESS) {
                destMaterial->diffuse = { diffuse.r, diffuse.g, diffuse.b };
            }
            aiColor3D specular{};
            if (srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular) == aiReturn_SUCCESS) {
                destMaterial->specular = { specular.r, specular.g, specular.b };
            }
            aiColor3D ambient{};
            if (srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient) == aiReturn_SUCCESS) {
                destMaterial->ambient = { ambient.r, ambient.g, ambient.b };
            }
            float shininess{};
            if (srcMaterial->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
                destMaterial->shininess = shininess;
            }

            // テクスチャが一つ以上ある
            if (srcMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                aiString path;
                srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);
                // 読み込む
                // TextureLoader内で多重読み込み対応済み
                std::string filename(path.C_Str());
                destMaterial->diffuseMap = TextureLoader::Load(directory / filename);
            }
            ++materialIndex;
        }
        return materials;
    }

}

std::list<ModelInstance*> ModelInstance::instanceLists_;

std::shared_ptr<Model> Model::Load(const std::filesystem::path& path) {

    // privateコンストラクタをmake_sharedで呼ぶためのヘルパー
    struct Helper : Model {
        Helper() : Model() {}
    };
    std::shared_ptr<Model> model = std::make_shared<Helper>();

    auto directory = path.parent_path();

    Assimp::Importer importer;
    int flags = 0;
    // 三角形のみ
    flags |= aiProcess_Triangulate;
    // 左手座標系に変換
    flags |= aiProcess_FlipUVs;
    // 接空間を計算
    //flags |= aiProcess_CalcTangentSpace;
    flags |= aiProcess_GenNormals;
    const aiScene* scene = importer.ReadFile(path.string(), flags);
    // 読み込めた
    if (!scene) {
        OutputDebugStringA(importer.GetErrorString());
        assert(false);
    }
    assert(scene->HasMeshes());

    std::vector<std::shared_ptr<Material>> materials = ParseMaterials(scene, directory);
    model->meshes_ = ParseMeshes(scene, materials);

    // 中間リソースをコピーする
    CommandContext commandContext;
    commandContext.Start(D3D12_COMMAND_LIST_TYPE_DIRECT);

    for (auto& mesh : model->meshes_) {
        mesh.CreateBuffers(commandContext);
    }
    model->blas_.Create(L"ModelBLAS", commandContext, model->meshes_);

    commandContext.Finish(true);

    return model;
}

ModelInstance::ModelInstance() {
    instanceLists_.emplace_back(this);
}

ModelInstance::~ModelInstance() {
    std::erase(instanceLists_, this);
   // auto iter = std::find(instanceLists_.begin(), instanceLists_.end(), this);
   // if (iter != instanceLists_.end()) {
   //     instanceLists_.erase(iter);
   // }
}

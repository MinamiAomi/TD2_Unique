#pragma once

#include <memory>
#include <filesystem>

#include "Math/MathUtils.h"
#include "Core/GPUBuffer.h"
#include "Mesh.h"
#include "Raytracing/BLAS.h"

class Model {
public:
    static std::shared_ptr<Model> Load(const std::filesystem::path& path);

    const BLAS& GetBLAS() const { return blas_; }
    const std::vector<Mesh>& GetMeshes() const { return meshes_; }

private:
    Model() = default;
    ~Model() = default;

    std::vector<Mesh> meshes_;
    BLAS blas_;
};

class ModelInstance {
public:
    static const std::list<ModelInstance*>& GetInstanceList() { return instanceLists_; }

    ModelInstance();
    ~ModelInstance();

    void SetModel(const std::shared_ptr<Model>& model) { model_ = model; }
    void SetWorldMatrix(const Matrix4x4& worldMatrix) { worldMatrix_ = worldMatrix; }
    void SetColor(const Vector3& color) { color_ = color; }
    void SetAlpha(float alpha) { alpha_ = alpha; }
    void SetReciveShadow(bool reciveShadow) { reciveShadow_ = reciveShadow; }
    void SetCastShadow(bool castShadow) { castShadow_ = castShadow; }
    void SetReflection(bool reflection) { reflection_ = reflection; }
    void SetUseLighting(bool useLighting) { useLighting_ = useLighting; }
    void SetIsActive(bool isActive) { isActive_ = isActive; }

    const std::shared_ptr<Model>& GetModel() const { return model_; }
    const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
    const Vector3& GetColor() const { return color_; }
    float GetAlpha() const { return alpha_; }
    bool ReciveShadow() const { return reciveShadow_; }
    bool CastShadow() const { return castShadow_; }
    bool Reflection() const { return reflection_; }
    bool UseLighting() const { return useLighting_; }
    bool IsActive() const { return isActive_; }

private:
    static std::list<ModelInstance*> instanceLists_;

    std::shared_ptr<Model> model_;
    Matrix4x4 worldMatrix_;
    Vector3 color_ = Vector3::one;
    float alpha_ = 1.0f;

    bool reciveShadow_ = true;
    bool castShadow_ = true;
    bool reflection_ = false;
    bool useLighting_ = true;
    bool isActive_ = true;
};
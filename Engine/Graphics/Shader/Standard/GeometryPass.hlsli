
struct DescriptorIndex {
    uint scene;
    uint instance;
    
    uint material;
    
    uint baseColorMap;
    uint baseColorSampler;
    
    uint metallicRoughnessMap;
    uint metallicRoughnessSampler;
    
    uint normalMap;
    uint normalSampler;

#ifdef ENABLE_SKINNING
    uint bone;
#endif
};
ConstantBuffer<DescriptorIndex> descriptorIndex : register(b0);

struct Scene {
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float3 cameraPosition;
};

struct Instance {
    float4x4 worldMatrix;
    float4x4 worldInverseTransposeMatrix;
};

struct Material {
    float3 baseColorFactor;
    float2 metallicRoughnessFactor;
};

struct Bone {
    float4x4 boneMatrix;
    float4x4 boneInverseTransposeMatrix;
};
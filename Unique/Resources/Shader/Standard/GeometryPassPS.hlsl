#include "GeometryPass.hlsli"

struct PSInput {
    float4 svPosition : SV_POSITION;
    float3 worldPosition : POSITION0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float3 baseColor : SV_TARGET0;
    float2 metallicRoughness : SV_TARGET1;
    float3 normal : SV_TARGET2;
};

PSOutput main(PSInput input) {

    PSOutput output;
    
    // マテリアル定数
    ConstantBuffer<Material> material = ResourceDescriptorHeap[descriptorIndex.material];
    // ベースカラー
    Texture2D<float3> baseColorMap = ResourceDescriptorHeap[descriptorIndex.baseColorMap];
    SamplerState baseColorSampler = SamplerDescriptorHeap[descriptorIndex.baseColorSampler];
    // メタリックとラフネス
    Texture2D<float2> metallicRoughnessMap = ResourceDescriptorHeap[descriptorIndex.metallicRoughnessMap];
    SamplerState metallicRoughnessSampler = SamplerDescriptorHeap[descriptorIndex.metallicRoughnessSampler];
    // 法線 
    Texture2D<float3> normalMap = ResourceDescriptorHeap[descriptorIndex.normalMap];
    SamplerState normalSampler = SamplerDescriptorHeap[descriptorIndex.normalSampler];
    
    output.baseColor = material.baseColorFactor * baseColorMap.Sample(baseColorSampler, input.texcoord);
    output.metallicRoughness = material.metallicRoughnessFactor * metallicRoughnessMap.Sample(metallicRoughnessSampler, input.texcoord);
    
    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    float3 binormal = normalize(cross(normal, tangent));
    float3x3 tangentSpace = float3x3(tangent, binormal, normal);
    
    // UNORMからSNORMに変換
    normal = normalMap.Sample(normalSampler, input.texcoord) * 2.0f - 1.0f;
    // 接空間に変換
    normal = mul(normal, tangentSpace);
    // 0~1に収める
    output.normal = (normal + 1.0f) * 0.5f;
    
    
    return output;
}
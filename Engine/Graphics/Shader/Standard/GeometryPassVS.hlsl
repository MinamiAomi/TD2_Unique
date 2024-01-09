#include "GeometryPass.hlsli"


struct VSInput {
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float2 texcoord : TEXCOORD0;
#ifdef ENABLE_SKINNING
    uint4 boneIndices : BLENDINDICES0;
    float4 boneWeights : BLENDWEIGHT0;
#endif
};

struct VSOutput {
    float4 svPosition : SV_POSITION;
    float3 worldPosition : POSITION0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float2 texcoord : TEXCOORD0;
};

VSOutput main(VSInput input) {
    VSOutput output;
        
    float4 localPosition = float4(input.position, 1.0f);
    float3 localNormal = input.normal;
    float3 localTangent = input.tangent;
        
#ifdef ENABLE_SKINNING
    // スキニング計算
    StructuredBuffer<Bone> bones = ResourceDescriptorHeap[descriptorIndex.bone];

    float4x4 boneMatrix =
        mul(bones[input.boneIndices.x].boneMatrix, input.boneWeights.x) +
        mul(bones[input.boneIndices.y].boneMatrix, input.boneWeights.y) +
        mul(bones[input.boneIndices.z].boneMatrix, input.boneWeights.z) +
        mul(bones[input.boneIndices.w].boneMatrix, input.boneWeights.w);
    
    localPosition = mul(localPosition, boneMatrix);
    
    float4x4 boneMatrixInverseTranspose = 
        mul(bones[input.boneIndices.x].boneInverseTransposeMatrix, input.boneWeights.x) +
        mul(bones[input.boneIndices.y].boneInverseTransposeMatrix, input.boneWeights.y) +
        mul(bones[input.boneIndices.z].boneInverseTransposeMatrix, input.boneWeights.z) +
        mul(bones[input.boneIndices.w].boneInverseTransposeMatrix, input.boneWeights.w);
    
    localNormal = normalize(mul(localNormal, (float3x3) boneMatrixInverseTranspose));
    localTangent = normalize(mul(localTangent, (float3x3) boneMatrixInverseTranspose));
#endif
    
    ConstantBuffer<Scene> scene = ResourceDescriptorHeap[descriptorIndex.scene];
    ConstantBuffer<Instance> instance = ResourceDescriptorHeap[descriptorIndex.instance];
    
    float4 worldPosition = mul(localPosition, instance.worldMatrix);
    output.svPosition = mul(worldPosition, mul(scene.viewMatrix, scene.projectionMatrix));
    output.worldPosition = worldPosition.xyz;
    output.normal = mul(localNormal, (float3x3) instance.worldInverseTransposeMatrix);
    output.tangent = mul(localTangent, (float3x3) instance.worldInverseTransposeMatrix);
    output.texcoord = input.texcoord;
    
    return output;
}
#include "../Lighting.hlsli"

struct DescriptorIndex {
    uint scene;
    
    // GBuffers
    uint baseColor;
    uint metallicRoughness;
    uint normal;
    uint depth;
    // sampler
    uint gBufferSampler;
    
    // Lights
    uint directionalLights;
    uint pointLights;
    uint spotLights;
};
ConstantBuffer<DescriptorIndex> descriptorIndex : register(b0);

struct Scene {
    float4x4 viewProjectionInverseMatrix;
    float3 cameraPosition;
    uint numDirectionalLights;
    uint numPointLights;
    uint numSpotLights;
};

struct PSInput {
    float4 svPosition : SV_POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

// texcoodとdepthからワールド座標を計算
float3 GetWorldPosition(in float2 texcoord, in float depth, in float4x4 viewProjectionInverseMatrix) {
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 position = float4(xy, depth, 1.0f);
    position = mul(position, viewProjectionInverseMatrix);
    return position.xyz / position.w;
}

PSOutput main(PSInput input) {

    PSOutput output;

    // リソースを取ってくる
    // 定数バッファ
    ConstantBuffer<Scene> scene = ResourceDescriptorHeap[descriptorIndex.scene];
    // GBuffer
    Texture2D<float3> baseColorGBuffer = ResourceDescriptorHeap[descriptorIndex.baseColor];
    Texture2D<float2> metallicRoughnessGBuffer = ResourceDescriptorHeap[descriptorIndex.metallicRoughness];
    Texture2D<float3> normalGBuffer = ResourceDescriptorHeap[descriptorIndex.normal];
    Texture2D<float> depthGBuffer = ResourceDescriptorHeap[descriptorIndex.normal];
    // サンプラー
    SamplerState gBufferSampler = SamplerDescriptorHeap[descriptorIndex.gBufferSampler];  
    // ライト
    StructuredBuffer<Lighting::DirectionalLight> directionalLights = ResourceDescriptorHeap[descriptorIndex.directionalLights];
    StructuredBuffer<Lighting::PointLight> pointLights = ResourceDescriptorHeap[descriptorIndex.pointLights];
    StructuredBuffer<Lighting::SpotLight> spotLights = ResourceDescriptorHeap[descriptorIndex.spotLights];
    
    // GBufferから取ってくる
    float3 worldPosition = GetWorldPosition(input.texcoord, depthGBuffer.Sample(gBufferSampler, input.texcoord), scene.viewProjectionInverseMatrix);
    float3 baseColor = baseColorGBuffer.Sample(gBufferSampler, input.texcoord);
    float2 metallicRoughness = metallicRoughnessGBuffer.Sample(gBufferSampler, input.texcoord);
    float3 normal = normalGBuffer.Sample(gBufferSampler, input.texcoord) * 2.0f - 1.0f;
    
    // 計算する
    output.color.xyz = baseColor * saturate(dot(normal, -directionalLights[0].direction));
    output.color.a = 1.0f;
    
    return output;
}
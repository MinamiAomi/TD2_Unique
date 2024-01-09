struct DescriptorIndex {
    uint tlas;
    uint shadow;
    uint reflection;
};

ConstantBuffer<DescriptorIndex> g_descriptorIndex : register(b0);

struct Scene {
    float4x4 viewProjectionInverseMatrix;
    float3 sunLightDirection;
};
ConstantBuffer<Scene> g_scene : register(b1);

// 一次レイ用ペイロード
struct PrimaryPayload {
    uint shadow;
    float3 reflection;
    uint reflected;
};

struct Attributes {
    float2 barycentrics;
};

#define TRUE_UINT 1
#define FALSE_UINT 0

#define INVALID_COLOR float3(-1.0f, -1.0f, -1.0f)

// 一次レイヒットグループ
#define PRIMARY_HIT_GROUP_INDEX 0
// 影ヒットグループ
#define SHADOW_HIT_GROUP_INDEX 1

#define MISS_SHADER_INDEX 0

#define PRIMARY_RAY_ATTRIBUTE (1 << 0)
#define SHADOW_RAY_ATTRIBUTE  (1 << 1)


//////////////////////////////////////////////////


// texcoodとdepthからワールド座標を計算
float3 GetWorldPosition(in float2 texcoord, in float depth, in float4x4 viewProjectionInverseMatrix) {
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 position = float4(xy, depth, 1.0f);
    position = mul(position, viewProjectionInverseMatrix);
    position.xyz /= position.w;
    return position.xyz;
}

[shader("raygeneration")]
void RayGeneration() {
    // レイのインデックス
    uint2 dispatchRaysIndex = DispatchRaysIndex().xy;
    // レイの縦横数
    uint2 dispatchRaysDimensions = DispatchRaysDimensions().xy;
    // テクスチャ座標系を求める    
    float2 texcoord = ((float2) dispatchRaysIndex + 0.5f) / (float2) dispatchRaysDimensions;
    
    //ConstantBuffer<Scene> scene = ResourceDescriptorHeap[descriptorIndex.scene];
    // 近面から遠面へのレイ
    RayDesc rayDesc;
    float3 rayOrigin = GetWorldPosition(texcoord, 0.0f, g_scene.viewProjectionInverseMatrix);
    rayDesc.Origin = rayOrigin;
    float3 rayDiff = GetWorldPosition(texcoord, 1.0f, g_scene.viewProjectionInverseMatrix) - rayOrigin;
    rayDesc.Direction = normalize(rayDiff);
    rayDesc.TMin = 0.0f;
    rayDesc.TMax = length(rayDiff);
    // レイを飛ばす
    RaytracingAccelerationStructure tlas = ResourceDescriptorHeap[g_descriptorIndex.tlas];
    PrimaryPayload payload;
    payload.shadow = 0;
    payload.reflection = INVALID_COLOR;
    payload.reflected = FALSE_UINT;
    TraceRay(
        tlas, // RaytracingAccelerationStructure
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES, // RayFlags
        PRIMARY_RAY_ATTRIBUTE, // InstanceInclusionMask
        1, // RayContributionToHitGroupIndex
        1, // MultiplierForGeometryContributionToHitGroupIndex
        MISS_SHADER_INDEX, // MissShaderIndex
        rayDesc, // Ray
        payload); // Payload

    float shadow = lerp(1.0f, 0.5f, payload.shadow);
    RWTexture2D<float> shadowBuffer = ResourceDescriptorHeap[g_descriptorIndex.shadow];
    shadowBuffer[dispatchRaysIndex] = shadow;
    
    RWTexture2D<float4> reflectionBuffer = ResourceDescriptorHeap[g_descriptorIndex.reflection];
    reflectionBuffer[dispatchRaysIndex] = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (payload.reflected) {
        reflectionBuffer[dispatchRaysIndex].rgb = payload.reflection;
        reflectionBuffer[dispatchRaysIndex].a = 1.0f;
    }
}


//////////////////////////////////////////////////


[shader("miss")]
void Miss(inout PrimaryPayload payload) {
    payload.shadow = FALSE_UINT;
    if (payload.reflected == TRUE_UINT) {
        payload.reflection = float3(0.1f, 0.4f, 0.6f);
    }
}


//////////////////////////////////////////////////


struct Vertex {
    float3 position;
    float3 normal;
    float2 texcoord;
};

struct Material {
    float3 color;
    uint reflection;
};

StructuredBuffer<Vertex> g_vertexBuffer : register(t0, space1);
StructuredBuffer<uint> g_indexBuffer : register(t1, space1);
Texture2D<float4> g_texture : register(t2, space1);
SamplerState g_sampler : register(s0, space1);
ConstantBuffer<Material> g_material : register(b0, space1);

float3 CalcBarycentrics(float2 barycentrics) {
    return float3(1.0f - barycentrics.x - barycentrics.y, barycentrics.x, barycentrics.y);
}
    
Vertex GetVertex(Attributes attributes) {
    Vertex vertex = (Vertex) 0;
    float3 barycentrics = CalcBarycentrics(attributes.barycentrics);
    uint primitiveID = PrimitiveIndex() * 3;

    for (uint i = 0; i < 3; ++i) {
        uint index = g_indexBuffer[primitiveID + i];
        vertex.position += g_vertexBuffer[index].position * barycentrics[i];
        vertex.normal += g_vertexBuffer[index].normal * barycentrics[i];
        vertex.texcoord += g_vertexBuffer[index].texcoord * barycentrics[i];
    }
    vertex.normal = normalize(vertex.normal);
    vertex.position = mul(float4(vertex.position, 1.0f), ObjectToWorld4x3());
    vertex.normal = mul(vertex.normal, (float3x3) ObjectToWorld4x3());
    
    return vertex;
}
    
[shader("closesthit")]
void PrimaryRayClosestHit(inout PrimaryPayload payload, in Attributes attributes) {

    // レイの情報    
    float hitT = RayTCurrent();
    float3 rayOrigin = WorldRayOrigin();
    float3 rayDirection = WorldRayDirection();
    // 頂点を取得
    Vertex vertex = GetVertex(attributes);
    // 反射
    // 反射後 色付け    
    if (payload.reflected == TRUE_UINT) {
        payload.reflection = g_texture.SampleLevel(g_sampler, vertex.texcoord, 0).rgb * g_material.color;
        return;
    }
    // 反射前 レイ飛ばし
    if (g_material.reflection) {
        payload.reflected = TRUE_UINT;
        float3 reflectionRayDirection = reflect(rayDirection, vertex.normal);
        
        RayDesc reflectionRay;
        reflectionRay.Origin = vertex.position;
        reflectionRay.Direction = normalize(reflectionRayDirection);
        reflectionRay.TMin = 0.0001f;
        reflectionRay.TMax = 10000.0f;
        
        RaytracingAccelerationStructure tlas = ResourceDescriptorHeap[g_descriptorIndex.tlas];
        
        TraceRay(
                tlas, // RaytracingAccelerationStructure
                RAY_FLAG_CULL_BACK_FACING_TRIANGLES, // RayFlags
                PRIMARY_RAY_ATTRIBUTE, // InstanceInclusionMask
                1, // RayContributionToHitGroupIndex
                1, // MultiplierForGeometryContributionToHitGroupIndex
                MISS_SHADER_INDEX, // MissShaderIndex
                reflectionRay, // Ray
                payload); // Payload
    }
    
    //// InstanceIDが1なら影を受ける
    //    if (InstanceID() == 1) {
    //    // シャドウレイを飛ばす
    //        float hitT = RayTCurrent();
    //        float3 rayOrigin = WorldRayOrigin();
    //        float3 rayDirection = WorldRayDirection();
    //    // ヒットしたポジション
    //        float3 hitPosition = rayOrigin + (hitT - 0.001f) * rayDirection;
    
    //   // ConstantBuffer<Scene> scene = ResourceDescriptorHeap[descriptorIndex.scene];
    //    // 衝突点からライトへのレイ
    //        RayDesc rayDesc;
    //        rayDesc.Origin = hitPosition;
    //        rayDesc.Direction = -g_scene.sunLightDirection;
    //        rayDesc.TMin = 0.001f; // 少し浮かす
    //        rayDesc.TMax = 100000.0f; // 
        
    //        RaytracingAccelerationStructure tlas = ResourceDescriptorHeap[g_descriptorIndex.tlas];
    //        TraceRay(
    //    tlas,
    //    RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH,
    //    SHADOW_RAY_ATTRIBUTE,
    //    SHADOW_HIT_GROUP_INDEX,
    //    0,
    //    MISS_SHADER_INDEX,
    //    rayDesc,
    //    payload);
    //    }
    //    else {
    //    // 影を受けない
    //        payload.shadow = FALSE_UINT;
    //    }
}


//////////////////////////////////////////////////


[shader("closesthit")]
void ShadowRayClosestHit(inout PrimaryPayload payload, in BuiltInTriangleIntersectionAttributes attribs) {
    payload.shadow = TRUE_UINT;
}


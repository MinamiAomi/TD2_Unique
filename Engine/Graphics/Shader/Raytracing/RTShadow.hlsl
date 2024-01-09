struct Scene {
    float4x4 viewProjectionInverseMatrix;
    float3 sunLightDirection;
};

RaytracingAccelerationStructure g_reciveShadowTlas : register(t0);
RaytracingAccelerationStructure g_castShadowTlas : register(t1);
RWTexture2D<float> g_shadow : register(u0);
ConstantBuffer<Scene> g_scene : register(b0);

// ペイロード
struct Payload {
    uint shadow;
};

struct Attributes {
    float2 barycentrics;
};

#define TRUE_UINT 1
#define FALSE_UINT 0

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
    
    float3 nearPosition = GetWorldPosition(texcoord, 0.0f, g_scene.viewProjectionInverseMatrix);
    float3 farPosition = GetWorldPosition(texcoord, 1.0f, g_scene.viewProjectionInverseMatrix);
    float3 rayDiff = farPosition - nearPosition;
    
    // 近面から遠面へのレイ
    RayDesc rayDesc;
    rayDesc.Origin = nearPosition;
    rayDesc.Direction = normalize(rayDiff);
    rayDesc.TMin = 0.0f;
    rayDesc.TMax = length(rayDiff);
    // レイを飛ばす
    Payload payload;
    payload.shadow = 0;
    TraceRay(
        g_reciveShadowTlas,                  // RaytracingAccelerationStructure
        RAY_FLAG_CULL_BACK_FACING_TRIANGLES, // RayFlags
        0xFF,                                // InstanceInclusionMask
        1,                                   // RayContributionToHitGroupIndex
        1,                                   // MultiplierForGeometryContributionToHitGroupIndex
        0,                                   // MissShaderIndex
        rayDesc,                             // Ray
        payload);                            // Payload

    float shadow = lerp(1.0f, 0.5f, payload.shadow);
    g_shadow[dispatchRaysIndex] = shadow;
    
}


//////////////////////////////////////////////////


[shader("miss")]
void Miss(inout Payload payload) {
    payload.shadow = FALSE_UINT;
}


//////////////////////////////////////////////////


struct Vertex {
    float3 position;
    float3 normal;
    float2 texcoord;
};

StructuredBuffer<Vertex> g_vertexBuffer : register(t0, space1);
StructuredBuffer<uint> g_indexBuffer : register(t1, space1);

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
void PrimaryRayClosestHit(inout Payload payload, in Attributes attributes) {

    // InstanceIDが1なら影を受ける
    bool reciveShadow = (InstanceID() == 1);
    // 影を受けない
    if (!reciveShadow) {
        payload.shadow = FALSE_UINT;
        return;
    }
    
    // シャドウレイを飛ばす
    float hitT = RayTCurrent();
    float3 rayOrigin = WorldRayOrigin();
    float3 rayDirection = WorldRayDirection();
        // ヒットしたポジション
    float3 hitPosition = rayOrigin + (hitT - 0.001f) * rayDirection;
    
    // 衝突点からライトへのレイ
    RayDesc rayDesc;
    rayDesc.Origin = hitPosition;
    rayDesc.Direction = -g_scene.sunLightDirection;
    rayDesc.TMin = 0.001f; // 少し浮かす
    rayDesc.TMax = 100000.0f; // 
      
    TraceRay(
        g_castShadowTlas,                         // RaytracingAccelerationStructure
        RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH, // RayFlags
        0xFF,                                     // InstanceInclusionMask
        0,                                        // RayContributionToHitGroupIndex
        0,                                        // MultiplierForGeometryContributionToHitGroupIndex
        0,                                        // MissShaderIndex
        rayDesc,                                  // Ray
        payload);                                 // Payload
}


//////////////////////////////////////////////////


[shader("closesthit")]
void ShadowRayClosestHit(inout Payload payload, in Attributes attribs) {
    payload.shadow = TRUE_UINT;
}

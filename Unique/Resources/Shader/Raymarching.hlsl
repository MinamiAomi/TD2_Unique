#include "Lighting.hlsli"

#define MAX_STEPS 64
#define MAX_DISTANCE 100.0f
#define EPSILON 0.0001f

#define MIN_FOG_DEPTH 50.0f
#define MAX_FOG_DEPTH 100.0f

#define REPETITION(x, y) (x - floor(x / y) * y - y * 0.5f)

struct Scene {
    float4x4 viewProjectionInverse;
    float3 cameraPosition;
    float time;
};
ConstantBuffer<Scene> g_Scene : register(b0);
RWTexture2D<float4> g_Output : register(u0);

// texcoodとdepthからワールド座標を計算
float3 GetWorldPosition(in float2 texcoord, in float depth, in float4x4 viewProjectionInverseMatrix) {
    // xは0~1から-1~1, yは0~1から1~-1に上下反転
    float2 xy = texcoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
    float4 position = float4(xy, depth, 1.0f);
    position = mul(position, viewProjectionInverseMatrix);
    position.xyz /= position.w;
    return position.xyz;
}

float Random(in float2 seed) {
    return frac(sin(dot(seed, float2(12.9898f, 78.233f))) * 43758.5453f);

}

float sdSphere(in float3 p, in float r) {
    return length(p) - r;
}

float sdBox(in float3 p, in float3 b) {
    float3 d = abs(p) - b;
    return min(max(d.x, max(d.y, d.z)), 0.0f) + length(max(d, 0.0f));
}

float GetDistance(in float3 position) {
    //float r = 1.0f;
    //float distance = length(Trans(position)) - r;
    
    // XZ軸に繰り返し
    float2 r = REPETITION(position.xz, 5.0f);
    float3 rayPos = float3(r.x, position.y, r.y);
    //float height = sin(r.x / 5.0f);
    //float height = lerp(0.0f, 5.0f, Random((float2)(int2) ((position.xz / 5.0f))));
    float distance = sdBox(rayPos - float3(0.0f, -50.0f, 0.0f), float3(2.25f, 1.0f, 2.25f)) - 0.25f;
    
    return distance;
}

float3 GetNormal(in float3 position) {
    float3 d = float3(0.0001f, -0.0001f, 0.0f);
    
    float3 normal;
    normal.x = GetDistance(position + d.xzz) - GetDistance(position + d.yzz);
    normal.y = GetDistance(position + d.zxz) - GetDistance(position + d.zyz);
    normal.z = GetDistance(position + d.zzx) - GetDistance(position + d.zzy);
    return normalize(normal);
}

float Raymarch(in float3 rayOrigin, in float3 rayDirection) {
    
    float distance = 0.0f;
    
    for (uint i = 0; i < MAX_STEPS; ++i) {
        float3 position = rayOrigin + rayDirection * distance;
        float d = GetDistance(position);
        distance += d;
        
        if (distance > MAX_DISTANCE || d < EPSILON) {
            break;
        }
    }

    return distance;
}

float3 Fog(in float3 color, in float3 fogColor, in float depth) {
    float t = (depth - MIN_FOG_DEPTH) / (MAX_FOG_DEPTH - MIN_FOG_DEPTH);
    return lerp(color, fogColor, saturate(t));
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

    float2 dimensions;
    g_Output.GetDimensions(dimensions.x, dimensions.y);
    
    float2 texcoord = ((float2) DTid.xy + 0.5f) / dimensions;
    float3 nearPosition = GetWorldPosition(texcoord, 0.0f, g_Scene.viewProjectionInverse);
    float3 farPosition = GetWorldPosition(texcoord, 1.0f, g_Scene.viewProjectionInverse);
    
    float3 rayOrigin = nearPosition;
    float3 rayDirection = normalize(farPosition - nearPosition);
    float distance = Raymarch(rayOrigin, rayDirection);
    
    float3 lightDirection = normalize(float3(0.2f, -1.0f, 0.3f));
    
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    if (distance < MAX_DISTANCE) {
        float3 position = rayOrigin + rayDirection * distance;
        float3 normal = GetNormal(position);
        float3 pixelToCamera = normalize(g_Scene.cameraPosition - position);
        color.rgb = Lighting::HalfLambertReflection(normal, lightDirection) + Lighting::BlinnPhongReflection(normal, pixelToCamera, lightDirection, 10.0f);
        color.rgb *= lerp(float3(1.0f, 1.0f, 1.0f), float3(1.0f, 0.0f, 0.0f), abs(floor(position.x / 5.0f) + floor(position.z / 5.0f)) % 2);
        
        color.rgb = Fog(color.rgb, float3(0.0f, 0.0f, 0.0f), distance);
        
    }
        
    g_Output[DTid.xy] = color;
}
RWTexture2D<float4> g_Texture : register(u0);

float Random(float2 uv, float seed) {
    return frac(sin(dot(uv, float2(12.9898f, 78.233f)) + seed) * 43758.5453f);
}

float2 Random2(float2 fact) {
    const float2 v1 = float2(127.1f, 311.7f);
    const float2 v2 = float2(269.5f, 183.3f);
    
    float2 angle = float2(dot(fact, v1), dot(fact, v2));
    return frac(sin(angle) * 43758.5453123f) * 2.0f - 1.0f;
}

float LerpRandom(float2 uv, float density, float seed) {
    float invDensity = 1.0f / density;
    float2 block = floor(uv * density);
    float2 pixel = frac(uv * density);
    //pixel = pixel * pixel * (3.0f - 2.0f * pixel);
    
    float v00 = Random((block + float2(0.0f, 0.0f)) * invDensity, 0.0f);
    float v01 = Random((block + float2(0.0f, 1.0f)) * invDensity, 0.0f);
    float v10 = Random((block + float2(1.0f, 0.0f)) * invDensity, 0.0f);
    float v11 = Random((block + float2(1.0f, 1.0f)) * invDensity, 0.0f);
    
    float v0010 = lerp(v00, v10, pixel.x);
    float v0111 = lerp(v01, v11, pixel.x);
    
    float random = lerp(v0010, v0111, pixel.y);
    return random;
}

float PerlinNoise(float2 uv, float density) {
    float2 uvFloor = floor(uv * density);
    float2 uvFrac = frac(uv * density);
    
    float2 v00 = Random2(uvFloor + float2(0.0f, 0.0f));
    float2 v01 = Random2(uvFloor + float2(0.0f, 1.0f));
    float2 v10 = Random2(uvFloor + float2(1.0f, 0.0f));
    float2 v11 = Random2(uvFloor + float2(1.0f, 1.0f));

    float c00 = dot(v00, uvFrac - float2(0.0f, 0.0f));
    float c01 = dot(v01, uvFrac - float2(0.0f, 1.0f));
    float c10 = dot(v10, uvFrac - float2(1.0f, 0.0f));
    float c11 = dot(v11, uvFrac - float2(1.0f, 1.0f));

    float2 u = uvFrac * uvFrac * (3.0f - 2.0f * uvFrac);
    
    float v0010 = lerp(c00, c10, u.x);
    float v0111 = lerp(c01, c11, u.x);

    return lerp(v0010, v0111, u.y) * 0.5f + 0.5f;
}

float FractalSumNoise(float2 uv, float density) {
    float fn;
    fn  = PerlinNoise(uv, density * 1.0f);
    //fn += PerlinNoise(uv, density * 2.0f) * 1.0f /  4.0f;
    //fn += PerlinNoise(uv, density * 4.0f) * 1.0f /  8.0f;
    //fn += PerlinNoise(uv, density * 8.0f) * 1.0f / 16.0f;
    return fn;
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
    
    float2 textureSize;
    g_Texture.GetDimensions(textureSize.x, textureSize.y);
    float2 uv = DTid.xy / textureSize;
    
    float random = FractalSumNoise(uv, 10.0f);
    
    g_Texture[DTid.xy] = lerp(float4(1.0f, 1.0f, 1.0f, 1.0f), float4(0.2f, 0.3f, 0.6f, 1.0f), 1.0f - pow(1.0f - random, 3.0f));
}
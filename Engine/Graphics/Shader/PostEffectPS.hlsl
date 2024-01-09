

Texture2D<float4> texture_ : register(t0);
Texture2D<float4> shadow_ : register(t1);
Texture2D<float4> reflection_ : register(t2);
SamplerState sampler_ : register(s0);

struct PSInput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

float3 LinearToSRGB(float3 color) {
    float3 sqrt1 = sqrt(color);
    float3 sqrt2 = sqrt(sqrt1);
    float3 sqrt3 = sqrt(sqrt2);
    float3 srgb = 0.662002687 * sqrt1 + 0.684122060 * sqrt2 - 0.323583601 * sqrt3 - 0.0225411470 * color;
    return srgb;
}
    
PSOutput main(PSInput input) {
    PSOutput output;
    
    output.color = texture_.Sample(sampler_, input.texcoord) * shadow_.Sample(sampler_, input.texcoord).r;
    
    float4 reflection = reflection_.Sample(sampler_, input.texcoord);
    output.color.rgb = lerp(output.color.rgb, reflection.rgb, reflection.a);
    
    //output.color.xyz = LinearToSRGB(output.color.xyz);
    //output.color.xyz = pow(output.color.xyz, 1.0f / (1.0f / 2.2f));
    
    return output;
}
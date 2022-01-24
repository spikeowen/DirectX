//--------------------------------------------------------------------------------------
// File: DX11 Framework.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
    
    
    float4 DiffuseMtrl;
    float4 DiffuseLight;
    float4 AmbientMaterial;
    float4 AmbientLight;
    float4 SpecularMtrl;
    float4 SpecularLight;
    float SpecularPower;
    float3 EyePosW; // Camera position in world space
    float3 LightVecW;
    float gTime;
}

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
    float3 Norm : NORMAL;
    float3 PosW : POSITION;
    float2 Tex : TEXCOORD0;
};

////--------------------------------------------------------------------------------------
//// Vertex Shader
////--------------------------------------------------------------------------------------
//VS_OUTPUT VS( float4 Pos : POSITION, float4 Color : COLOR )
//{
//    VS_OUTPUT output = (VS_OUTPUT)0;
//	Pos.xy += 0.5f * sin(Pos.x) * sin(3.0f * gTime);
//	Pos.z *= 0.6f + 0.4f * sin(2.0f * gTime);
//    output.Pos = mul( Pos, World );
//    output.Pos = mul( output.Pos, View );
//    output.Pos = mul( output.Pos, Projection );
//    output.Color = Color;
//    return output;
//}


//------------------------------------------------------------------------------------

// Vertex Shader - Implements Gouraud Shading using Diffuse lighting only

//------------------------------------------------------------------------------------

VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD0)

{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(Pos, World);
    output.PosW = output.Pos;

    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);

    float3 normalW = mul(float4(NormalL, 0.0f), World).xyz;
    normalW = normalize(normalW);

    output.Norm = normalW;
    output.Tex = Tex;

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    // Compute the reflection vector
    float3 r = reflect(-LightVecW, input.Norm);
    float3 toEye = normalize(EyePosW - input.Pos);

    // Determine how much (if any) specular light makes it into the eye.
    float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);
    float3 specularRGB = specularAmount * (SpecularMtrl * SpecularLight).rgb;

    // Compute Diffuse lighting only
    float diffuseAmount = max(dot(LightVecW, input.Norm), 0.0f);
    float3 diffRGB = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;

    // Compute Ambient lighting only
    float3 ambientRGB = AmbientMaterial * AmbientLight;

    // Compute Colour using Diffuse + Ambient Lighting
    input.Color.rgb = diffRGB + ambientRGB + specularRGB;
    input.Color.a = DiffuseMtrl.a;
    float4 textureColor = txDiffuse.Sample(samLinear, input.Tex);
    input.Color = input.Color * textureColor;
    clip(input.Color.a - 0.25f);

    return input.Color;
    //return float4(1,1,0,1);
}

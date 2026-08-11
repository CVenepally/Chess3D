#define MAX_LIGHTS 64
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

Texture2D       diffuseTexture      : register(t0);
SamplerState    diffuseSampler      : register(s0);
Texture2D       normalTexture       : register(t1);
SamplerState    normalSampler       : register(s1);
Texture2D       sgeTexture          : register(t2);
SamplerState    sgeSampler          : register(s2);

//------------------------------------------------------------------------------------------------
struct VertexShaderInput
{
    float3 modelPosition    : POSITION;
    float4 color            : COLOR;
    float2 uv               : TEXCOORD;
    float3 modelTangent     : TANGENT;
    float3 modelBitangent   : BITANGENT;
    float3 modelNormal      : NORMAL;
};

//------------------------------------------------------------------------------------------------
struct PixelShaderInput
{
    float4 clipPosition     : SV_POSITION;
    float4 worldPosition    : POSITION;
    float4 color            : COLOR;
    float2 uv               : TEXCOORD;
    float4 worldTangent     : TANGENT;
    float4 worldBitangent   : BITANGENT;
    float4 worldNormal      : NORMAL;
};

//------------------------------------------------------------------------------------------------
struct Light
{
    int     LightType;
    float3  Position;
	//--------------------------------------16 bytes
	
    float3  Direction;
    float   Padding0;
    //--------------------------------------16 bytes
	
    float4  Color;
	//--------------------------------------16 bytes	

    float InnerRadius;
    float OuterRadius;
    float InnerDot;
    float OuterDot;
};

//------------------------------------------------------------------------------------------------
cbuffer DebugConstants : register(b1)
{
    float   Time;
    int     DebugInt;
    float   DebugFloat;
    int     Padding;
}

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
    float4x4 WorldToCameraTransform;
    float4x4 CameraToRenderTransform;
    float4x4 RenderToClipTransform;
};

//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register(b3)
{
    float4x4    ModelToWorldTransform;
    float4      ModelColor; 
    int         SpecialInt;
    float3      Padding3;
};

//------------------------------------------------------------------------------------------------
cbuffer LightConstants : register(b4)
{
    Light   DirectionalLight; // 64 bytes
    Light   AllLights[MAX_LIGHTS]; // 64 bytes
	
    float3  CameraPosition;
    int     NumLights;
	//----------------------16
	
    float   AmbientIntensity;
    float3  DummyPadding;
	//-----------------------16	
};

//------------------------------------------------------------------------------------------------
PixelShaderInput VertexMain(VertexShaderInput vertexInput)
{
    float4 modelPosition    = float4(vertexInput.modelPosition, 1);
    float4 worldPosition    = mul(ModelToWorldTransform,    modelPosition);
    float4 cameraPosition   = mul(WorldToCameraTransform,   worldPosition);
    float4 renderPosition   = mul(CameraToRenderTransform,  cameraPosition);
    float4 clipPosition     = mul(RenderToClipTransform,    renderPosition);
    
    float4 worldTangent     = mul(ModelToWorldTransform, float4(vertexInput.modelTangent, 0.f));
    float4 worldBitangent   = mul(ModelToWorldTransform, float4(vertexInput.modelBitangent, 0.f));
    float4 worldNormal      = mul(ModelToWorldTransform, float4(vertexInput.modelNormal, 0.f));
    
    PixelShaderInput pixelIn;
    
    pixelIn.clipPosition    = clipPosition;
    pixelIn.worldPosition   = worldPosition;
    pixelIn.color           = vertexInput.color;
    pixelIn.uv              = vertexInput.uv;
    pixelIn.worldTangent    = worldTangent;
    pixelIn.worldBitangent  = worldBitangent;
    pixelIn.worldNormal     = worldNormal;
    
    return pixelIn;
}

//------------------------------------------------------------------------------------------------
float3 EncodeXYZtoRGB(float3 xyzToEncode)
{
    return (xyzToEncode + 1.f) * 0.5f;
}

//------------------------------------------------------------------------------------------------
float3 DecodeRGBtoXYZ(float3 rgbToDecode)
{
    return (rgbToDecode * 2.f) - 1.f;
}

//------------------------------------------------------------------------------------------------
float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
    float fraction = saturate((inValue - inStart) / (inEnd - inStart));
    float outValue = outStart + fraction * (outEnd - outStart);
    return outValue;
}

//------------------------------------------------------------------------------------------------
float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
    float fraction = (inValue - inStart) / (inEnd - inStart);
    float outValue = outStart + fraction * (outEnd - outStart);
    return outValue;
}

//------------------------------------------------------------------------------------------------
float4 PixelMain(PixelShaderInput pixelIn) : SV_Target0
{    
    float4 diffuseTexel     = diffuseTexture.Sample(diffuseSampler, pixelIn.uv);
    float4 normalTexel      = normalTexture.Sample(normalSampler, pixelIn.uv);
    float4 sgeTexel         = sgeTexture.Sample(normalSampler, pixelIn.uv);
    float4 surfaceColor     = pixelIn.color;
    float4 modelColor       = ModelColor;
    float4 diffuseColor     = diffuseTexel * surfaceColor * modelColor;
    clip(diffuseColor.a - 0.01f);
    
    float3 surfaceTangentWorldSpace     = normalize(pixelIn.worldTangent.xyz);
    float3 surfaceBitangentWorldSpace   = normalize(pixelIn.worldBitangent.xyz);
    float3 surfaceNormalWorldSpace      = normalize(pixelIn.worldNormal.xyz);
    
    float3x3 tbnToWorld              = float3x3(surfaceTangentWorldSpace, surfaceBitangentWorldSpace, surfaceNormalWorldSpace);
    float3   pixelNormalTBNSpace     = normalize(DecodeRGBtoXYZ(normalTexel.rgb));   
    float3   pixelNormalWorldSpace   = mul(pixelNormalTBNSpace, tbnToWorld);
    
    /// LIGHTING
    float3 lightDir         = normalize(DirectionalLight.Direction); // or light to pixel / Normalize it C++ before sending it in.
    float3 pixelToLight     = -lightDir;
    float3 pixelToCamera    = normalize(CameraPosition - pixelIn.worldPosition.xyz);
    
    float specularity   = sgeTexel.r;
    float glossiness    = sgeTexel.g;
    float emissiveness  = sgeTexel.b;
  
    // Diffuse color, specular color from all lights will be added.
    float3 totalDiffuse = float3(0.f, 0.f, 0.f);
    float3 totalSpecular = float3(0.f, 0.f, 0.f);
    
    /// DIRECTIONAL LIGHT
    
    // Diffuse
    float  diffuseLightDot          = saturate(dot(pixelToLight, pixelNormalWorldSpace));
    float  directionalLightStrength = DirectionalLight.Color.a * saturate(RangeMapClamped(diffuseLightDot, -AmbientIntensity, 1.f, 0.f, 1.f));
    float3 directionalLightDiffuse  = directionalLightStrength * DirectionalLight.Color.rgb;
    totalDiffuse += directionalLightDiffuse;
    
    // Specular
    float3  sunReflectionDir            = normalize(pixelToLight + pixelToCamera);
    float   sunReflectionDirNormalDot   = dot(sunReflectionDir, pixelNormalWorldSpace);
    float   specularPower               = RangeMapClamped(glossiness, 0.f, 1.f, 1.f, 32.f);
    float   sunSpecular                 = glossiness * DirectionalLight.Color.a * pow(sunReflectionDirNormalDot, specularPower);
    float3  sunSpecularHighlight        = sunSpecular * DirectionalLight.Color.rgb;  
    totalSpecular += sunSpecularHighlight;

    /// ALL OTHER SCENE LIGHTS
    for (int lightIndex = 0; lightIndex < NumLights; ++lightIndex)
    {
        float3 lightWorldPos        = AllLights[lightIndex].Position;
        float3 lightColor           = AllLights[lightIndex].Color.rgb;
        float lightIntensity        = AllLights[lightIndex].Color.a;
        float3 pixelToLightVec      = lightWorldPos - pixelIn.worldPosition.xyz;
        float3 pixelToLightDir      = normalize(pixelToLightVec);
        float pixelToLightDistance  = length(pixelToLightVec);
        float innerRadius           = AllLights[lightIndex].InnerRadius;
        float outerRadius           = AllLights[lightIndex].OuterRadius;
        float innerDot              = AllLights[lightIndex].InnerDot;
        float outerDot              = AllLights[lightIndex].OuterDot;
        float spotFalloff           = 1.f;
        float3 spotLightDirection   = AllLights[lightIndex].Direction;
        
        // Diffuse
        float falloff = RangeMapClamped(pixelToLightDistance, outerRadius, innerRadius, 0.f, 1.f);
        
        if (AllLights[lightIndex].LightType == SPOT_LIGHT)
        {
            float spotDirNormalDot = dot(spotLightDirection, -pixelToLightDir);
            spotFalloff = saturate(RangeMap(spotDirNormalDot, outerDot, innerDot, 0.f, 1.f));
        }
        
        float lightDirNormalDot = dot(pixelToLightDir, pixelNormalWorldSpace);
        float sceneLightStrength = spotFalloff * lightIntensity * falloff * RangeMapClamped(lightDirNormalDot, -AmbientIntensity, 1.f, 0.f, 1.f);
        
        float3 diffuseLight = sceneLightStrength * lightColor;
        totalDiffuse += diffuseLight;
        
        // Specular
        float3 reflectionDirection = normalize(pixelToCamera + pixelToLightDir);
        float reflectionDirNormalDot = saturate(dot(reflectionDirection, pixelNormalWorldSpace));
        float specularStrength = glossiness * lightIntensity * pow(reflectionDirNormalDot, specularPower);
        specularStrength *= falloff * spotFalloff;
        
        float3 specularHighight = specularStrength * lightColor;
        totalSpecular += specularHighight;
    }
    
    // Total Emissive
    float3 totalEmissive = diffuseTexel.rgb * emissiveness;
    
    /// COMPOSITE LIGHTING COLOR    
    float3 finalLightingColor   = (saturate(totalDiffuse) * diffuseColor.rgb) + (totalSpecular * specularity) + totalEmissive;
    float4 finalColor           = float4(finalLightingColor, diffuseColor.a);
    
    if(SpecialInt == 1)
    {
        finalColor.rgb += finalColor.rgb * 1.5f;
    }
    else if(SpecialInt == 2)
    {
        float timeScaledAlpha = (sin(Time * 3.f) * 0.5f) + 0.5f;
        float finalAlpha = RangeMapClamped(timeScaledAlpha, 0.f, 1.f, 0.2f, 0.9f);
        finalColor.a = finalAlpha;
    //    finalColor.rgb += float3(1.f, 1.f, 1.f);
        
    }
    else if(SpecialInt == 3)
    {
        finalColor.rgb += finalColor.rgb * 0.8f;
        finalColor.a = 0.6f;
    }

    // Debug Modes (TODO)
    // 0 - Default (Lit)
    // 1 - Diffuse Only (unlit)
    // 2 - Diffuse + Light (no normal maps)
    // 3 - Diffuse + Light + Normals (No SGE)
    // 5 - Normal Maps Only (raw normal texel rgb)
    // 6 - SGE Only (raw)
    // 7 - Specular Only
    // 8 - Gloss Only
    // 9 - Emit only
    // 11 - Pixel Normals Transformed
    // 12 - Pixel Normals TBN Space
    // 13 - Surface Normals
    // 14 - Tangents, surface
    // 15 - Tangents, transformed
    // 16 - Bitangents, surface
    // 17 - Bitangents, transformed
    // 18 - UVs
    // 19 - Light Strength
    // 10 - Vertex Color only
    
    //if(DebugInt == 1)
    //{
    //    finalColor.rgb = diffuseTexel.rgb;
    //}
    //else if(DebugInt == 2)
    //{
    //    finalColor.rgb = normalTexel.rgb;
    //}
    //else if (DebugInt == 3)
    //{
    //    finalColor.rgb = float3(pixelIn.uv, 0.f);
    //}
    //else if(DebugInt == 4)
    //{
    //    finalColor.rgb = EncodeXYZtoRGB(surfaceTangentWorldSpace);
    //}
    //else if (DebugInt == 5)
    //{
    //    finalColor.rgb = EncodeXYZtoRGB(surfaceBitangentWorldSpace);
    //}
    //else if (DebugInt == 6)
    //{
    //    finalColor.rgb = EncodeXYZtoRGB(surfaceNormalWorldSpace);
    //}
    //else if (DebugInt == 7)
    //{
    //    finalColor.rgb = EncodeXYZtoRGB(pixelNormalTBNSpace);
    //}
    //else if (DebugInt == 8)
    //{
    //    finalColor.rgb = lightStrength.xxx;
    //}
    //else if(DebugInt == 9)
    //{
    //    finalColor.rgb = diffuseColor.rgb;
    //}
    //else if (DebugInt == 11)
    //{
    //    finalColor.rgb = specular.xxx;
    //}
    //else if (DebugInt == 12)
    //{
    //   finalColor.rgb = pixelIn.color.rgb;
    //}
    
    return finalColor;
}


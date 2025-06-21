Texture2D       diffuseTexture      : register(t0);
SamplerState    diffuseSampler      : register(s0);
Texture2D       normalTexture       : register(t1);
SamplerState    normalSampler       : register(s1);
Texture2D       sgeTexture          : register(t2);
SamplerState    sgeSampler          : register(s2);

#define MAX_LIGHTS 64
#define DIRECTIONAL_LIGHT 0


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
    float   Intensity;
    //--------------------------------------16 bytes
	
    float4  Color;
	//--------------------------------------16 bytes	

    float   SpotAngle;
    float   ConstantAttenuation;
    float   LinearAttenuation;
    float   QuadraticAttenuation;
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
    float4x4 ModelToWorldTransform;
    float4 ModelColor;
};

//------------------------------------------------------------------------------------------------
cbuffer LightConstants : register(b4)
{
    Light DirectionalLight; // 64 bytes
    Light AllLights[MAX_LIGHTS]; // 64 bytes
	
    float3 CameraPosition;
    int NumLights;
	//----------------------16
	
    float AmbientIntensity;
    float3 DummyPadding;
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
float4 PixelMain(PixelShaderInput pixelIn) : SV_Target0
{
    
    float4 diffuseTexel     = diffuseTexture.Sample(diffuseSampler, pixelIn.uv);
    float4 normalTexel      = normalTexture.Sample(normalSampler, pixelIn.uv);
    float4 sgeTexel         = sgeTexture.Sample(normalSampler, pixelIn.uv);
    float4 surfaceColor     = pixelIn.color;
    float4 modelColor       = ModelColor;
    float4 diffuseColor     = diffuseTexel * surfaceColor * modelColor;
    float3 lightDir         = normalize(DirectionalLight.Direction); // or light to pixel
    float3 pixelToLight     = -lightDir;
    float3 pixelToCamera    = normalize(CameraPosition - pixelIn.worldPosition.xyz);
    
    float  specularIntensity    = sgeTexel.r;
    float  gloss                = sgeTexel.g;
    float  emissiveIntensity    = sgeTexel.b;
    
    float3 surfaceTangentWorldSpace     = normalize(pixelIn.worldTangent.xyz);
    float3 surfaceBitangentWorldSpace   = normalize(pixelIn.worldBitangent.xyz);
    float3 surfaceNormalWorldSpace      = normalize(pixelIn.worldNormal.xyz);
    
    float3x3 tbnToWorld              = float3x3(surfaceTangentWorldSpace, surfaceBitangentWorldSpace, surfaceNormalWorldSpace);
    float3   pixelNormalTBNSpace     = normalize(DecodeRGBtoXYZ(normalTexel.rgb));   
    float3   pixelNormalWorldSpace   = mul(pixelNormalTBNSpace, tbnToWorld);
    
    float  diffuseLightDot  = DirectionalLight.Intensity * saturate(dot(-lightDir, pixelNormalWorldSpace));
    float4 lightColor       = float4((AmbientIntensity + diffuseLightDot).xxx * DirectionalLight.Color.rgb, 1);
    
    // specular calculation
    float3 hVector         = normalize(pixelToLight + pixelToCamera);
    float  hDotN           = dot(hVector, pixelNormalWorldSpace);
    float  specularPower   = RangeMapClamped(gloss, 0.f, 1.f, 1.f, 32.f);
    float3 specularColor   = specularIntensity * (saturate(pow(hDotN, specularPower) * lightColor.rgb));
    
    float3 emissiveColor = lightColor.rgb * emissiveIntensity;
    
    float4 finalColor = float4((diffuseColor.rgb + specularColor + emissiveColor) * lightColor.rgb, diffuseColor.a);
        
    clip(finalColor.a - 0.01f);
 
    if(DebugInt == 1)
    {
        finalColor.rgb = diffuseTexel.rgb;
    }
    else if(DebugInt == 2)
    {
        finalColor.rgb = normalTexel.rgb;
    }
    else if (DebugInt == 3)
    {
        finalColor.rgb = float3(pixelIn.uv, 0.f);
    }
    else if(DebugInt == 4)
    {
        finalColor.rgb = EncodeXYZtoRGB(surfaceTangentWorldSpace);
    }
    else if (DebugInt == 5)
    {
        finalColor.rgb = EncodeXYZtoRGB(surfaceBitangentWorldSpace);
    }
    else if (DebugInt == 6)
    {
        finalColor.rgb = EncodeXYZtoRGB(surfaceNormalWorldSpace);
    }
    else if (DebugInt == 7)
    {
        finalColor.rgb = EncodeXYZtoRGB(pixelNormalWorldSpace);
    }
    else if (DebugInt == 8)
    {
        finalColor.rgb = lightColor.xxx;
    }
    else if (DebugInt == 11)
    {
        finalColor.rgb = specularColor;
    }
    else if (DebugInt == 12)
    {
        finalColor.rgb = emissiveColor;
    }
    
    return finalColor;
}


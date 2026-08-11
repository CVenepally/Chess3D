#define MAX_LIGHTS 64
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
#define PI 3.1415926f

Texture2D       albedoTexture : register(t3);
SamplerState    albedoSampler : register(s3);

Texture2D       normalTexture  : register(t1);
SamplerState    normalSampler  : register(s1);

Texture2D       metallicTexture : register(t4);
SamplerState    metallicSampler : register(s4);

Texture2D       roughnessTexture : register(t5);
SamplerState    roughnessSampler : register(s5);

Texture2D       aoTexture : register(t6);
SamplerState    aoSampler : register(s6);                

//------------------------------------------------------------------------------------------------
struct VertexInput
{
    float3 vi_position          : POSITION;
    float4 vi_vertexColor       : COLOR;
    float2 vi_texCoords         : TEXCOORD;
    float3 vi_vertexTangent     : TANGENT;
    float3 vi_vertexBitangent   : BITANGENT;
    float3 vi_vertexNormal      : NORMAL;
};

//------------------------------------------------------------------------------------------------
struct PixelInput
{
    float4 pi_position          : SV_POSITION;
    float3 pi_worldPosition     : POSITION;
    float4 pi_vertexColor        : COLOR;
    float2 pi_texCoords         : TEXCOORD;
    float3 pi_vertexTangent     : TANGENT0;
    float3 pi_vertexBitangent   : BITANGENT0;
    float3 pi_vertexNormal      : NORMAL0;
    float3 pi_worldTangent      : TANGENT1;
    float3 pi_worldBitangent    : BITANGENT1;
    float3 pi_worldNormal       : NORMAL1;
};

//------------------------------------------------------------------------------------------------
struct Light
{
    int     lightType;
    float3  position;
	//--------------------------------------16 bytes
	
    float3  direction;
    float   padding1;
    //--------------------------------------16 bytes
	
    float4  color;
	//--------------------------------------16 bytes	

    float   innerRadius;
    float   outerRadius;
    float   innerDot;
    float   outerDot;
};

//------------------------------------------------------------------------------------------------
cbuffer DebugConstants : register(b1)
{
    float   debugTime;
    int     debugInt;
    float   debugFloat;
    float   padding0;
}

//------------------------------------------------------------------------------------------------
cbuffer CameraConstants : register(b2)
{
    float4x4 worldToCameraTransform;
    float4x4 cameraToRenderTransform;
    float4x4 renderToClipTransform;
}

//------------------------------------------------------------------------------------------------
cbuffer ModelConstants : register(b3)
{
    float4x4    modelToWorldTransform;
    float4      modelColor;
    int         specialInt;
    float3      padding3;
}

//------------------------------------------------------------------------------------------------
cbuffer LightConstants : register(b4)
{
    Light   directionalLight; // 64 bytes
    Light   allLights[MAX_LIGHTS]; // 64 bytes
	
    float3  cameraPosition;
    int     numLights;
	//----------------------16
	
    float   ambientIntensity;
    float3  padding4;
	//-----------------------16	
}

//------------------------------------------------------------------------------------------------
PixelInput VertexMain(VertexInput vertexInput)
{
    float4 modelPosition    = float4(vertexInput.vi_position, 1);
    float4 worldPosition    = mul(modelToWorldTransform, modelPosition);
    float4 cameraPosition   = mul(worldToCameraTransform, worldPosition);
    float4 renderPosition   = mul(cameraToRenderTransform, cameraPosition);
    float4 clipPosition     = mul(renderToClipTransform, renderPosition);
    float4 worldTangent     = mul(modelToWorldTransform, float4(vertexInput.vi_vertexTangent,   0.f));
    float4 worldBitangent   = mul(modelToWorldTransform, float4(vertexInput.vi_vertexBitangent, 0.f));
    float4 worldNormal      = mul(modelToWorldTransform, float4(vertexInput.vi_vertexNormal,    0.f));
    
    PixelInput pixelIn;
    pixelIn.pi_position         = clipPosition;
    pixelIn.pi_worldPosition    = worldPosition.xyz;
    pixelIn.pi_texCoords        = vertexInput.vi_texCoords;
    pixelIn.pi_vertexColor      = vertexInput.vi_vertexColor;
    pixelIn.pi_vertexTangent    = vertexInput.vi_vertexTangent;
    pixelIn.pi_vertexBitangent  = vertexInput.vi_vertexBitangent;
    pixelIn.pi_vertexNormal     = vertexInput.vi_vertexNormal;
    pixelIn.pi_worldTangent     = worldTangent.xyz;
    pixelIn.pi_worldBitangent   = worldBitangent.xyz;
    pixelIn.pi_worldNormal      = worldNormal.xyz;
    
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
float NormalDistribution(float3 surfaceNormal, float3 halfVector, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    
    float normalHalfVectorDot = max(dot(surfaceNormal, halfVector), 0.f);
    float normalHalfVectorDot2 = normalHalfVectorDot * normalHalfVectorDot;
    
    float denom = (normalHalfVectorDot2 * (a2 - 1.f) + 1.f);
    denom = PI * denom * denom; 
    
    return a2 / denom;
}

//------------------------------------------------------------------------------------------------
float GeometrySchlik(float lightOrCameraDotWithNormal, float roughness)
{
    float r = 1 + roughness; // Schlik remap of roughness to calculate occlussion bias k based on roughness
    float occlusionBias = (r * r) / 8.f;

    return lightOrCameraDotWithNormal / (lightOrCameraDotWithNormal * (1.f - occlusionBias) + occlusionBias);
}

//------------------------------------------------------------------------------------------------
float GeometrySmith(float3 surfaceNormal, float3 pixelToLight, float3 pixelToCamera, float roughness)
{
    float normalCameraDot   = max(dot(surfaceNormal, pixelToCamera), 0.f);
    float normalLightDot    = max(dot(surfaceNormal, pixelToLight), 0.f);

    float viewDirectionGeometrySchlik   = GeometrySchlik(normalCameraDot, roughness);
    float lightDirectionGeometrySchlik  = GeometrySchlik(normalLightDot, roughness);
    
    return viewDirectionGeometrySchlik * lightDirectionGeometrySchlik;
}

//------------------------------------------------------------------------------------------------
float3 FresnelSchlick(float cosTheta, float3 baseReflectivity)
{
    return baseReflectivity + (1.f - baseReflectivity) * pow(saturate(1.f - cosTheta), 5.f);
}

//------------------------------------------------------------------------------------------------
float4 PixelMain(PixelInput pixelIn) : SV_Target0
{    
    float4 albedoTexel      = albedoTexture.Sample(albedoSampler, pixelIn.pi_texCoords);
    float4 normalTexel      = normalTexture.Sample(normalSampler, pixelIn.pi_texCoords);
    float4 metallicTexel    = metallicTexture.Sample(metallicSampler, pixelIn.pi_texCoords);
    float4 roughnessTexel   = roughnessTexture.Sample(roughnessSampler, pixelIn.pi_texCoords);
    float4 aoTexel          = aoTexture.Sample(aoSampler, pixelIn.pi_texCoords);

    float metalness = metallicTexel.r;
    float roughness = roughnessTexel.r;
    float ao        = aoTexel.r;
    
    float4 albedoColor      = albedoTexel * pixelIn.pi_vertexColor * modelColor;
    
    //--------------------------------------------------------------------------------------------------
    // Pixel Normal Calculations
    //--------------------------------------------------------------------------------------------------
    float3 worldTangent     = normalize(pixelIn.pi_worldTangent);
    float3 worldBitangent   = normalize(pixelIn.pi_worldBitangent);
    float3 worldNormal      = normalize(pixelIn.pi_worldNormal);

    float3x3 tbnToWorldTransform = float3x3(worldTangent, worldBitangent, worldNormal);
    
    float3 pixelNormalTBNSpace      = normalize(DecodeRGBtoXYZ(normalTexel.rgb));
    float3 pixelNormalWorldSpace    = mul(pixelNormalTBNSpace, tbnToWorldTransform);
    
    //--------------------------------------------------------------------------------------------------
    // Lighting
    //--------------------------------------------------------------------------------------------------  
    float3 totalRadience = float3(0.f, 0.f, 0.f); // Lo term in the reflectance equation
    float3 pixelToCamera = normalize(cameraPosition - pixelIn.pi_worldPosition); // V
    
    // F0 - Refelectivity of the sufrace at 0 incidence or directly looking at it along the surface normal. 
    // This varies for metals and non metals.
    // For non metals it is always 0.04 (approximated, averaged to be physically accurate for most non metals, hence init with 0.04).
    // For metals we lerp 0.04 to albedo value based on the metallic property.
    // We store it as float3 instead of float because if surface is a metal, there's a tint to it.
    float3 baseSurfaceReflectivity  = float3(0.04f.xxx); //F0
    baseSurfaceReflectivity         = lerp(baseSurfaceReflectivity, albedoColor.rgb, metalness);
   
    //--------------------------------------------------------------------------------------------------
    // Directional Lighting / Sun Light
    //--------------------------------------------------------------------------------------------------  
    float3 sunToPixel                   = directionalLight.direction;
    float3 pixelToSun                   = -sunToPixel; // L
    float3 sunIdealReflectionDirection  = normalize(pixelToSun + pixelToCamera); //halfVector or H 
    float3 sunColor                     = directionalLight.color.rgb * directionalLight.color.a;
    
    float normalDistribution            = NormalDistribution(pixelNormalWorldSpace, sunIdealReflectionDirection, roughness);
    float geometryOcclusion             = GeometrySmith(pixelNormalWorldSpace, pixelToSun, pixelToCamera, roughness);
    float cosTheta                      = max(dot(sunIdealReflectionDirection, pixelToCamera), 0.f);
    float3 fresnelSchlick               = FresnelSchlick(cosTheta, baseSurfaceReflectivity);
    
    float3 specularReflectionRatio  = fresnelSchlick;
    float3 diffuseReflectionRatio   = float3(1.f.xxx) - specularReflectionRatio;
    diffuseReflectionRatio *= (1.f - metalness);
    
    float3 numerator = normalDistribution * geometryOcclusion * fresnelSchlick;
    float denominator = 4.f * max(dot(pixelNormalWorldSpace, pixelToCamera), 0.f) * max(dot(pixelNormalWorldSpace, pixelToSun), 0.f) + 0.0001f;
    float3 specular = numerator / denominator;
    
    float lightNormalDot = max(dot(pixelNormalWorldSpace, pixelToSun), 0.f);
    float3 sunRadience = (diffuseReflectionRatio * albedoColor.rgb / PI + specular) * sunColor * lightNormalDot;
    totalRadience += sunRadience;  
    //--------------------------------------------------------------------------------------------------
    // Final Color
    //--------------------------------------------------------------------------------------------------  
    
    float3 finalAmbience = float3(0.03.xxx) * albedoColor.rgb * ambientIntensity * ao;
    float3 color = finalAmbience + totalRadience;
    
    color = color / (color + float3(1.f.xxx));
    color = pow(color, float3((1.f / 2.2f).xxx));
    
    float4 finalColor = float4(color, albedoColor.a);
       
    //--------------------------------------------------------------------------------------------------
    // Debugs
    //--------------------------------------------------------------------------------------------------  
    if(debugInt == 1)
    {
        finalColor.rgb = albedoTexel.rgb;
    }
    else if(debugInt == 2)
    {
        finalColor.rgb = normalTexel.rgb;
    }
    else if (debugInt == 3)
    {
        finalColor.rgb = roughnessTexel.rgb;
    }
    else if (debugInt == 4)
    {
        finalColor.rgb = metallicTexel.rgb;
    }
    else if (debugInt == 5)
    {
        finalColor.rgb = aoTexel.rgb;
    }
    else if (debugInt == 6)
    {
        finalColor.rgb = float3(pixelIn.pi_texCoords, 0.f);
    }
    else if(debugInt == 7)
    {
        finalColor.rgb = EncodeXYZtoRGB(pixelIn.pi_vertexNormal);
    }
    else if (debugInt == 8)
    {
        finalColor.rgb = EncodeXYZtoRGB(pixelIn.pi_vertexTangent);
    }
    else if (debugInt == 9)
    {
        finalColor.rgb = EncodeXYZtoRGB(pixelIn.pi_vertexBitangent);
    }
    else if (debugInt == 10)
    {
        finalColor.rgb = EncodeXYZtoRGB(worldNormal);
    }
    else if (debugInt == 11)
    {
        finalColor.rgb = EncodeXYZtoRGB(worldTangent);
    }
    else if (debugInt == 12)
    {
        finalColor.rgb = EncodeXYZtoRGB(worldBitangent);
    }
    else if(debugInt == 13)
    {
        finalColor = pixelIn.pi_vertexColor;
    }
    else if(debugInt == 14)
    {
        finalColor = modelColor;
    }    
    
    if (specialInt == 1)
    {
        finalColor.rgb += finalColor.rgb * 1.5f;
    }
    else if (specialInt == 2)
    {
        float timeScaledAlpha = (sin(debugTime * 3.f) * 0.5f) + 0.5f;
        float finalAlpha = RangeMapClamped(timeScaledAlpha, 0.f, 1.f, 0.2f, 0.9f);
        finalColor.a = finalAlpha;
    }
    else if (specialInt == 3)
    {
        finalColor.rgb += finalColor.rgb * 0.8f;
        finalColor.a = 0.6f;
    }
 
    clip(finalColor.a - 0.01);
    
    return finalColor;
}
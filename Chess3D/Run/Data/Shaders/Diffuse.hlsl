Texture2D       diffuseTexture  : register(t0);
SamplerState    samplerState    : register(s0);

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
float3 DecodeXYZtoRGB(float3 rgbToDecode)
{
    return (rgbToDecode * 2.f) - 1.f;
}

//------------------------------------------------------------------------------------------------
float4 DebugColors(PixelShaderInput pixelIn)
{  
    float4 debugColor = float4(0.f, 0.f, 0.f, 1.f);
    
    if(DebugInt == 1)
    {
        debugColor.rgb = float3(pixelIn.uv.xy, 0.f);
    }
    else if (DebugInt == 2)
    {
        float3 normalizedWorldNormal = normalize(pixelIn.worldNormal.xyz);
        debugColor.rgb = EncodeXYZtoRGB(normalizedWorldNormal);
    }
    else if (DebugInt == 3)
    {
        float3 normalizedWorldTangent = normalize(pixelIn.worldTangent.xyz);
        debugColor.rgb = EncodeXYZtoRGB(normalizedWorldTangent);
    }
    else if (DebugInt == 4)
    {
        float3 normalizedWorldBiTangent = normalize(pixelIn.worldBitangent.xyz);
        debugColor.rgb = EncodeXYZtoRGB(normalizedWorldBiTangent);
    }
    return debugColor;
}

//------------------------------------------------------------------------------------------------
float4 PixelMain(PixelShaderInput pixelIn) : SV_Target0
{
    float3 normalizedWorldNormal = normalize(pixelIn.worldNormal.xyz);

    float   ambient         = AmbientIntensity;
    float   diffuse         = DirectionalLight.Intensity * saturate(dot(normalizedWorldNormal, -DirectionalLight.Direction));
    float4  lightColor      = float4((ambient + diffuse).xxx * DirectionalLight.Color.rgb, 1);
    float4  textureColor    = diffuseTexture.Sample(samplerState, pixelIn.uv);
    float4  vertexColor     = pixelIn.color;
    float4  modelColor      = ModelColor;

    float4 finalColor = textureColor * vertexColor * modelColor * lightColor;
 
    if (DebugInt != 0)
    {
        return DebugColors(pixelIn);
    }
    
    clip(finalColor.a - 0.01f);
    
    return finalColor;
}


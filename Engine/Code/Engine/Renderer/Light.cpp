#include "Engine/Renderer/Light.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include <string>

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Light Light::CreateDirectionalLight(Vec3 const& direction, Rgba8 const& color, float intensity)
{
    Light directionalLight;
    directionalLight.m_direction = direction;
    directionalLight.m_color = color.GetAsVec4();
    directionalLight.m_lightType = static_cast<int>(LightType::DIRECTIONAL);
    directionalLight.m_intensity = intensity;

    return directionalLight;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Light Light::CreatePointLight(Vec3 const& position, float intensity, float constantAtten, float linearAtten, float exponentialAtten, Rgba8 const& color)
{
	Light pointLight;
	pointLight.m_position = position;
	pointLight.m_intensity = intensity;
    pointLight.m_constantAttenuation = constantAtten;
    pointLight.m_linearAttenuation = linearAtten;
    pointLight.m_quadraticAttenuation = exponentialAtten;

	pointLight.m_color = color.GetAsVec4();
	pointLight.m_lightType = static_cast<int>(LightType::POINT);

	return pointLight;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Light Light::CreateSpotLight(Vec3 const& position, Vec3 const& direction, float spotAngle, float intensity, float constantAtten, float linearAttenuation, float quadraticAttenuation, Rgba8 const& color)
{
	Light spotLight;
	spotLight.m_position = position;
	spotLight.m_intensity = intensity;
    spotLight.m_direction = direction;
    spotLight.m_spotAngle = spotAngle;
	spotLight.m_constantAttenuation = constantAtten;
	spotLight.m_linearAttenuation = linearAttenuation;
	spotLight.m_quadraticAttenuation = quadraticAttenuation;

	spotLight.m_color = color.GetAsVec4();
	spotLight.m_lightType = static_cast<int>(LightType::SPOT);

	return spotLight;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Light Light::CreateFromXML(XmlElement const& lightElement)
{

    Light light;

    std::string lightType = ParseXmlAttribute(lightElement, "type", "None");
    
    if(lightType == "None")
    {
        light.m_lightType = -1;
    }
    else if(lightType == "Point")
    {
        light.m_lightType = static_cast<int>(LightType::POINT);
    }
	else if(lightType == "Directional")
	{
		light.m_lightType = static_cast<int>(LightType::DIRECTIONAL);
	}
	else if(lightType == "Spot")
	{
		light.m_lightType = static_cast<int>(LightType::SPOT);
	}

    light.m_position                = ParseXmlAttribute(lightElement, "position",             light.m_position);
    light.m_direction               = ParseXmlAttribute(lightElement, "direction",            light.m_direction);
    light.m_intensity               = ParseXmlAttribute(lightElement, "intensity",            light.m_intensity);
    light.m_constantAttenuation     = ParseXmlAttribute(lightElement, "constantAttenuation",  light.m_constantAttenuation);
    light.m_linearAttenuation       = ParseXmlAttribute(lightElement, "linearAttenuation",    light.m_linearAttenuation);
    light.m_quadraticAttenuation    = ParseXmlAttribute(lightElement, "quadraticAttenuation", light.m_quadraticAttenuation);
    Rgba8 color                     = ParseXmlAttribute(lightElement, "color", Rgba8::WHITE);
    light.m_color                   = color.GetAsVec4();
    light.m_spotAngle               = ParseXmlAttribute(lightElement, "spotAngle",            light.m_spotAngle);

	return light;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Light::SetPosition(Vec3 const& newPosition)
{
    m_position = newPosition;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Light::SetDirection(Vec3 const& newDirection)
{
    m_direction = newDirection;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Rgba8 Light::GetColorAsRGBA() const
{
    Rgba8 color;
    color.r = static_cast<uchar>(Lerp(0.f, 255.f, m_color.x));
    color.g = static_cast<uchar>(Lerp(0.f, 255.f, m_color.y));
    color.b = static_cast<uchar>(Lerp(0.f, 255.f, m_color.z));
    color.a = static_cast<uchar>(Lerp(0.f, 255.f, m_color.w));

    return color;
}


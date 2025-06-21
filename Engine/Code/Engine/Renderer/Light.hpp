#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/Rgba8.hpp"

#include "ThirdParty/tinyXML2/tinyxml2.h"

typedef tinyxml2::XMLElement	XmlElement;

constexpr int MAX_LIGHTS = 64;

enum class LightType
{
	NONE = -1,

	DIRECTIONAL,
	POINT,
	SPOT
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct Light
{
public:
	
	int       m_lightType				= static_cast<int>(LightType::NONE);	
	Vec3	  m_position			    = Vec3::ZERO;

	Vec3	  m_direction			    = Vec3::ZERO;
	float	  m_intensity				= 0.f;

	Vec4	  m_color				    = Vec4::ONE;

	float     m_spotAngle				= 0.f;
	float	  m_constantAttenuation		= 0.f;
	float	  m_linearAttenuation		= 0.f;
	float	  m_quadraticAttenuation	= 0.f;

public:
	Light() = default;
	~Light() = default;

	static Light CreateDirectionalLight(Vec3 const& direction, Rgba8 const& color = Rgba8::WHITE, float intensity = 0.5f);
	static Light CreatePointLight(Vec3 const& position, float intensity, float constantAtten = 1.f, float linearAttenuation = 0.f, float exponentialAttenuation = 0.f, Rgba8 const& color = Rgba8::WHITE);
	static Light CreateSpotLight(Vec3 const& position, Vec3 const& direction, float spotAngle, float intensity, float constantAtten = 1.f, float linearAttenuation = 0.f, float exponentialAttenuation = 0.f, Rgba8 const& color = Rgba8::WHITE);
	static Light CreateFromXML(XmlElement const& lightElement);
	
	void SetPosition(Vec3 const& newPosition);
	void SetDirection(Vec3 const& newDirection);

	Rgba8 GetColorAsRGBA() const;
};
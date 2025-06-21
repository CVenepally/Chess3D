#pragma once

#include "Engine/Renderer/Camera.hpp"

#include <string>

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class Shader;
class Renderer;
class Texture;
class ConstantBuffer;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct ShadowMapConfig
{
	std::string m_name;
	int			m_size				= 1024;
	float		m_depthBias			= 0.005f;
	Shader*		m_shadowMapShader	= nullptr;
	Renderer*	m_renderer			= nullptr;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
class ShadowMap
{

public:

	ShadowMap(ShadowMapConfig const& config);
	~ShadowMap();

	std::string GetName() const;
	Texture*	GetTexture() const;

	void		UpdateCamera();
	void		BeginDepthPass();
	void		EndDepthPass();
	void		BindAsTexture();
	void		UnbindAsTexture();

public:

	Camera				m_camera; // #ToDo: If everything works, move this to lights;

private:

	ShadowMapConfig		m_config;
	Texture*			m_shadowMapTexture;
	ConstantBuffer*		m_CBO;
};
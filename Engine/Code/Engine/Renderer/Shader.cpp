#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------
Shader::Shader(const ShaderConfig& config)
	:m_config(config)
{

}

//------------------------------------------------------------------------------------------------------------------
Shader::~Shader()
{
	DX_SAFE_RELEASE(m_vertexShader);
	DX_SAFE_RELEASE(m_pixelShader);
	DX_SAFE_RELEASE(m_inputLayout);
}

//------------------------------------------------------------------------------------------------------------------
const std::string& Shader::GetName() const
{

	return m_config.m_name;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Shader::ReleaseVertexShader()
{
	DX_SAFE_RELEASE(m_vertexShader);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Shader::ReleasePixelShader()
{
	DX_SAFE_RELEASE(m_pixelShader);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Shader::ReleaseInputLayout()
{
	DX_SAFE_RELEASE(m_inputLayout);
}

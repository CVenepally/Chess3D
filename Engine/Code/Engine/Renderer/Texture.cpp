#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------
Texture::Texture()
{

}


//------------------------------------------------------------------------------------------------------------------
Texture::~Texture()
{
	DX_SAFE_RELEASE(m_texture);
	DX_SAFE_RELEASE(m_shaderResourceView);
	DX_SAFE_RELEASE(m_depthStencilView);
}


//------------------------------------------------------------------------------------------------------------------
IntVec2 Texture::GetDimensions() const
{

	return m_dimensions;

}


//------------------------------------------------------------------------------------------------------------------
std::string const& Texture::GetImageFilePath() const
{

	return m_name;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ID3D11Texture2D* Texture::GetTextureResource() const
{
	return m_texture;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ID3D11ShaderResourceView* Texture::GetSRV() const
{
	return m_shaderResourceView;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ID3D11DepthStencilView* Texture::GetDSV() const
{
	return m_depthStencilView;
}

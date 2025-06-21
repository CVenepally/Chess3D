#pragma once

#include "Engine/Math/IntVec2.hpp"

#include <string>

//------------------------------------------------------------------------------------------------------------------
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11DepthStencilView;
//------------------------------------------------------------------------------------------------------------------

class Texture
{
	friend class Renderer;

private:

	Texture();
	Texture(Texture const& copy) = delete;
	~Texture();

public:
	
	IntVec2	GetDimensions() const;
	std::string const& GetImageFilePath() const;
	
	ID3D11Texture2D*			GetTextureResource() const;
	ID3D11ShaderResourceView*	GetSRV() const;
	ID3D11DepthStencilView*		GetDSV() const;

protected:

	std::string					m_name;
	IntVec2						m_dimensions;

	ID3D11Texture2D*			m_texture				= nullptr;
	ID3D11ShaderResourceView*	m_shaderResourceView	= nullptr;
	ID3D11DepthStencilView*		m_depthStencilView		= nullptr;

};
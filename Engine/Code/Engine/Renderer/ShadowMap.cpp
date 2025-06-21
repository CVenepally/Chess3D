#include "Engine/Renderer/ShadowMap.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"

#include <d3d11.h>

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct ShadowMapConstants
{
	Mat44 LightWorldToClipTransform;
	float TexelSize[2];
	float DepthBias;
	float Padding;
};
static constexpr int k_shadowMapBufferSlot = 4;

static constexpr int k_shadowMapTextureSlot = 1;
static constexpr int k_shadowMapSamplerSlot = 1;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ShadowMap::ShadowMap(ShadowMapConfig const& config)
	: m_config(config)
{
	IntVec2 textureDimensions = IntVec2(m_config.m_size, m_config.m_size);
	m_shadowMapTexture = m_config.m_renderer->CreateShadowMapTexture(m_config.m_name.c_str(), textureDimensions);

	m_camera.m_viewportBounds.m_mins = Vec2::ZERO;
	m_camera.m_viewportBounds.m_maxs = Vec2(static_cast<float>(m_config.m_size));
//	m_camera.SetOrthographicView(m_camera.m_viewportBounds, 0.f, 100.f);

	m_config.m_shadowMapShader->ReleasePixelShader();

	m_CBO = m_config.m_renderer->CreateConstantBuffer(sizeof(ShadowMapConstants));
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ShadowMap::~ShadowMap()
{
	delete m_CBO;
	m_CBO = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
std::string ShadowMap::GetName() const
{
	return m_config.m_name;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Texture* ShadowMap::GetTexture() const
{
	return m_shadowMapTexture;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ShadowMap::BeginDepthPass()
{
	ID3D11DeviceContext*	deviceContext = m_config.m_renderer->m_deviceContext;
	ID3D11DepthStencilView* dsv = m_shadowMapTexture->GetDSV();

	deviceContext->OMSetRenderTargets(0, nullptr, dsv);
	deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.f, 0);

	m_config.m_renderer->BeginCamera(m_camera);
	m_config.m_renderer->BindShader(m_config.m_shadowMapShader);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ShadowMap::EndDepthPass()
{
	m_config.m_renderer->ResetToDefaultRenderTargets();
	m_config.m_renderer->BindShader(nullptr);
	m_config.m_renderer->EndCamera(m_camera);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ShadowMap::BindAsTexture()
{
	ID3D11DeviceContext* deviceContext = m_config.m_renderer->m_deviceContext;
	ID3D11ShaderResourceView* srv = m_shadowMapTexture->GetSRV();
	ID3D11SamplerState* samplerState = m_config.m_renderer->GetSamplerStateForMode(SamplerMode::BILINEAR_COMPARISION_BORDER);

	deviceContext->PSSetShaderResources(k_shadowMapTextureSlot, 1, &srv);
	deviceContext->PSSetSamplers(k_shadowMapSamplerSlot, 1, &samplerState);

	ShadowMapConstants shadowMapConstants;
	shadowMapConstants.LightWorldToClipTransform = m_camera.GetWorldToClipTransform();
	shadowMapConstants.DepthBias = m_config.m_depthBias;
	shadowMapConstants.TexelSize[0] = 1.f / static_cast<float>(m_config.m_size);
	shadowMapConstants.TexelSize[1] = 1.f / static_cast<float>(m_config.m_size);

//	Mat44 lightTansform = m_camera.GetWorldToClipTransform();
	m_config.m_renderer->CopyCPUToGPU(&shadowMapConstants, sizeof(ShadowMapConstants), m_CBO);
	m_config.m_renderer->BindConstantBuffer(k_shadowMapBufferSlot, m_CBO);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void ShadowMap::UnbindAsTexture()
{
	ID3D11DeviceContext* deviceContext = m_config.m_renderer->m_deviceContext;
	ID3D11ShaderResourceView* nullSRV[] = {nullptr};

	deviceContext->PSSetShaderResources(k_shadowMapTextureSlot, 1, &nullSRV[0]);
}

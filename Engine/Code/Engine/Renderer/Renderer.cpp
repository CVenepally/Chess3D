#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Image.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/DefaultShader.hpp"
#include "Engine/Renderer/Light.hpp"

#include "Engine/Window/Window.hpp"

#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_PCUTBN.hpp"

#include "ThirdParty/stb/stb_image.h"
#include "Game/EngineBuildPreferences.hpp"

#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

//------------------------------------------------------------------------------------------------------------------
DWORD shaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct DebugConstants
{
	float	Time;
	int		DebugInt;
	float	DebugFloat;
	int		Padding;
};
static const int k_debugConstantsSlot = 1;


struct CameraConstants
{
	Mat44 WorldToCameraTransform;
	Mat44 CameraToRenderTransform;
	Mat44 RenderToClipTransform;

};
static const int k_cameraConstantsSlot = 2;


struct ModelConstants
{
	Mat44 ModelToWorldTransform;
	float ModelColor[4];
};
static const int k_modelConstantsSlot = 3;

struct LightConstants
{
	Light DirectionalLight; 
	//------------------- 16*5

	Light AllLights[MAX_LIGHTS];
	//------------------- 16*5

	Vec3  CameraPosition;
	int	  NumLights;  
	//------------------16

	float AmbientIntensity;
	Vec3 _padding0;
	//------------------16
};
static const int k_lightConstantsSlot = 4;

//------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern Window* g_theWindow;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Renderer::Renderer(RenderConfig const& config)
	:m_config(config)

{

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Renderer::~Renderer()
{

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::Startup()
{
	unsigned int deviceFlags = 0;

#if defined(ENGINE_DEBUG_RENDER)
	m_dxgiDebugModule = (void*)::LoadLibraryA("dxgidebug.dll");
	if(m_dxgiDebugModule == nullptr)
	{
		ERROR_AND_DIE("Could not load dxgidebug.dll");
	}

	typedef HRESULT(WINAPI* GetDebugModuleCB)(REFIID, void**);
	((GetDebugModuleCB)::GetProcAddress((HMODULE)m_dxgiDebugModule, "DXGIGetDebugInterface"))
		(_uuidof(IDXGIDebug), &m_dxgiDebug);

	if(m_dxgiDebug == nullptr)
	{
		ERROR_AND_DIE("Could not load debug module");
	}

	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

	shaderFlags = D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	shaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	CreateSwapChain(deviceFlags);

	CreateBackBufferAndRenderTargetView();

	m_defaultShader = CreateShader("Default", defaultShader);
 	BindShader(m_defaultShader);

	CreateVertexAndIndexBuffers();
	
	CreateRasterizerStates();
	m_deviceContext->RSSetState(m_rasterizerState);

	CreateConstantBuffers();
	CreateBlendStates();
	
	Image defaultTextureImage = Image(IntVec2(1, 1), Rgba8::WHITE);

	m_defaultTexture = CreateTextureFromImage(defaultTextureImage);
	m_defaultTexture->m_name = "Default";

	BindTexture(m_defaultTexture);

	CreateSamplerStates();
	CreateDepthStencilTexture();
	CreateDepthStencilStates();

	m_desiredBlendMode		= BlendMode::ALPHA;
	for(int slot = 0; slot < 16; ++slot)
	{
		m_desiredSamplerMode[slot] = SamplerMode::POINT_CLAMP;
	}
	m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	m_desiredDepthMode		= DepthMode::READ_WRITE_LESS_EQUAL;

	HRESULT hr;

	hr = m_deviceContext->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), reinterpret_cast<void**>(&m_userDefinedAnnotations));
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create user defined annotations interface!");
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::BeginFrame()
{
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilDSV);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::EndFrame()
{
	if(m_config.m_window)
	{		
		HRESULT hr;
		hr = m_swapChain->Present(0, 0);
		if(hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			ERROR_AND_DIE("Device has been lost, application will now terminate");
		}
	}

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::Shutdown()
{

	for(int shaderIndex = 0; shaderIndex < static_cast<int>(m_loadedShaders.size()); ++shaderIndex)
	{
		if(m_loadedShaders[shaderIndex])
		{
			delete m_loadedShaders[shaderIndex];
			m_loadedShaders[shaderIndex] = nullptr;
		}
	}

	// Release Textures
	for(int textureIndex = 0; textureIndex < static_cast<int>(m_loadedTextures.size()); ++textureIndex)
	{
		if(m_loadedTextures[textureIndex])
		{
			delete m_loadedTextures[textureIndex];
			m_loadedTextures[textureIndex] = nullptr;
		}
	}

	// Release Fonts
	for(int fontIndex = 0; fontIndex < static_cast<int>(m_loadedFonts.size()); ++fontIndex)
	{
		if(m_loadedFonts[fontIndex])
		{
			delete m_loadedFonts[fontIndex];
			m_loadedFonts[fontIndex] = nullptr;
		}
	}

	m_defaultTexture = nullptr;
	m_defaultShader = nullptr;
	m_currentShader = nullptr;

	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	delete m_immediateVBO_PCUTBN;
	m_immediateVBO_PCUTBN = nullptr;

	delete m_immediateIBO;
	m_immediateIBO = nullptr;

	delete m_cameraCBO;
	m_cameraCBO = nullptr;

	delete m_modelCBO;
	m_modelCBO = nullptr;

	delete m_lightCBO;
	m_lightCBO = nullptr;

	delete m_skyboxCBO;
	m_skyboxCBO = nullptr;

	delete m_debugCBO;
	m_debugCBO = nullptr;
	// DirectX release
	for(int i = 0; i < static_cast<int>(BlendMode::COUNT); ++i)
	{
		DX_SAFE_RELEASE(m_blendStates[i]);
	}

	for(int i = 0; i < static_cast<int>(SamplerMode::COUNT); ++i)
	{
		DX_SAFE_RELEASE(m_samplerStates[i]);
	}

	for(int i = 0; i < static_cast<int>(RasterizerMode::COUNT); ++i)
	{
		DX_SAFE_RELEASE(m_rasterizerStates[i]);
	}

	for(int i = 0; i < static_cast<int>(DepthMode::COUNT); ++i)
	{
		DX_SAFE_RELEASE(m_depthStencilStates[i]);
	}

	DX_SAFE_RELEASE(m_userDefinedAnnotations);
	DX_SAFE_RELEASE(m_depthStencilDSV);
	DX_SAFE_RELEASE(m_depthStencilTexture);
	DX_SAFE_RELEASE(m_renderTargetView);
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_deviceContext);
	DX_SAFE_RELEASE(m_device);


#if defined(ENGINE_DEBUG_RENDER)
	((IDXGIDebug*)m_dxgiDebug)->ReportLiveObjects(
		DXGI_DEBUG_ALL,
		(DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
	);

	((IDXGIDebug*)m_dxgiDebug)->Release();
	m_dxgiDebug = nullptr;

	::FreeLibrary((HMODULE)m_dxgiDebugModule);
	m_dxgiDebugModule = nullptr;
#endif

}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::ClearScreen(const Rgba8& clearColor)  
{
	float colorAsFloats[4];
	clearColor.GetAsFloat(colorAsFloats);
	m_deviceContext->ClearDepthStencilView(m_depthStencilDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, colorAsFloats);
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::BeginCamera(const Camera& camera)
{
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = camera.m_viewportBounds.m_mins.x;
	viewport.TopLeftY = camera.m_viewportBounds.m_mins.y;
	viewport.Width = static_cast<float>(camera.m_viewportBounds.m_maxs.x);
	viewport.Height = static_cast<float>(camera.m_viewportBounds.m_maxs.y);
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	m_deviceContext->RSSetViewports(1, &viewport);

	CameraConstants cameraConsts;
	cameraConsts.WorldToCameraTransform = camera.GetWorldToCameraTransform();
	cameraConsts.CameraToRenderTransform = camera.GetCameraToRenderTransform();
 	cameraConsts.RenderToClipTransform = camera.GetRenderToClipTransform();

	ModelConstants modelConsts;
	modelConsts.ModelToWorldTransform = Mat44();
	modelConsts.ModelColor[0] = NormalizeByte(255);
	modelConsts.ModelColor[1] = NormalizeByte(255);
	modelConsts.ModelColor[2] = NormalizeByte(255);
	modelConsts.ModelColor[3] = NormalizeByte(255);

	CopyCPUToGPU(&cameraConsts, sizeof(CameraConstants), m_cameraCBO);
	BindConstantBuffer(k_cameraConstantsSlot, m_cameraCBO);

	CopyCPUToGPU(&modelConsts, sizeof(ModelConstants), m_modelCBO);
	BindConstantBuffer(k_modelConstantsSlot, m_modelCBO);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::EndCamera(const Camera& camera) 
{

	UNUSED(camera);

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::BeginRenderEvent(char const* eventName)
{
	int eventNameLength = static_cast<int>(strlen(eventName)) + 1;
	int eventNameWideCharLength = MultiByteToWideChar(CP_UTF8, 0, eventName, eventNameLength, NULL, 0);

	wchar_t* eventNameWideCharStr = new wchar_t[eventNameWideCharLength];
	MultiByteToWideChar(CP_UTF8, 0, eventName, eventNameLength, eventNameWideCharStr, eventNameWideCharLength);

	m_userDefinedAnnotations->BeginEvent(eventNameWideCharStr);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::EndRenderEvent(char const* eventName)
{
	UNUSED(eventName);
	m_userDefinedAnnotations->EndEvent();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes)
{
	unsigned int vboSize = numVertexes * sizeof(Vertex_PCU);

	CopyCPUToGPU(vertexes, vboSize, m_immediateVBO);
	DrawVertexBuffer(m_immediateVBO, numVertexes);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray(std::vector<Vertex_PCU> const& vertexes)
{
	DrawVertexArray(static_cast<int>(vertexes.size()), vertexes.data());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::DrawVertexArray(std::vector<Vertex_PCUTBN> const& vertexes)
{
	unsigned int vboSize = static_cast<unsigned int>(vertexes.size()) * sizeof(Vertex_PCUTBN);

	CopyCPUToGPU(vertexes.data(), vboSize, m_immediateVBO_PCUTBN);
	DrawVertexBuffer(m_immediateVBO_PCUTBN, static_cast<unsigned int>(vertexes.size()));
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::DrawIndexedVertexArray(std::vector<Vertex_PCUTBN> const& vertexes, std::vector<unsigned int> const& indices)
{
	unsigned int vboSize = static_cast<unsigned int>(vertexes.size()) * sizeof(Vertex_PCUTBN);
	unsigned int iboSize = static_cast<unsigned int>(indices.size()) * sizeof(unsigned int);

	CopyCPUToGPU(vertexes.data(), vboSize, m_immediateVBO_PCUTBN);
	CopyCPUToGPU(indices.data(), iboSize, m_immediateIBO);

	DrawIndexedVertexBuffer(m_immediateVBO_PCUTBN, m_immediateIBO, static_cast<unsigned int>(indices.size()));
}

//------------------------------------------------------------------------------------------------------------------
void Renderer::DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount)
{
	SetStatesIfChanged();
	BindVertexBuffer(vbo);
	m_deviceContext->Draw(vertexCount, 0);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::DrawIndexedVertexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, unsigned int indexCount)
{
	SetStatesIfChanged();
	BindVertexBuffer(vbo);
	BindIndexBuffer(ibo);
	m_deviceContext->DrawIndexed(indexCount, 0, 0);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::CreateSwapChain(unsigned int deviceFlags)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = m_config.m_window->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = m_config.m_window->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND)m_config.m_window->GetHWND();
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, deviceFlags, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, nullptr, &m_deviceContext);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create D3D 11 device and swap chain");
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::CreateBackBufferAndRenderTargetView()
{
	ID3D11Texture2D* backBuffer;
	HRESULT hr;

	hr = m_swapChain->GetBuffer(0, _uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not get swap chain buffer.");
	}

	hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create render target view for swap chain buffer.");
	}

	backBuffer->Release();
}

//------------------------------------------------------------------------------------------------------------------
void Renderer::CreateRasterizerStates()
{

	HRESULT hr;

	// SOLID_CULL_NONE
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.f;
	rasterizerDesc.SlopeScaledDepthBias = 0.f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[static_cast<int>(RasterizerMode::SOLID_CULL_NONE)]);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create rasterizer state for Mode SOLID_CULL_NONE");
	}

	// SOLID_CULL_BACK
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;

	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[static_cast<int>(RasterizerMode::SOLID_CULL_BACK)]);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create rasterizer state for Mode SOLID_CULL_BACK");
	}
	
	// WIREFRAME_CULL_NONE
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;


	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[static_cast<int>(RasterizerMode::WIREFRAME_CULL_NONE)]);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create rasterizer state for Mode WIREFRAME_CULL_NONE");
	}

	// WIREFRAME_CULL_BACK
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;


	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerStates[static_cast<int>(RasterizerMode::WIREFRAME_CULL_BACK)]);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create rasterizer state for Mode WIREFRAME_CULL_BACK");
	}

}

//------------------------------------------------------------------------------------------------------------------
void Renderer::SetRasterizerMode(RasterizerMode rasterizerMode)
{

	m_desiredRasterizerMode = rasterizerMode;

}

//------------------------------------------------------------------------------------------------------------------
void Renderer::CreateBlendStates()
{
	HRESULT result;

	// opaque
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::OPAQUE)]);

	if(!SUCCEEDED(result))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::OPAQUE failed");
	}

	// Alpha
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::ALPHA)]);

	if(!SUCCEEDED(result))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::ALPHA failed");
	}

	// additive
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
	blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
	blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	result = m_device->CreateBlendState(&blendDesc, &m_blendStates[(int)(BlendMode::ADDITIVE)]);

	if(!SUCCEEDED(result))
	{
		ERROR_AND_DIE("CreateBlendState for BlendMode::ADDITIVE failed");
	}
}

//------------------------------------------------------------------------------------------------------------------
void Renderer::SetBlendMode(BlendMode blendMode)
{
	
	m_desiredBlendMode = blendMode;

}

//------------------------------------------------------------------------------------------------------------------
void Renderer::SetStatesIfChanged()
{
	if(m_blendState != m_blendStates[static_cast<int>(m_desiredBlendMode)])
	{
		m_blendState = m_blendStates[static_cast<int>(m_desiredBlendMode)];
		float blendFactor[4] = {0.f, 0.f, 0.f, 0.f};
		UINT sampleMask = 0xffffffff;
		m_deviceContext->OMSetBlendState(m_blendState, blendFactor, sampleMask);
	}

	for(int slot = 0; slot < 16; ++slot)
	{
		if(m_samplerState[slot] != m_samplerStates[static_cast<int>(m_desiredSamplerMode[slot])])
		{
			m_samplerState[slot] = m_samplerStates[static_cast<int>(m_desiredSamplerMode[slot])];
			m_deviceContext->PSSetSamplers(slot, 1, &m_samplerState[slot]);
		}
	}
	
	if(m_rasterizerState != m_rasterizerStates[static_cast<int>(m_desiredRasterizerMode)])
	{
		m_rasterizerState = m_rasterizerStates[static_cast<int>(m_desiredRasterizerMode)];
		m_deviceContext->RSSetState(m_rasterizerState);
	}

	if(m_depthStencilState != m_depthStencilStates[static_cast<int>(m_desiredDepthMode)])
	{
		m_depthStencilState = m_depthStencilStates[static_cast<int>(m_desiredDepthMode)];
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	}

}

//------------------------------------------------------------------------------------------------------------------
void Renderer::CreateSamplerStates()
{
	HRESULT hr;
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[static_cast<int>(SamplerMode::POINT_CLAMP)]);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateSamplerState for SamplerMode::POINT_CLAMP failed");
	}
	
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[static_cast<int>(SamplerMode::BILINEAR_WRAP)]);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateSamplerState for SamplerMode::BILINEAR_WRAP failed");
	}

	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	samplerDesc.BorderColor[0] = 1.f;
	samplerDesc.BorderColor[1] = 1.f;
	samplerDesc.BorderColor[2] = 1.f;
	samplerDesc.BorderColor[3] = 1.f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[static_cast<int>(SamplerMode::BILINEAR_COMPARISION_BORDER)]);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("CreateSamplerState for SamplerMode::BILINEAR_COMPARISION_BORDER failed");
	}
}

//------------------------------------------------------------------------------------------------------------------
void Renderer::SetSamplerMode(SamplerMode samplerMode, int slot)
{
	m_desiredSamplerMode[slot] = samplerMode;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
ID3D11SamplerState* Renderer::GetSamplerStateForMode(SamplerMode samplerMode)
{
	return m_samplerStates[static_cast<int>(samplerMode)];
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDepthStencilTexture()
{

	D3D11_TEXTURE2D_DESC depthTextureDesc = {};
	depthTextureDesc.Width			  = m_config.m_window->GetClientDimensions().x;
	depthTextureDesc.Height			  = m_config.m_window->GetClientDimensions().y;
	depthTextureDesc.MipLevels		  = 1;
	depthTextureDesc.ArraySize		  = 1;
	depthTextureDesc.Usage			  = D3D11_USAGE_DEFAULT;
	depthTextureDesc.Format			  = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDesc.BindFlags		  = D3D11_BIND_DEPTH_STENCIL;
	depthTextureDesc.SampleDesc.Count = 1;

	HRESULT hr;
	hr = m_device->CreateTexture2D(&depthTextureDesc, nullptr, &m_depthStencilTexture);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create texture for depth stencil.");
	}

	hr = m_device->CreateDepthStencilView(m_depthStencilTexture, nullptr, &m_depthStencilDSV);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create depth stencil view.");
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::CreateDepthStencilStates()
{

	HRESULT hr;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	// disabled
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[static_cast<int>(DepthMode::DISABLED)]);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("FAILED to create Depth Stencil State for DepthMode::DISABLED\n(CreateDepthStencilStates())");
	}

	// READ_ONLY_ALWAYS
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[static_cast<int>(DepthMode::READ_ONLY_ALWAYS)]);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("FAILED to create Depth Stencil State for DepthMode::READ_ONLY_ALWAYS\n(CreateDepthStencilStates())");
	}

	//READ_ONLY_LESS_EQUAL
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[static_cast<int>(DepthMode::READ_ONLY_LESS_EQUAL)]);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("FAILED to create Depth Stencil State for DepthMode::READ_ONLY_LESS_EQUAL\n(CreateDepthStencilStates())");
	}

	// READ_WRITE_LESS_EQUAL
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[static_cast<int>(DepthMode::READ_WRITE_LESS_EQUAL)]);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("FAILED to create Depth Stencil State for DepthMode::READ_WRITE_LESS_EQUAL\n(CreateDepthStencilStates())");
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::SetDepthMode(DepthMode depthMode)
{

	m_desiredDepthMode = depthMode;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::SetModelConstants(Mat44 const& modelToWorldTransform, Rgba8 const& modelColor)
{
	ModelConstants modelConsts;
	modelConsts.ModelToWorldTransform = modelToWorldTransform;

	modelConsts.ModelColor[0] = NormalizeByte(modelColor.r);
	modelConsts.ModelColor[1] = NormalizeByte(modelColor.g);
	modelConsts.ModelColor[2] = NormalizeByte(modelColor.b);
	modelConsts.ModelColor[3] = NormalizeByte(modelColor.a);

	CopyCPUToGPU(&modelConsts, sizeof(ModelConstants), m_modelCBO);
	BindConstantBuffer(k_modelConstantsSlot, m_modelCBO);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::SetLightConstants(Light const& directionalLight, std::vector<Light> const& allLights, Vec3 const& cameraPosition, float ambientIntensity)
{
	LightConstants lightConstants;
	lightConstants.DirectionalLight = directionalLight;
	lightConstants.CameraPosition   = cameraPosition;
	lightConstants.NumLights        = static_cast<int>(allLights.size());
	lightConstants.AmbientIntensity = ambientIntensity;

	for(int index = 0; index < static_cast<int>(allLights.size()); ++index)
	{
		lightConstants.AllLights[index] = allLights[index];
	}

	CopyCPUToGPU(&lightConstants, sizeof(LightConstants), m_lightCBO);
	BindConstantBuffer(k_lightConstantsSlot, m_lightCBO);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::SetDebugConstants(float time, int debugInt, float debugFloat)
{
	DebugConstants debugConsts;
	debugConsts.Time		= time;
	debugConsts.DebugInt	= debugInt;
	debugConsts.DebugFloat	= debugFloat;

	CopyCPUToGPU(&debugConsts, sizeof(debugConsts), m_debugCBO);
	BindConstantBuffer(k_debugConstantsSlot, m_debugCBO);
}

//------------------------------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTextureFromFilePath(char const* imageFilePath)
{

	Texture* existingTexture = GetTextureForFileName(imageFilePath);
	if(existingTexture)
	{
		return existingTexture;
	}

	Image textureImage = CreateImageFromFile(imageFilePath);

	Texture* newTexture = CreateTextureFromImage(textureImage);
	return newTexture;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTextureFromFileNameAndType(std::string const& imageName, TextureType textureType)
{

	std::string textureName;

	switch(textureType)
	{
		case TextureType::DIFFUSE:
		{
			textureName = Stringf("%s_d", imageName.c_str());
			break;
		}
		case TextureType::NORMAL:
		{
			textureName = Stringf("%s_n", imageName.c_str());
			break;
		}
		case TextureType::SGE:
		{
			textureName = Stringf("%s_sge", imageName.c_str());
			break;
		}
		default:
			break;
	}

	std::string imageFilePath = Stringf("Data/Images/Textures/%s.png", textureName.c_str());


	Texture* existingTexture = GetTextureForFileName(imageFilePath.c_str());
	if(existingTexture)
	{
		return existingTexture;
	}

	Image textureImage = CreateImageFromFile(imageFilePath.c_str());

	Texture* newTexture = CreateTextureFromImage(textureImage);
	return newTexture;
}


//------------------------------------------------------------------------------------------------------------------
Texture* Renderer::GetTextureForFileName(char const* imageFilePath)
{
	
	for(size_t texture = 0; texture < m_loadedTextures.size(); texture++)
	{
		if(m_loadedTextures[texture]->GetImageFilePath() == imageFilePath)
		{

			return m_loadedTextures[texture];

		}
	}

	return nullptr;

}


//------------------------------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromImage(Image& textureImage)
{
	HRESULT hr;

	Texture* newTexture = new Texture();
	newTexture->m_name = textureImage.GetImageFilePath();
	newTexture->m_dimensions = textureImage.GetDimensions();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = textureImage.GetDimensions().x;
	textureDesc.Height = textureImage.GetDimensions().y;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureData;
	textureData.pSysMem = textureImage.GetRawData();
	textureData.SysMemPitch = 4 * textureImage.GetDimensions().x;

	hr = m_device->CreateTexture2D(&textureDesc, &textureData, &newTexture->m_texture);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateTextureFromImage failed for image file \"%s\".", textureImage.GetImageFilePath().c_str()));
	}

	hr = m_device->CreateShaderResourceView(newTexture->m_texture, NULL, &newTexture->m_shaderResourceView);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateShaderResourceView failed for image file \"%s\".", textureImage.GetImageFilePath().c_str()));
	}
	m_loadedTextures.push_back(newTexture);

	return newTexture;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Texture* Renderer::CreateShadowMapTexture(char const* textureName, IntVec2 dimensions)
{
	HRESULT hr;

	Texture* shadowMapTexture = new Texture();
	shadowMapTexture->m_name = textureName;
	shadowMapTexture->m_dimensions = dimensions;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width				= dimensions.x;
	textureDesc.Height				= dimensions.y;
	textureDesc.MipLevels			= 1;
	textureDesc.ArraySize			= 1;
	textureDesc.Format				= DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count	= 1;
	textureDesc.Usage				= D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

	hr = m_device->CreateTexture2D(&textureDesc, nullptr, &shadowMapTexture->m_texture);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("Could not create Shadow Map Texture \"%s\".", textureName));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format						= DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip	= 0;
	srvDesc.Texture2D.MipLevels			= 1;

	hr = m_device->CreateShaderResourceView(shadowMapTexture->m_texture, &srvDesc, &shadowMapTexture->m_shaderResourceView);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateShaderResourceView failed for image file \"%s\".", textureName));
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = m_device->CreateDepthStencilView(shadowMapTexture->m_texture, &dsvDesc, &shadowMapTexture->m_depthStencilView);
	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE(Stringf("CreateDepthStencilView failed for image file \"%s\".", textureName));
	}

	m_loadedTextures.push_back(shadowMapTexture);
	return shadowMapTexture;
}

//------------------------------------------------------------------------------------------------------------------
Image Renderer::CreateImageFromFile(char const* imageFilePath)
{

	Image textureImage = Image(imageFilePath);
	return textureImage;

}

//------------------------------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateOrGetBitmapFont(char const* bitmapFontFilePathWithNoExtension)
{
	
	BitmapFont* existingFont = GetBitmapFont(bitmapFontFilePathWithNoExtension);

	if(existingFont)
	{
		return existingFont;
	}

	BitmapFont* newFont = CreateBitmapFont(bitmapFontFilePathWithNoExtension);
	
	m_loadedFonts.push_back(newFont);

	return newFont;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateOrGetBitmapFontWithFontName(std::string fontName)
{

	std::string fontFilePath = "Data/Fonts/" + fontName;

	return CreateOrGetBitmapFont(fontFilePath.c_str());
}

//------------------------------------------------------------------------------------------------------------------
BitmapFont* Renderer::CreateBitmapFont(char const* bitmapFontFilePathWithNoExtension)
{

	std::string textureFilePath = Stringf("%s.png", bitmapFontFilePathWithNoExtension);

	Image fontSheet = Image(textureFilePath.c_str());

	Texture* fontTexture = CreateTextureFromImage(fontSheet);

	BitmapFont* font = new BitmapFont(bitmapFontFilePathWithNoExtension, *fontTexture);

	return font;

}


//------------------------------------------------------------------------------------------------------------------
BitmapFont* Renderer::GetBitmapFont(char const* bitmapFontFilePathWithNoExtension)
{
	for(size_t fonts = 0; fonts < m_loadedFonts.size(); fonts++)
	{
		if(m_loadedFonts[fonts]->GetImageFilePath() == bitmapFontFilePathWithNoExtension)
		{

			return m_loadedFonts[fonts];

		}
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Shader* Renderer::CreateOrGetShader(char const* shaderName, InputLayoutType type)
{

	Shader* shader = GetShader(shaderName);
	
	if(!shader)
	{
		shader = CreateShader(shaderName, type);
	}

	return shader;
}

//------------------------------------------------------------------------------------------------------------------
Shader* Renderer::CreateShader(char const* shaderName, char const* shaderSource, InputLayoutType type)
{

	ShaderConfig shaderConfig;
	shaderConfig.m_name = shaderName;

	Shader* shader = new Shader(shaderConfig);
	
	std::vector<uint8_t> vertexShaderByteCode;
	std::vector<uint8_t> pixelShaderByteCode;

	char const* vertexShaderEntryPoint = shader->m_config.m_vertexEntryPoint.c_str();
	char const* pixelShaderEntryPoint = shader->m_config.m_pixelEntryPoint.c_str();

	CreateVertexShader(shader, shaderName, shaderSource, vertexShaderByteCode, vertexShaderEntryPoint);

	CreatePixelShader(shader, shaderName, shaderSource, pixelShaderByteCode, pixelShaderEntryPoint);

	CreateInputLayout(shader, vertexShaderByteCode, type);

	m_loadedShaders.push_back(shader);

	return shader;

}


//------------------------------------------------------------------------------------------------------------------
Shader* Renderer::CreateShader(char const* shaderName, InputLayoutType type)
{
	std::string shaderFilePath = Stringf("%s.hlsl", shaderName);

	std::string shaderData;

	bool result = FileReadToString(shaderData, shaderFilePath);

	if(!result)
	{
		ERROR_RECOVERABLE(Stringf("Could not load shader from file: %s", shaderFilePath.c_str()));
	}

	Shader* newShader = CreateShader(shaderName, shaderData.c_str(), type);

	return newShader;

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
Shader* Renderer::GetShader(char const* shaderName)
{
	for(size_t shaderIndex = 0; shaderIndex < m_loadedShaders.size(); ++shaderIndex)
	{
		if(m_loadedShaders[shaderIndex]->GetName() == shaderName)
		{
			return m_loadedShaders[shaderIndex];
		}
	}

	return nullptr;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::CreateConstantBuffers()
{
	m_cameraCBO = CreateConstantBuffer(sizeof(CameraConstants));
	m_modelCBO	= CreateConstantBuffer(sizeof(ModelConstants));
	m_lightCBO	= CreateConstantBuffer(sizeof(LightConstants));
	m_debugCBO	= CreateConstantBuffer(sizeof(DebugConstants));
}

//------------------------------------------------------------------------------------------------------------------
VertexBuffer* Renderer::CreateVertexBuffer(const unsigned int size, const unsigned int stride)
{
	VertexBuffer* vertexBuffer = new VertexBuffer(m_device, size, stride);
	return vertexBuffer;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
IndexBuffer* Renderer::CreateIndexBuffer(const unsigned int size, const unsigned int stride)
{
	IndexBuffer* indexBuffer = new IndexBuffer(m_device, size, stride);
	return indexBuffer;
}

//------------------------------------------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU(const void* data, unsigned int size, VertexBuffer* vbo)
{
	if(vbo->GetSize() < size)
	{
		vbo->Resize(size);
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(vbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, (size_t)size);
	m_deviceContext->Unmap(vbo->m_buffer, 0);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU(const void* data, unsigned int size, IndexBuffer* ibo)
{

	if(ibo->GetSize() < size)
	{
		ibo->Resize(size);
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(ibo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, (size_t)size);
	m_deviceContext->Unmap(ibo->m_buffer, 0);

}

//------------------------------------------------------------------------------------------------------------------
ConstantBuffer* Renderer::CreateConstantBuffer(const unsigned int size)
{
	ConstantBuffer* constantBuffer = new ConstantBuffer(m_device, size);
	return constantBuffer;
}

//------------------------------------------------------------------------------------------------------------------
void Renderer::CopyCPUToGPU(const void* data, unsigned int size, ConstantBuffer* cbo)
{

	D3D11_MAPPED_SUBRESOURCE resource;
	m_deviceContext->Map(cbo->m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, (size_t)size);
	m_deviceContext->Unmap(cbo->m_buffer, 0);

}

//------------------------------------------------------------------------------------------------------------------
bool Renderer::CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target)
{

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT result;

	result = D3DCompile(source, strlen(source), name, nullptr, nullptr, entryPoint, target, shaderFlags, 0, &shaderBlob, &errorBlob);

	if(SUCCEEDED(result))
	{
		outByteCode.resize(shaderBlob->GetBufferSize());

		memcpy(outByteCode.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

		shaderBlob->Release();
		shaderBlob = nullptr;

		if(errorBlob != nullptr)
		{
			errorBlob->Release();
			errorBlob = nullptr;
		}

		return true;
	}
	else
	{
		if(shaderBlob != nullptr)
		{
			shaderBlob->Release();
			shaderBlob = nullptr;
		}

		if(errorBlob != nullptr)
		{
			DebuggerPrintf(static_cast<char*>(errorBlob->GetBufferPointer()));
			errorBlob->Release();
			errorBlob = nullptr;
		}

		return false;
	}

}

//------------------------------------------------------------------------------------------------------------------
void Renderer::BindShader(Shader* shader)
{

	if(shader == nullptr)
	{
		shader = m_defaultShader;
	}

	m_deviceContext->IASetInputLayout(shader->m_inputLayout);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_deviceContext->VSSetShader(shader->m_vertexShader, nullptr, 0);
	m_deviceContext->PSSetShader(shader->m_pixelShader, nullptr, 0);

}

//------------------------------------------------------------------------------------------------------------------
void Renderer::BindVertexBuffer(VertexBuffer* vbo)
{
	UINT startOffset = 0;

	m_deviceContext->IASetVertexBuffers(0, 1, &vbo->m_buffer, &vbo->m_stride, &startOffset);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::BindIndexBuffer(IndexBuffer* ibo)
{
	m_deviceContext->IASetIndexBuffer(ibo->m_buffer, DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//------------------------------------------------------------------------------------------------------------------
void Renderer::BindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	m_deviceContext->VSSetConstantBuffers(slot, 1, &cbo->m_buffer);
	m_deviceContext->PSSetConstantBuffers(slot, 1, &cbo->m_buffer);
}

//------------------------------------------------------------------------------------------------------------------
void Renderer::BindTexture(Texture* texture, int slot)
{
	if(!texture)
	{
		texture = m_defaultTexture;
	}

	m_deviceContext->PSSetShaderResources(slot, 1, &texture->m_shaderResourceView);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::ResetToDefaultRenderTargets()
{
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilDSV);
	m_deviceContext->ClearDepthStencilView(m_depthStencilDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

//------------------------------------------------------------------------------------------------------------------
void Renderer::CreateVertexShader(Shader* shader, char const* shaderName, char const* shaderSource, std::vector<uint8_t>& vsByteCode, char const* vsEntryPoint)
{
	bool compileResult;
	HRESULT shaderResult;

	compileResult = CompileShaderToByteCode(vsByteCode, shaderName, shaderSource, vsEntryPoint, "vs_5_0");

	if(!compileResult)
	{
		ERROR_AND_DIE("Could not compile vertex shader");
	}

	shaderResult = m_device->CreateVertexShader(vsByteCode.data(), vsByteCode.size(), nullptr, &shader->m_vertexShader);

	if(!SUCCEEDED(shaderResult))
	{
		ERROR_AND_DIE("Could not create a vertex shader");
	}
}

//------------------------------------------------------------------------------------------------------------------
void Renderer::CreatePixelShader(Shader* shader, char const* shaderName, char const* shaderSource, std::vector<uint8_t>& psByteCode, char const* psEntryPoint)
{
	bool compileResult;
	HRESULT shaderResult;

	compileResult = CompileShaderToByteCode(psByteCode, shaderName, shaderSource, psEntryPoint, "ps_5_0");

	if(!compileResult)
	{
		ERROR_AND_DIE("Could not compile pixel shader");
	}

	shaderResult = m_device->CreatePixelShader(psByteCode.data(), psByteCode.size(), nullptr, &shader->m_pixelShader);

	if(!SUCCEEDED(shaderResult))
	{
		ERROR_AND_DIE("Could not create a pixel shader");
	}

}

//------------------------------------------------------------------------------------------------------------------
void Renderer::CreateInputLayout(Shader* shader, std::vector<uint8_t> const& vsByteCode, InputLayoutType type)
{
	HRESULT shaderResult;

	switch(type)
	{
		case InputLayoutType::VERTEX_P:
		{
			D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			UINT numElements = ARRAYSIZE(inputElementDesc);
			shaderResult = m_device->CreateInputLayout(inputElementDesc, numElements, vsByteCode.data(), vsByteCode.size(), &shader->m_inputLayout);

			if(!SUCCEEDED(shaderResult))
			{
				ERROR_AND_DIE("Could not create vertex layout for positions");
			}
			break;
		}

		case InputLayoutType::VERTEX_PCU:
		{
			D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,							  D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR",	 0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			UINT numElements = ARRAYSIZE(inputElementDesc);
			shaderResult = m_device->CreateInputLayout(inputElementDesc, numElements, vsByteCode.data(), vsByteCode.size(), &shader->m_inputLayout);

			if(!SUCCEEDED(shaderResult))
			{
				ERROR_AND_DIE("Could not create vertex layout for VPCU");
			}
			break;
		}

		case InputLayoutType::VERTEX_PCUTBN:
		{
			D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
				{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,							 D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR",	  0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			UINT numElements = ARRAYSIZE(inputElementDesc);
			shaderResult = m_device->CreateInputLayout(inputElementDesc, numElements, vsByteCode.data(), vsByteCode.size(), &shader->m_inputLayout);

			if(!SUCCEEDED(shaderResult))
			{
				ERROR_AND_DIE("Could not create vertex layout for VPCU");
			}
			break;
		}

		default:
			break;
	}



}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Renderer::CreateVertexAndIndexBuffers()
{
	m_immediateVBO		  = CreateVertexBuffer(sizeof(Vertex_PCU), sizeof(Vertex_PCU));
	m_immediateVBO_PCUTBN = CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_immediateIBO		  = CreateIndexBuffer(sizeof(unsigned int), sizeof(unsigned int));
}


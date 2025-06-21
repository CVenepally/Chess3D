#pragma once

#if defined (OPAQUE)
#undef OPAQUE
#endif
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>
#include <string>
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11InputLayout;
struct ID3D11BlendState;
struct ID3D11SamplerState;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3DUserDefinedAnnotation;
struct IntVec2;
struct Rgba8;
struct Vertex_PCU;
struct Vertex_PCUTBN;
struct Light;

class Camera;
class Window;
class Texture;
class Image;
class Shader;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class BitmapFont; 								

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
struct RenderConfig
{
	Window* m_window = nullptr;
};
//------------------------------------------------------------------------------------------------------------------
enum class InputLayoutType
{
	VERTEX_P,
	VERTEX_PCU,
	VERTEX_PCUTBN,

	COUNT
};

enum class BlendMode
{
	ALPHA,
	ADDITIVE,
	OPAQUE,

	COUNT
};

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	BILINEAR_COMPARISION_BORDER,

	COUNT
};

enum class RasterizerMode
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,

	COUNT
};

enum class DepthMode
{
	DISABLED,
	READ_ONLY_ALWAYS,
	READ_ONLY_LESS_EQUAL,
	READ_WRITE_LESS_EQUAL,

	COUNT
};

enum class TextureType
{
	NONE = -1,

	DIFFUSE,
	NORMAL,
	SGE

};
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

class Renderer
{

public:
	Renderer(RenderConfig const& config);
	~Renderer();

	void						Startup();
	void						BeginFrame();
	void						EndFrame();
	void						Shutdown();
								
	void						ClearScreen(const Rgba8& clearColor);
	void						BeginCamera(const Camera& camera);
	void						EndCamera(const Camera& camera);

	void						BeginRenderEvent(char const* eventName);
	void						EndRenderEvent(char const* eventName);

	void						DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes);
	void						DrawVertexArray(std::vector<Vertex_PCU> const& vertexes);
	void						DrawVertexArray(std::vector<Vertex_PCUTBN> const& vertexes);
	void						DrawIndexedVertexArray(std::vector<Vertex_PCUTBN> const& vertexes, std::vector<unsigned int> const& indices);
	void						DrawVertexBuffer(VertexBuffer* vbo, unsigned int vertexCount);
	void						DrawIndexedVertexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, unsigned int indexCount);
	
	void						CreateSwapChain(unsigned int deviceFlags);
	void						CreateBackBufferAndRenderTargetView();
	
	void						SetStatesIfChanged();

	void						CreateRasterizerStates();
	void						SetRasterizerMode(RasterizerMode rasterizerMode);

	void						CreateBlendStates();
	void						SetBlendMode(BlendMode blendMode);
	
	void						CreateSamplerStates();
	void						SetSamplerMode(SamplerMode samplerMode, int slot = 0);
	ID3D11SamplerState*			GetSamplerStateForMode(SamplerMode samplerMode);

	void						CreateDepthStencilTexture();
	
	void						CreateDepthStencilStates();
	void						SetDepthMode(DepthMode depthMode);

	void						SetModelConstants(Mat44 const& modelToWorldTransform = Mat44(), Rgba8 const& modelColor = Rgba8::WHITE);
	void						SetLightConstants(Light const& directionalLight, std::vector<Light> const& allLights, Vec3 const& cameraPositon, float ambientIntensity);
	void						SetDebugConstants(float time, int debugInt = 0, float debugFloat = 0.f);

	VertexBuffer*				CreateVertexBuffer(const unsigned int size, const unsigned int stride);
	IndexBuffer*				CreateIndexBuffer(const unsigned int size, const unsigned int stride);
	ConstantBuffer*				CreateConstantBuffer(const unsigned int size);

	// #ToDo: FIX EVERY SINGLE GAME
	Texture*					CreateOrGetTextureFromFilePath(char const* imageFilePath);
	Texture*					CreateOrGetTextureFromFileNameAndType(std::string const& imageName, TextureType textureType = TextureType::DIFFUSE);
	Texture*					GetTextureForFileName(char const* imageFilePath);
	Texture*					CreateTextureFromImage(Image& textureImage);
	Texture*					CreateShadowMapTexture(char const* textureName, IntVec2 dimensions);
	Image						CreateImageFromFile(char const* imageFilePath);
		
	BitmapFont*					CreateOrGetBitmapFont(char const* bitmapFontFilePathWithNoExtension);
	BitmapFont*					CreateOrGetBitmapFontWithFontName(std::string fontName);
	BitmapFont*					CreateBitmapFont(char const* bitmapFontFilePathWithNoExtension);
	BitmapFont*					GetBitmapFont(char const* bitmapFontFilePathWithNoExtension);
								
	Shader*						CreateOrGetShader(char const* shaderName, InputLayoutType type = InputLayoutType::VERTEX_PCU);
	Shader*						CreateShader(char const* shaderName, char const* shaderSource, InputLayoutType type = InputLayoutType::VERTEX_PCU);
	Shader*						CreateShader(char const* shaderName, InputLayoutType type = InputLayoutType::VERTEX_PCU);
	Shader*						GetShader(char const* shaderName);


	void						CopyCPUToGPU(const void* data, unsigned int size, VertexBuffer* vbo);
	void						CopyCPUToGPU(const void* data, unsigned int size, IndexBuffer* ibo);
	void						CopyCPUToGPU(const void* data, unsigned int size, ConstantBuffer* cbo);

	bool						CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target);
	
	void						BindShader(Shader* shader);
	void						BindVertexBuffer(VertexBuffer* vbo);
	void						BindIndexBuffer(IndexBuffer* ibo);
	void						BindConstantBuffer(int slot, ConstantBuffer* cbo);
	void						BindTexture(Texture* texture, int slot = 0);

	void						ResetToDefaultRenderTargets();

private:

	void						CreateVertexShader(Shader* shader, char const* shaderName, char const* shaderSource, std::vector<uint8_t>& vsByteCode, char const* vsEntryPoint);
	void						CreatePixelShader(Shader* shader, char const* shaderName, char const* shaderSource, std::vector<uint8_t>& psByteCode, char const* psEntryPoint);
	void						CreateInputLayout(Shader* shader, std::vector<uint8_t> const& vsByteCode, InputLayoutType type = InputLayoutType::VERTEX_PCU);
	void						CreateVertexAndIndexBuffers();

	void						CreateConstantBuffers();

private:
	RenderConfig				m_config;
	std::vector<Texture*>		m_loadedTextures;
	std::vector<Shader*>		m_loadedShaders;
	std::vector<BitmapFont*>	m_loadedFonts;

	Shader*						m_currentShader				= nullptr;
	Shader*						m_defaultShader				= nullptr;
	Texture*					m_defaultTexture			= nullptr;
	
public:

	ID3D11Device*				m_device					= nullptr;
	ID3D11DeviceContext*		m_deviceContext				= nullptr;
	
protected:

	ID3D11RasterizerState*		m_rasterizerState			= nullptr;
	ID3D11RenderTargetView*		m_renderTargetView			= nullptr;
	IDXGISwapChain*				m_swapChain					= nullptr;
	VertexBuffer*				m_immediateVBO				= nullptr;
	VertexBuffer*				m_immediateVBO_PCUTBN		= nullptr;
	IndexBuffer*				m_immediateIBO				= nullptr;
	ConstantBuffer*				m_cameraCBO					= nullptr;	
	ConstantBuffer*				m_modelCBO					= nullptr;
	ConstantBuffer*				m_skyboxCBO					= nullptr;
	ConstantBuffer*				m_lightCBO					= nullptr;
	ConstantBuffer*				m_debugCBO					= nullptr;
	ID3D11BlendState*			m_blendState				= nullptr;
	ID3D11SamplerState*			m_samplerState[16]			= {};
	ID3D11DepthStencilState*	m_depthStencilState			= nullptr;
	ID3DUserDefinedAnnotation*	m_userDefinedAnnotations	= nullptr;

	ID3D11Texture2D*			m_depthStencilTexture		= nullptr;
	ID3D11DepthStencilView*		m_depthStencilDSV			= nullptr;
	
	ID3D11BlendState*			m_blendStates[static_cast<int>(BlendMode::COUNT)]			= {};
	ID3D11SamplerState*			m_samplerStates[static_cast<int>(SamplerMode::COUNT)]		= {};
	ID3D11RasterizerState*		m_rasterizerStates[static_cast<int>(RasterizerMode::COUNT)]	= {};
	ID3D11DepthStencilState*	m_depthStencilStates[static_cast<int>(DepthMode::COUNT)]	= {};


	BlendMode					m_desiredBlendMode			= BlendMode::ALPHA;
	SamplerMode					m_desiredSamplerMode[16]	= {};
	RasterizerMode				m_desiredRasterizerMode		= RasterizerMode::WIREFRAME_CULL_BACK;
	DepthMode					m_desiredDepthMode			= DepthMode::READ_WRITE_LESS_EQUAL;

	void*						m_dxgiDebug					= nullptr;
	void*						m_dxgiDebugModule			= nullptr;
	

};
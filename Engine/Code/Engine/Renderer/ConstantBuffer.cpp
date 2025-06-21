#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Core/EngineCommon.hpp"
//------------------------------------------------------------------------------------------------------------------
ConstantBuffer::ConstantBuffer(ID3D11Device* device, unsigned int size)
	: m_device(device)
	, m_size(size)
{
	Create();
}

//------------------------------------------------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}

//------------------------------------------------------------------------------------------------------------------
void ConstantBuffer::Create()
{
	HRESULT hr;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = m_size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_device->CreateBuffer(&bufferDesc, nullptr, &m_buffer);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create constant buffer");
	}
}

//------------------------------------------------------------------------------------------------------------------
unsigned int ConstantBuffer::GetSize()
{
	return m_size;
}
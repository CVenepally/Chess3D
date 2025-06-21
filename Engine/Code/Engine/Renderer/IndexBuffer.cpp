#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/EngineCommon.hpp"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
IndexBuffer::IndexBuffer(ID3D11Device* device, unsigned int size, unsigned int stride)
	: m_size(size)
	, m_stride(stride)
	, m_device(device)
{
	Create();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
IndexBuffer::~IndexBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
void IndexBuffer::Create()
{

	HRESULT hr;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = m_size;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = m_device->CreateBuffer(&bufferDesc, nullptr, &m_buffer);

	if(!SUCCEEDED(hr))
	{
		ERROR_AND_DIE("Could not create index buffer");
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------s
void IndexBuffer::Resize(unsigned int size)
{

	DX_SAFE_RELEASE(m_buffer);
	m_size = size;
	Create();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int IndexBuffer::GetSize()
{
	return m_size;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
unsigned int IndexBuffer::GetStride()
{
	return m_stride;
}

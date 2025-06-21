#pragma once

//------------------------------------------------------------------------------------------------------------------
struct ID3D11Buffer;
struct ID3D11Device;

//------------------------------------------------------------------------------------------------------------------
class IndexBuffer
{
	friend class Renderer;

public:

	IndexBuffer(ID3D11Device* device, unsigned int size, unsigned int stride);
	IndexBuffer(const IndexBuffer& copy) = delete;
	virtual ~IndexBuffer();

	void Create();
	void Resize(unsigned int size);

	unsigned int GetSize();
	unsigned int GetStride();

private:

	ID3D11Device* m_device = nullptr;
	ID3D11Buffer* m_buffer = nullptr;

	unsigned int m_size = 0;
	unsigned int m_stride = 0;
};
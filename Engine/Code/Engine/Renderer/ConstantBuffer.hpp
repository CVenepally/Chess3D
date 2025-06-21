#pragma once

//------------------------------------------------------------------------------------------------------------------
struct ID3D11Buffer;
struct ID3D11Device;

//------------------------------------------------------------------------------------------------------------------
class ConstantBuffer
{
	friend class Renderer;

public:
	ConstantBuffer(ID3D11Device* device, unsigned int size);
	ConstantBuffer(const ConstantBuffer& copy) = delete;
	virtual ~ConstantBuffer();

	void Create();
	
	unsigned int GetSize();


private:
	ID3D11Buffer* m_buffer = nullptr;
	ID3D11Device* m_device = nullptr;

	unsigned m_size = 0;
};
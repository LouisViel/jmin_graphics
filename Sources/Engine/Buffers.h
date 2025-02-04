#pragma once

using Microsoft::WRL::ComPtr;

template<typename TData>
class VertexBuffer {
private:
	ComPtr<ID3D11Buffer> buffer;
	std::vector<TData> data;

public:
	VertexBuffer() { }

	inline void Create(DeviceResources* ressources) { Create(ressources->GetD3DDevice()); }
	void Create(ID3D11Device1* device)
	{
		CD3D11_BUFFER_DESC desc(sizeof(TData) * data.size(), D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA subResData = { };
		subResData.pSysMem = data.data();
		device->CreateBuffer(&desc, &subResData, buffer.ReleaseAndGetAddressOf());
	}

	inline void Apply(DeviceResources* ressources, UINT slot = 0) { Apply(ressources->GetD3DDeviceContext(), slot); }
	void Apply(ID3D11DeviceContext1* context, UINT slot = 0)
	{
		ID3D11Buffer* vbs[] = { buffer.Get() };
		const unsigned int strides[] = { sizeof(TData) }; // TODO : revoir за, besoin potentiellement d'une multiplication nan ?
		const unsigned int offsets[] = { 0 };
		context->IASetVertexBuffers(slot, 1, vbs, strides, offsets);
	}

	std::vector<TData>* const get()
	{
		return &data;
	}
};


//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////


class IndexBuffer {
private:
	ComPtr<ID3D11Buffer> buffer;
	std::vector<uint32_t> indexs;

public:
	IndexBuffer() { }

	inline void Create(DeviceResources* ressources) { Create(ressources->GetD3DDevice()); }
	void Create(ID3D11Device1* device)
	{
		CD3D11_BUFFER_DESC desc(sizeof(uint32_t) * indexs.size(), D3D11_BIND_INDEX_BUFFER);
		D3D11_SUBRESOURCE_DATA subResData = { };
		subResData.pSysMem = indexs.data();
		device->CreateBuffer(&desc, &subResData, buffer.ReleaseAndGetAddressOf());
	}

	inline void Apply(DeviceResources* ressources) { Apply(ressources->GetD3DDeviceContext()); }
	void Apply(ID3D11DeviceContext1* context)
	{
		context->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

	std::vector<uint32_t>* const get()
	{
		return &indexs;
	}
};


//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////


template<typename TData>
class ConstantBuffer {
private:
	ComPtr<ID3D11Buffer> buffer;
	TData data;

public:
	ConstantBuffer() { }

	inline void Create(DeviceResources* ressources) { Create(ressources->GetD3DDevice()); }
	void Create(ID3D11Device1* device)
	{
		CD3D11_BUFFER_DESC desc(sizeof(TData), D3D11_BIND_CONSTANT_BUFFER);
		D3D11_SUBRESOURCE_DATA subResData = { };
		subResData.pSysMem = &data;
		device->CreateBuffer(&desc, &subResData, buffer.ReleaseAndGetAddressOf());
	}

	inline void Update(DeviceResources* ressources) { Update(ressources->GetD3DDeviceContext()); }
	void Update(ID3D11DeviceContext1* context) {
		context->UpdateSubresource(buffer.Get(), 0, nullptr, &data, 0, 0);
	}

	inline void Apply(DeviceResources* ressources, UINT slot = 0) { Apply(ressources->GetD3DDeviceContext(), slot); }
	void Apply(ID3D11DeviceContext1* context, UINT slot = 0)
	{
		ID3D11Buffer* cbs[] = { buffer.Get() };
		context->VSSetConstantBuffers(slot, 1, cbs);
	}

	inline void UpdateAndApply(DeviceResources* ressources, UINT slot = 0) { UpdateAndApply(ressources->GetD3DDeviceContext()); }
	void UpdateAndApply(ID3D11DeviceContext1* context, UINT slot = 0)
	{
		Update(context);
		Apply(context, slot);
	}

	TData* const get()
	{
		return &data;
	}
};
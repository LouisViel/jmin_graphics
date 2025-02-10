#pragma once

using Microsoft::WRL::ComPtr;

template<typename TVertex>
class VertexBuffer {
	ComPtr<ID3D11Buffer> buffer;
public:
	std::vector<TVertex> data;
	VertexBuffer() {};

	uint32_t PushVertex(TVertex v) {
		data.push_back(v);
		return data.size() - 1;
	}

	size_t Size() {
		return data.size();
	}

	void Create(DeviceResources* deviceRes) {
		CD3D11_BUFFER_DESC desc(
			sizeof(TVertex) * data.size(),
			D3D11_BIND_VERTEX_BUFFER
		);
		D3D11_SUBRESOURCE_DATA dataInitial = {};
		dataInitial.pSysMem = data.data();

		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			&dataInitial,
			buffer.ReleaseAndGetAddressOf()
		);
	}

	void UpdateBuffer(DeviceResources* deviceRes) {
		deviceRes->GetD3DDeviceContext()->UpdateSubresource(buffer.Get(), 0, nullptr, &data, 0, 0);
	}

	void Apply(DeviceResources* deviceRes, int slot = 0) {
		ID3D11Buffer* vbs[] = { buffer.Get() };
		const UINT strides[] = { sizeof(TVertex) };
		const UINT offsets[] = { 0 };
		deviceRes->GetD3DDeviceContext()->IASetVertexBuffers(slot, 1, vbs, strides, offsets);
	}
};

class IndexBuffer {
	ComPtr<ID3D11Buffer> buffer;
	std::vector<uint32_t> indices;
public:
	IndexBuffer() {};

	void PushTriangle(uint32_t a, uint32_t b, uint32_t c) {
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	}

	size_t Size() {
		return indices.size();
	}

	void Create(DeviceResources* deviceRes) {
		CD3D11_BUFFER_DESC desc(
			sizeof(uint32_t) * indices.size(),
			D3D11_BIND_INDEX_BUFFER
		);
		D3D11_SUBRESOURCE_DATA dataInitial = {};
		dataInitial.pSysMem = indices.data();

		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			&dataInitial,
			buffer.ReleaseAndGetAddressOf()
		);
	}

	void Apply(DeviceResources* deviceRes) {
		deviceRes->GetD3DDeviceContext()->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
};

template<typename TData>
class ConstantBuffer {
	ComPtr<ID3D11Buffer> buffer;
public:
	TData data;

	ConstantBuffer() {};

	void Create(DeviceResources* deviceRes) {
		CD3D11_BUFFER_DESC desc(sizeof(TData), D3D11_BIND_CONSTANT_BUFFER);
		deviceRes->GetD3DDevice()->CreateBuffer(
			&desc, nullptr,
			buffer.ReleaseAndGetAddressOf()
		);
	}

	void UpdateBuffer(DeviceResources* deviceRes) {
		deviceRes->GetD3DDeviceContext()->UpdateSubresource(buffer.Get(), 0, nullptr, &data, 0, 0);
	}

	void ApplyToVS(DeviceResources* deviceRes, int slot = 0) {
		ID3D11Buffer* cbs[] = { buffer.Get() };
		deviceRes->GetD3DDeviceContext()->VSSetConstantBuffers(slot, 1, cbs);
	}
};
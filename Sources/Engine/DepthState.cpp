#include "pch.h"

#include "DepthState.h"

using namespace DirectX;

DepthState::DepthState(bool depthRead, bool depthWrite, D3D11_COMPARISON_FUNC func) {
	desc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);
	desc.DepthEnable = depthRead;
	desc.DepthWriteMask = depthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = func;
}

void DepthState::Create(DeviceResources* deviceRes) {
	auto d3dDevice = deviceRes->GetD3DDevice();
	auto d3dContext = deviceRes->GetD3DDeviceContext();

	d3dDevice->CreateDepthStencilState(&desc, depthStencilState.ReleaseAndGetAddressOf());
}

void DepthState::Apply(DeviceResources* deviceRes) {
	auto d3dContext = deviceRes->GetD3DDeviceContext();
	
	d3dContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
}

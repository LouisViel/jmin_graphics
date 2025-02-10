#include "pch.h"

#include "BlendState.h"

using namespace DirectX;

BlendState::BlendState(D3D11_BLEND src, D3D11_BLEND dst, D3D11_BLEND_OP op) {
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0] = {
		true,
		src, dst, op,
		src, dst, op,
		D3D11_COLOR_WRITE_ENABLE_ALL
	};
}

BlendState::BlendState(D3D11_BLEND src, D3D11_BLEND dst, D3D11_BLEND_OP op, D3D11_BLEND srcAlpha, D3D11_BLEND dstAlpha, D3D11_BLEND_OP opAlpha) {
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0] = {
		true,
		src, dst, op,
		srcAlpha, dstAlpha, opAlpha,
		D3D11_COLOR_WRITE_ENABLE_ALL
	};
}

void BlendState::Create(DeviceResources* deviceRes) {
	auto d3dDevice = deviceRes->GetD3DDevice();
	auto d3dContext = deviceRes->GetD3DDeviceContext();

	d3dDevice->CreateBlendState(&desc, blendState.ReleaseAndGetAddressOf());
}

void BlendState::Apply(DeviceResources* deviceRes) {
	auto d3dContext = deviceRes->GetD3DDeviceContext();

	d3dContext->OMSetBlendState(blendState.Get(), nullptr, 0xffffffff);
}

#pragma once

using Microsoft::WRL::ComPtr;

class DepthState {
	CD3D11_DEPTH_STENCIL_DESC desc;
	ComPtr<ID3D11DepthStencilState> depthStencilState;
public:
	DepthState() : desc(D3D11_DEFAULT) {};
	DepthState(bool depthRead, bool depthWrite, D3D11_COMPARISON_FUNC func = D3D11_COMPARISON_LESS);
	
	void Create(DeviceResources* deviceRes);
	void Apply(DeviceResources* deviceRes);
};
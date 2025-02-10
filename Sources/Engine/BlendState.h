#pragma once

using Microsoft::WRL::ComPtr;

class BlendState {
	CD3D11_BLEND_DESC desc;
	ComPtr<ID3D11BlendState> blendState;
public:
	BlendState() : desc(D3D11_DEFAULT) {};
	BlendState(D3D11_BLEND src, D3D11_BLEND dst, D3D11_BLEND_OP op);
	BlendState(D3D11_BLEND src, D3D11_BLEND dst, D3D11_BLEND_OP op, D3D11_BLEND srcAlpha, D3D11_BLEND dstAlpha, D3D11_BLEND_OP opAlpha);
	
	void Create(DeviceResources* deviceRes);
	void Apply(DeviceResources* deviceRes);
};
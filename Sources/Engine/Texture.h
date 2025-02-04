#pragma once

using Microsoft::WRL::ComPtr;

class Texture {
	std::wstring textureName;

	ComPtr<ID3D11ShaderResourceView> textureRV;
	ComPtr<ID3D11SamplerState> samplerState;
public:
	Texture(std::wstring name) : textureName(name) {};

	void Create(DeviceResources* deviceRes);
	void Apply(DeviceResources* deviceRes);
};
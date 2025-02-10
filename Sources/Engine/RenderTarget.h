#pragma once

using Microsoft::WRL::ComPtr;

class RenderTarget {
public:
	enum Mode {
		COLOR_ONLY,
		DEPTH_ONLY,
		COLOR_DEPTH,
	};
private:
	DXGI_FORMAT colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	int width = 255;
	int height = 255;

	Mode mode = COLOR_ONLY;

	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11ShaderResourceView> shaderRV;

	ComPtr<ID3D11Texture2D> depthTexture;
	ComPtr<ID3D11DepthStencilView> depthStencilView;
	ComPtr<ID3D11ShaderResourceView> shaderRVDepth;
public:
	RenderTarget(int width, int height, Mode mode) : width(width), height(height), mode(mode) {};

	void Create(DeviceResources* deviceRes);
	void Clear(DeviceResources* deviceRes);
	void ApplyTarget(DeviceResources* deviceRes);
	void ApplyShaderResourcePS(DeviceResources* deviceRes, int slot);
};
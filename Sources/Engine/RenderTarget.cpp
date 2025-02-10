#include "pch.h"

#include "RenderTarget.h"

using namespace DirectX;

void RenderTarget::Create(DeviceResources* deviceRes) {
	auto d3dDevice = deviceRes->GetD3DDevice();
	auto d3dContext = deviceRes->GetD3DDeviceContext();

	if (mode != COLOR_ONLY) {
		CD3D11_TEXTURE2D_DESC descDS(DXGI_FORMAT_R32_TYPELESS, width, height, 1, 0,
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE);
		d3dDevice->CreateTexture2D(&descDS, 0, depthTexture.ReleaseAndGetAddressOf());

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D32_FLOAT);
		d3dDevice->CreateDepthStencilView(depthTexture.Get(), &depthStencilViewDesc, depthStencilView.ReleaseAndGetAddressOf());

		CD3D11_SHADER_RESOURCE_VIEW_DESC shaderRVDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT, 0, 1);
		d3dDevice->CreateShaderResourceView(depthTexture.Get(), &shaderRVDesc, shaderRVDepth.ReleaseAndGetAddressOf());
	}
	if (mode != DEPTH_ONLY) {
		CD3D11_TEXTURE2D_DESC descRT(colorFormat, width, height, 1, 0,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
		d3dDevice->CreateTexture2D(&descRT, 0, texture.ReleaseAndGetAddressOf());

		CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, colorFormat);
		d3dDevice->CreateRenderTargetView(texture.Get(), &renderTargetViewDesc, renderTargetView.ReleaseAndGetAddressOf());

		CD3D11_SHADER_RESOURCE_VIEW_DESC shaderRVDesc(D3D11_SRV_DIMENSION_TEXTURE2D, colorFormat, 0, 1);
		d3dDevice->CreateShaderResourceView(texture.Get(), &shaderRVDesc, shaderRV.ReleaseAndGetAddressOf());
	}
}

void RenderTarget::Clear(DeviceResources* deviceRes) {
	auto d3dDevice = deviceRes->GetD3DDevice();
	auto d3dContext = deviceRes->GetD3DDeviceContext();

	if (mode != DEPTH_ONLY)
		d3dContext->ClearRenderTargetView(renderTargetView.Get(), ColorsLinear::Black);

	if (mode != COLOR_ONLY)
		d3dContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderTarget::ApplyTarget(DeviceResources* deviceRes) {
	auto d3dDevice = deviceRes->GetD3DDevice();
	auto d3dContext = deviceRes->GetD3DDeviceContext();

	ID3D11RenderTargetView* rtvs[] = { renderTargetView.Get() };
	if (mode == DEPTH_ONLY)
		d3dContext->OMSetRenderTargets(0, nullptr, depthStencilView.Get());
	else if (mode == COLOR_ONLY)
		d3dContext->OMSetRenderTargets(1, rtvs, nullptr);
	else
		d3dContext->OMSetRenderTargets(1, rtvs, depthStencilView.Get());

	CD3D11_VIEWPORT viewport(0.0f, 0.0f, (float)width, (float)height);
	d3dContext->RSSetViewports(1, &viewport);
}

void RenderTarget::ApplyShaderResourcePS(DeviceResources* deviceRes, int slot) {
	auto d3dDevice = deviceRes->GetD3DDevice();
	auto d3dContext = deviceRes->GetD3DDeviceContext();

	ID3D11ShaderResourceView* srvs[] = { shaderRV.Get() };
	d3dContext->PSSetShaderResources(slot, 1, srvs);
}
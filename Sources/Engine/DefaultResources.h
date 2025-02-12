#pragma once

#include "BlendState.h"
#include "DepthState.h"
#include "Buffers.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class DefaultResources {
	static DefaultResources* instance;
public:
	BlendState opaque = BlendState();
	BlendState alphaBlend = BlendState(D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD);

	DepthState defaultDepth = DepthState(true, true);
	DepthState depthRead = DepthState(true, false);
	DepthState depthEqual = DepthState(true, false, D3D11_COMPARISON_EQUAL);
	DepthState noDepth = DepthState(false, false);

	struct ModelData {
		Matrix model;
	};
	ConstantBuffer<ModelData> cbModel;

	DefaultResources();
	static DefaultResources* Get() { return instance; }

	void Create(DeviceResources* deviceRes);
};
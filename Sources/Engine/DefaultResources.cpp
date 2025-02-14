#include "pch.h"

#include "DefaultResources.h"

DefaultResources* DefaultResources::instance = nullptr;

DefaultResources::DefaultResources() {
	assert(!instance);
	instance = this;
}

void DefaultResources::Create(DeviceResources* deviceRes) {
	opaque.Create(deviceRes);
	alphaBlend.Create(deviceRes);

	defaultDepth.Create(deviceRes);
	depthRead.Create(deviceRes);
	depthEqual.Create(deviceRes);
	noDepth.Create(deviceRes);

	cbModel.Create(deviceRes);
}

#pragma once

#include "./core/GPUBuffers.hpp"
#include "./core/Shader.hpp"

class IDrawable {
public:
	virtual void render(GPUBuffers*, Shader*) = 0;
	virtual ~IDrawable() {}
};

#pragma once

class IShaderable {
public:
	enum ShadingMode {
		FLAT_SHADING = 1,
		SMOOTH_SHADING, 
		NO_SHADING,
	};
public:
	virtual void apply_shading(ShadingMode mode) = 0;
	virtual ~IShaderable() {}
};

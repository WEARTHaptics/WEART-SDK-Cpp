#pragma once

#include "WeArtTemperature.h"
#include "WeArtForce.h"
#include "WeArtTexture.h"
#include <vector>

//! @brief General effect function interface
class WeArtEffect {
public:
	virtual WeArtTemperature	getTemperature(void) = 0;
	virtual WeArtForce			getForce(void) = 0;
	virtual WeArtTexture		getTexture(void) = 0;
};

//! @brief Basic touch effect object to apply effects
class TouchEffect : public WeArtEffect {
public:
	TouchEffect(WeArtTemperature temp, WeArtForce force, WeArtTexture texture)
		: _temperature(temp), _force(force), _texture(texture) {}

	//! @brief Set the effect parameters
	//! @param temp		Temperature to apply
	//! @param force	Force to apply
	//! @param texture	Texture to apply
	//! @return true if one of the parameters changed, false otherwise
	bool Set(WeArtTemperature temp, WeArtForce force, WeArtTexture texture);

	virtual WeArtTemperature getTemperature(void) override {
		return _temperature;
	}

	virtual WeArtForce getForce(void) override {
		return _force;
	}

	virtual WeArtTexture getTexture(void) override {
		return _texture;
	}

private:
	WeArtTemperature _temperature;
	WeArtForce _force;
	WeArtTexture _texture;
};
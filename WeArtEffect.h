#pragma once

#include "WeArtTemperature.h"
#include "WeArtForce.h"
#include "WeArtTexture.h"
#include <vector>

//! @brief General effect function interface
class WeArtEffect {
public:
	//! @brief Get temperature value 
	//! @return current effect temperature value
	virtual WeArtTemperature getTemperature() = 0;

	//! @brief Get force value 
	//! @return current effect forcevalue
	virtual WeArtForce getForce() = 0;

	//! @brief Get texture value 
	//! @return current effect texture value
	virtual WeArtTexture getTexture() = 0;
};

//! @brief Basic object to apply effects
//! 
//! The TouchEffect object allows to apply simple effect to an haptic object.
//! The temperature and force values are applied as set in the object, while 
//! the texture velocity is forced to its maximum value if active.
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

	virtual WeArtTemperature getTemperature() override {
		return _temperature;
	}

	virtual WeArtForce getForce() override {
		return _force;
	}

	virtual WeArtTexture getTexture() override {
		return _texture;
	}

private:
	WeArtTemperature _temperature;
	WeArtForce _force;
	WeArtTexture _texture;
};
/**
*	WEART - Force struct
*	https://www.weart.it/
*/

#pragma once
#include "WeArtCommon.h"

//! @brief Force value to be applied to an effect
struct WeArtForce {
public:
	WeArtForce() : active(false), _value(DefaultValue) {};

	WeArtForce(bool active, float force) {
		this->active = active;
		value(force);
	}

	static constexpr float DefaultValue = 0.0f;
	static constexpr float MinValue = 0.0f;
	static constexpr float MaxValue = 1.0f;

	bool active;

	//! @brief Force value getter
	//! @return the force value
	float value() const {
		return _value;
	}

	//! @brief Force value setter
	//! @param force Value to set 
	void value(float force) {
		_value = force <= MinValue ? MinValue : force >= MaxValue ? MaxValue : force;
	}

	bool operator== (const WeArtForce& other) {
		return (active == other.active && _value == other.value());
	};

private:
	float _value;
};

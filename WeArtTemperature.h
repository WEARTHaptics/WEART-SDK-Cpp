#pragma once
#include "WeArtCommon.h"

//! @brief Temperature value to be applied to an effect
struct WeArtTemperature {
public:
	WeArtTemperature() : active(false), _value(DefaultValue) {};

	WeArtTemperature(bool active, float temperature) : active(active) {
		value(temperature);
	}

	static constexpr float DefaultValue = 0.5f;
	static constexpr float MinValue = 0.0f;
	static constexpr float MaxValue = 1.0f;

	bool active;

	//! @brief Temperature value getter
	//! @return the temperature value
	float value() const {
		return _value;
	}

	//! @brief Temperature value setter
	//! @param temperature The temperature value to set
	void value(float temperature) {
		_value = temperature <= MinValue ? MinValue : temperature >= MaxValue ? MaxValue : temperature;
	}

	bool operator==(const WeArtTemperature& other) const {
		return (active == other.active && _value == other.value());
	};

private:
	float _value;
};



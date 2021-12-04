#pragma once

struct WeArtTemperature {

public:
	bool active;
	float value;

	WeArtTemperature() : active(false), value(WeArtConstants::defaultTemperature) {};

	bool operator==(const WeArtTemperature& other) const {
		return (active == other.active && value == other.value);
	};
};



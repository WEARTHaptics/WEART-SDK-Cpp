#pragma once

struct WeArtForce {

public:
	
	bool active;
	float value;

	WeArtForce() : active(false), value(WeArtConstants::defaultForce) {};

	bool operator== (const WeArtForce& other) {
		return (active == other.active && value == other.value);
	};
};

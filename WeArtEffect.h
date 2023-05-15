#pragma once

#include "WeArtTemperature.h"
#include "WeArtForce.h"
#include "WeArtTexture.h"
#include <vector>

//! @brief General effect function interface
class WeArtEffect {
public:
	virtual WeArtTemperature	getTemperature	(void) = 0;
	virtual WeArtForce			getForce		(void) = 0;
	virtual WeArtTexture		getTexture		(void) = 0;
};

#include "pch.h"
#include "WeArtEffect.h"

bool TouchEffect::Set(WeArtTemperature temp, WeArtForce force, WeArtTexture texture) {
	bool changed = false;

	// Temperature
	changed |= !(_temperature == temp);
	_temperature = temp;

	// Force
	changed |= !(_force == force);
	_force = force;

	// Texture
	changed |= !(_texture == texture);
	// forcing Vz texture static
	texture.textureVelocity()[2] = 0.5f;

	_texture = texture;

	return changed;
}
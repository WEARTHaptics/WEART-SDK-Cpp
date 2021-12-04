#pragma once

struct WeArtTexture {
	
	bool active;
	TextureType textureType;
	std::vector<float> textureVelocity;
	float volume;

	WeArtTexture() : active(false),
		textureType((TextureType)WeArtConstants::defaultTextureIndex),
		textureVelocity { WeArtConstants::defaultTextureVelocity[0], WeArtConstants::defaultTextureVelocity[1], WeArtConstants::defaultTextureVelocity[2] },
		volume(WeArtConstants::defaultVolumeTexture)
	{};

	bool operator==(const WeArtTexture& other) const {
		return (active == other.active && textureType == other.textureType && textureVelocity == other.textureVelocity && volume == other.volume);
	};
};


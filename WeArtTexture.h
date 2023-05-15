/**
*	WEART - Texture component
*	https://www.weart.it/
*/
#pragma once
#include <WEART_SDK/WeArtCommon.h>

//! @brief Temperature value to be applied to an effect
struct WeArtTexture {
public:
	WeArtTexture() : active(false),
		_textureType(DefaultTextureType),
		_textureVelocity{ WeArtConstants::defaultTextureVelocity[0], WeArtConstants::defaultTextureVelocity[1], WeArtConstants::defaultTextureVelocity[2] },
		_volume(DefaultVolume) {};

	WeArtTexture(bool active, TextureType type, float vx, float vy, float vz, float textureVolume)
		: active(active), _textureType(type) {
		textureVelocity(vx, vy, vz);
		volume(textureVolume);
	}

	static constexpr TextureType DefaultTextureType = TextureType::ClickNormal;

	static constexpr float DefaultVolume = 100.0f;
	static constexpr float MinVolume = 0.0f;
	static constexpr float MaxVolume = 100.0f;

	static constexpr float MinVelocity = 0.0f;
	static constexpr float MaxVelocity = 0.0f;

	bool active;

	//! @brief Texture velocity getter
	//! @return current texture velocity value
	std::vector<float> textureVelocity() const {
		return _textureVelocity;
	}

	//! @brief Texture velocity setter
	//! @param Vx Texture velocity on X axis
	//! @param Vy Texture velocity on Y axis
	//! @param Vz Texture velocity on Z axis
	void textureVelocity(float Vx, float Vy, float Vz) {
		float vx = Vx <= MinVelocity ? MinVelocity : Vx >= MaxVelocity ? MaxVelocity : Vx;
		float vy = Vy <= MinVelocity ? MinVelocity : Vy >= MaxVelocity ? MaxVelocity : Vy;
		float vz = Vz <= MinVelocity ? MinVelocity : Vz >= MaxVelocity ? MaxVelocity : Vz;
		_textureVelocity = { vx, vy, vz };
	}

	//! @brief Texture type getter
	//! @return current texture type value
	TextureType textureType() const {
		return _textureType;
	}

	//! @brief Texture type setter
	//! @param type Texture type value to set
	void textureType(TextureType type) {
		TextureType lower = TextureType::ClickNormal;
		TextureType upper = TextureType::DoubleSidedTape;
		_textureType = type <= lower ? lower : type >= upper ? upper : type;
	}

	//! @brief Volume value getter
	//! @return current texture volume
	float volume() const {
		return _volume;
	}

	//! @brief Volume value setter
	//! @param v Volume to set
	void volume(float v) {
		_volume = v <= MinVolume ? MinVolume : v >= MaxVolume ? MaxVolume : v;
	}

	bool operator==(const WeArtTexture& other) const {
		return (active == other.active && _textureType == other.textureType() && _textureVelocity == other.textureVelocity() && _volume == other.volume());
	};

private:
	float _volume;
	TextureType _textureType;
	std::vector<float> _textureVelocity;
};


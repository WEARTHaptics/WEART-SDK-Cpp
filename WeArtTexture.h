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
		_textureVelocity{ DefaultVelocity },
		_volume(DefaultVolume) {};

	WeArtTexture(bool active, TextureType type, float velocity, float textureVolume)
		: active(active), _textureType(type) {
		textureVelocity(velocity);
		volume(textureVolume);
	}

	static constexpr TextureType DefaultTextureType = TextureType::ClickNormal;

	static constexpr float DefaultVolume = 100.0f;
	static constexpr float MinVolume = 0.0f;
	static constexpr float MaxVolume = 100.0f;

	static constexpr float DefaultVelocity = 0.0f;
	static constexpr float MinVelocity = 0.0f;
	static constexpr float MaxVelocity = 0.5f;

	bool active;

	//! @brief Gets the texture velocity
	//! @return current texture velocity value
	float textureVelocity() const {
		return _textureVelocity;
	}

	//! @brief Sets the texture velocity (speed at which the vibration is emitted, between 0 and 0.5)
	//! @param velocity Texture velocity
	void textureVelocity(float velocity) {
		_textureVelocity = velocity <= MinVelocity ? MinVelocity : velocity >= MaxVelocity ? MaxVelocity : velocity;
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
	float _textureVelocity;
};


#pragma once

#include "WeArtCommon.h"
#include "WeArtEffect.h"
#include <vector>

#include "WeArtTemperature.h"
#include "WeArtForce.h"
#include "WeArtTexture.h"
#include "WeArtMessages.h"
#include "WeArtClient.h"

//! @brief A haptic object class, representing one or multiple thimbles and hand with which the user interacts.
//!
//! The interaction is represented by the use of effects (WeArtEffect).
//! 
//! The communication with the lower layers is triggered by adding and removing effects
//! (see WeArtHapticObject::AddEffect() and WeArtHapticObject::RemoveEffect()), which subsequently triggers an WeArtHapticObject::UpdateEffects() event.
//!
//! Such event then sends foward messages representing the processed events.
class WeArtHapticObject {
public:
	// Sets default values for this component's properties
	WeArtHapticObject(WeArtClient* client);

	// Hand/finger state variables
	int32  handSideFlag;
	int32 actuationPointFlag;

	// State variables. Serialized.
	WeArtTemperature weArtTemperature;
	WeArtForce weArtForce;
	WeArtTexture weArtTexture;

	// List of effect active on this object
	// Not serialized.
	std::vector<WeArtEffect*> activeEffects;

	void AddEffect(WeArtEffect* effect);
	void RemoveEffect(WeArtEffect* effect);
	bool ContainsEffect(WeArtEffect* effect);
	void UpdateEffects(void);

	void SendMessage(WeArtMessage* msg);

private:
	WeArtClient* weArtclient;
};

#pragma once

#include "WeArtCommon.h"
#include "WeArtEffect.h"
#include <vector>

// Included cause of a bug in UnrealHeaderTool.
// Those are already included in WeArtEffect.h,
// but need to be included there as well.
#include "WeArtTemperature.h"
#include "WeArtForce.h"
#include "WeArtTexture.h"
#include "WeArtMessages.h"


// A haptic object class, representing player-controller objects able to interact
// with objects of the @TouchableObject class.
//
// The communication with the lower layers is triggered by adding and removing effects
// (see @AddEffect() and @RemoveEffect()), which subsequently triggers an @UpdateEffects() event.
//
// Such event then sends foward (to, presumably, the middleware)
// messages representing the processed events
class WeArtHapticObject 
{

public:
	// Sets default values for this component's properties
	WeArtHapticObject();

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
};
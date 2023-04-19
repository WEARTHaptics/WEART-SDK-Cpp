/**
*	WEART - Tracking thimble component
*	https://www.weart.it/
*/
#pragma once

#include "WeArtCommon.h"
#include "WeArtMessages.h"
#include "WeArtMessageListener.h"

class WeArtThimbleTrackingObject : public WeArtMessageListener
{
public:

	WeArtThimbleTrackingObject(HandSide handSide, ActuationPoint actuationPoint);

	void OnMessageReceived(WeArtMessage *msg) override;

	float GetClosure() const { return Closure; }


protected:

	HandSide handSide;

	ActuationPoint actuationPoint;
	
	float Closure;
};


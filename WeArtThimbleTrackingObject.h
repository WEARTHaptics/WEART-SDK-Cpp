#pragma once

#include "WeArtCommon.h"
#include "WeArtMessages.h"

class WeArtThimbleTrackingObject
{

public:

	WeArtThimbleTrackingObject(HandSide handSide, ActuationPoint actuationPoint);

	void OnMessageReceived(WeArtMessage *msg);

	float GetClosure();


protected:

	HandSide handSide;

	ActuationPoint actuationPoint;
	
	float Closure;
};


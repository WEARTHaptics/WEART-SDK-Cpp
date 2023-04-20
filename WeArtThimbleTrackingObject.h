/**
*	WEART - Tracking thimble component
*	https://www.weart.it/
*/
#pragma once

#include "WeArtCommon.h"
#include "WeArtMessages.h"
#include "WeArtMessageListener.h"

//! @brief Object used to track a single thimble object and its closure value
class WeArtThimbleTrackingObject : public WeArtMessageListener {
public:
	WeArtThimbleTrackingObject(HandSide handSide, ActuationPoint actuationPoint);

	//! @copydoc WeArtMessageListener::OnMessageReceived
	void OnMessageReceived(WeArtMessage* msg) override;

	//! @brief Get last received thimble closure
	//! @return last received thimble closure
	float GetClosure() const {
		return Closure;
	}
protected:

	HandSide handSide;

	ActuationPoint actuationPoint;

	float Closure;
};


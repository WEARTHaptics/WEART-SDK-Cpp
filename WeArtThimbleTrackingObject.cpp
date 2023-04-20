/**
*	WEART - Tracking thimble component
*	https://www.weart.it/
*/

#include "pch.h"
#include "WeArtThimbleTrackingObject.h"
#include "WeArtController.h"

WeArtThimbleTrackingObject::WeArtThimbleTrackingObject(HandSide side, ActuationPoint actuation)
	: WeArtMessageListener({ TrackingMessage::ID }) {
	handSide = side;
	actuationPoint = actuation;
	Closure = 0.0f;
}

void WeArtThimbleTrackingObject::OnMessageReceived(WeArtMessage* msg) {
	if (msg->getID() == TrackingMessage::ID) {
		TrackingMessage* trackingMsg = static_cast<TrackingMessage*>(msg);
		Closure = trackingMsg->GetClosure(handSide, actuationPoint);
	}
}

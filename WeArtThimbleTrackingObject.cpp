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
	_closure = 0.0f;
}

void WeArtThimbleTrackingObject::OnMessageReceived(WeArtMessage* msg) {
	if (msg->getID() == TrackingMessage::ID) {
		TrackingMessage* trackingMsg = static_cast<TrackingMessage*>(msg);
		_closure = trackingMsg->GetClosure(handSide, actuationPoint);
		_abduction = trackingMsg->GetAbduction(handSide, actuationPoint);
	}
}

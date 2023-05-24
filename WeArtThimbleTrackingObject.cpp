/**
*	WEART - Tracking thimble component
*	https://www.weart.it/
*/

#include "WeArtThimbleTrackingObject.h"

WeArtThimbleTrackingObject::WeArtThimbleTrackingObject(HandSide side, ActuationPoint actuation)
	: WeArtMessageListener({ TrackingMessage::ID }) {
	handSide = side;
	actuationPoint = actuation;
	_closure = WeArtConstants::defaultClosure;
	_abduction = WeArtConstants::defaultAbduction;
}

void WeArtThimbleTrackingObject::OnMessageReceived(WeArtMessage* msg) {
	if (msg->getID() == TrackingMessage::ID) {
		TrackingMessage* trackingMsg = static_cast<TrackingMessage*>(msg);
		_closure = trackingMsg->GetClosure(handSide, actuationPoint);
		_abduction = trackingMsg->GetAbduction(handSide, actuationPoint);
	}
}

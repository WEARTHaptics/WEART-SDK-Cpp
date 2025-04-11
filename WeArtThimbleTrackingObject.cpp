/**
*	WEART - Tracking thimble component
*	https://www.weart.it/
*/

#include "WeArtThimbleTrackingObject.h"

WeArtThimbleTrackingObject::WeArtThimbleTrackingObject(HandSide side, ActuationPoint actuation)
	: WeArtMessageListener({ TrackingMessage::ID, TrackingBendingG2Message::ID }) {
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
	else if (msg->getID() == TrackingBendingG2Message::ID) {
		TrackingBendingG2Message* trackingMsg = static_cast<TrackingBendingG2Message*>(msg);
		// Update tracking data only if the hand matches
		if(trackingMsg->GetHandSide() == this->handSide)
		{
			_closure = trackingMsg->GetClosure(handSide, actuationPoint);
			_abduction = trackingMsg->GetAbduction(handSide, actuationPoint);
		}
		else
		{
			// Do not update tracking data
		}
	}
}
